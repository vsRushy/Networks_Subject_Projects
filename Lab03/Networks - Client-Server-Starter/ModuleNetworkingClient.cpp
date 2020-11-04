#include "ModuleNetworkingClient.h"


bool  ModuleNetworkingClient::start(const char * serverAddressStr, int serverPort, const char *pplayerName)
{
	playerName = pplayerName;

	// TODO(jesus): TCP connection stuff
	// - Create the socket
	// - Create the remote address object
	// - Connect to the remote address
	// - Add the created socket to the managed list of sockets using addSocket()

	// If everything was ok... change the state

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		reportError("Can't create socket.");
		return false;
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverAddressStr, &serverAddress.sin_addr);

	if (connect(s, (const sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		reportError("Can't connect.");
		return false;
	}

	addSocket(s);

	state = ClientState::Start;

	return true;
}

bool ModuleNetworkingClient::isRunning() const
{
	return state != ClientState::Stopped;
}

bool ModuleNetworkingClient::update()
{
	if (state == ClientState::Start)
	{
		// TODO(jesus): Send the player name to the server
		OutputMemoryStream packet;
		packet << ClientMessage::Hello;
		packet << playerName;

		if (sendPacket(packet, s))
		{
			state = ClientState::Logging;
		}
		else
		{
			disconnect();
			state = ClientState::Stopped;
		}
	}

	return true;
}

bool ModuleNetworkingClient::cleanUp()
{
	messages.clear();

	return true;
}

bool ModuleNetworkingClient::gui()
{
	if (state != ClientState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Client Window");

		Texture *tex = App->modResources->client;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("Welcome to the chat %s!", playerName.c_str());

		ImGui::SameLine();
		if (ImGui::Button("Logout"))
		{
			disconnect();
			state = ClientState::Stopped;
			messages.clear();
		}

		ImGui::Spacing();

		ImGui::BeginChild("Chat Zone", ImVec2(430.0f, 468.0f), true);
		for (const Message& message : messages)
		{
			ImGui::TextColored(ImVec4(message.color.r, message.color.g, message.color.b, message.color.a), "%s", message.message.c_str());
		}
		ImGui::EndChild();

		char text_to_send[Kilobytes(1)] = "";
		if (ImGui::InputText("Line", text_to_send, IM_ARRAYSIZE(text_to_send), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			OutputMemoryStream packet_o;
			packet_o << ClientMessage::Chat;
			packet_o << text_to_send;

			if (!sendPacket(packet_o, s))
			{
				disconnect();
				state = ClientState::Stopped;
			}

			ImGui::SetKeyboardFocusHere(-1);
		}
		
		ImGui::End();
	}

	return true;
}

void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	ServerMessage serverMessage;
	packet >> serverMessage;

	switch (serverMessage)
	{
	case ServerMessage::Welcome:
	{
		std::string message;
		packet >> message;

		messages.push_back(Message(message, Color(0.0f, 1.0f, 1.0f, 1.0f)));

		break;
	}

	case ServerMessage::NonWelcome:
	{
		std::string messagePlayerName;
		packet >> messagePlayerName;

		LOG("The user with the name %s already exists.", messagePlayerName.c_str());

		disconnect();
		state = ClientState::Stopped;

		break;
	}

	case ServerMessage::Chat:
	{
		std::string message;
		packet >> message;

		messages.push_back(Message(message, playerColor));

		break;
	}

	case ServerMessage::Help:
	{
		std::string message;
		packet >> message;

		messages.push_back(Message(message, Color(0.8f, 0.8f, 0.8f, 1.0f)));

		break;
	}

	case ServerMessage::List:
	{
		std::string message;
		packet >> message;

		messages.push_back(Message(message, Color(0.8f, 0.2f, 0.5f, 1.0f)));

		break;
	}

	case ServerMessage::Disconnect:
	{
		disconnect();
		state = ClientState::Stopped;

		break;
	}

	case ServerMessage::Whisper:
	{
		std::string message;
		packet >> message;

		messages.push_back(Message(message, Color(0.3f, 0.3f, 0.3f, 1.0f)));

		break;
	}

	case ServerMessage::ChangeClientName:
	{
		packet >> playerName;

		break;
	}

	case ServerMessage::InvalidCommand:
	{
		std::string message;
		packet >> message;

		messages.push_back(Message(message, Color(1.0f, 0.0f, 0.0f, 1.0f)));

		break;
	}

	case ServerMessage::Clear:
	{
		messages.clear();

		break;
	}

	case ServerMessage::ChangeClientColor:
	{
		packet >> playerColor.r >> playerColor.g >> playerColor.b >> playerColor.a;

		break;
	}

	case ServerMessage::ClientConnected:
	{
		std::string message;
		packet >> message;

		messages.push_back(Message(message, Color(1.0f, 1.0f, 0.5f, 1.0f)));

		break;
	}

	case ServerMessage::ClientDisconnected:
	{
		std::string message;
		packet >> message;

		messages.push_back(Message(message, Color(0.5f, 0.0f, 1.0f, 1.0f)));

		break;
	}

	default:
	{
		break;
	}
	}
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	state = ClientState::Stopped;
}

