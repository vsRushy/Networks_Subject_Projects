#include "ModuleNetworkingServer.h"
#include "Helper.h"


#pragma comment (lib, "ws2_32.lib")

#define PORT 8000
#define MAX_SIMUL_CONNECTIONS 100

//////////////////////////////////////////////////////////////////////
// ModuleNetworkingServer public methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::start(int port)
{
	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	// - Set address reuse
	// - Bind the socket to a local interface
	// - Enter in listen mode
	// - Add the listenSocket to the managed list of sockets using addSocket()

	// Initialization
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't initialize sockets library.");
	}

	// -----------------------------------------------------------

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		PrintWSErrorAndExit("Can't create TCP socket.");
	}

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(PORT);
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	int enable = 1;
	iResult = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	if (iResult == SOCKET_ERROR)
		PrintWSErrorAndExit("Can't set socket options.");

	iResult = bind(s, (const sockaddr*)&bindAddr, sizeof(bindAddr));
	if (iResult == SOCKET_ERROR)
		PrintWSErrorAndExit("Can't bind socket.");

	iResult = listen(s, MAX_SIMUL_CONNECTIONS); // Is this already defined somewhere? 
	if (iResult == SOCKET_ERROR)
		PrintWSErrorAndExit("Can't bind socket.");

	addSocket(s);

	state = ServerState::Listening;

	return true;
}

bool ModuleNetworkingServer::isRunning() const
{
	return state != ServerState::Stopped;
}



//////////////////////////////////////////////////////////////////////
// Module virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::update()
{
	return true;
}

bool ModuleNetworkingServer::gui()
{
	if (state != ServerState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Server Window");

		Texture *tex = App->modResources->server;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("List of connected sockets:");

		for (auto &connectedSocket : connectedSockets)
		{
			ImGui::Separator();
			ImGui::Text("Socket ID: %d", connectedSocket.socket);
			ImGui::Text("Address: %d.%d.%d.%d:%d",
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b1,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b2,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b3,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b4,
				ntohs(connectedSocket.address.sin_port));
			ImGui::Text("Player name: %s", connectedSocket.playerName.c_str());
		}

		ImGui::End();
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
// ModuleNetworking virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::isListenSocket(SOCKET socket) const
{
	return socket == listenSocket;
}

void ModuleNetworkingServer::onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress)
{
	// Add a new connected socket to the list
	ConnectedSocket connectedSocket;
	connectedSocket.socket = socket;
	connectedSocket.address = socketAddress;
	connectedSockets.push_back(connectedSocket);
}

void ModuleNetworkingServer::onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet)
{
	// Set the player name of the corresponding connected socket proxy
	ClientMessage clientMessage;
	packet >> clientMessage;

	switch(clientMessage)
	{
	case ClientMessage::Hello:
	{
		std::string playerName;
		packet >> playerName;

		// First check if username exists.
		for (const auto& connectedSocket : connectedSockets)
		{
			if (connectedSocket.playerName == playerName)
			{
				OutputMemoryStream packet_o;
				packet_o << ServerMessage::NonWelcome << playerName;

				if (!sendPacket(packet_o, socket))
				{
					disconnect();
					state = ServerState::Stopped;

					break;
				}

				return; // Exit function
			}
		}

		for (auto& connectedSocket : connectedSockets)
		{
			OutputMemoryStream packet_o;

			if (connectedSocket.socket == socket)
			{
				connectedSocket.playerName = playerName;

				packet_o << ServerMessage::Welcome << 
					"**************************************************\n"
					"               WELCOME TO THE CHAT\n"
					"Please type /help to see the available commands.\n"
					"**************************************************";
			}
			else
			/* Notify and send to al clients the new user that has joined. */
			{
				std::string message = "********* " + playerName + " joined *********";
				packet_o << ServerMessage::ClientConnected << message;
			}

			if (!sendPacket(packet_o, connectedSocket.socket))
			{
				disconnect();
				state = ServerState::Stopped;

				break;
			}
		}

		break;
	}

	case ClientMessage::Chat:
	{
		std::string message;
		packet >> message;

		/* Command messages */
		if (message.compare("/help") == 0)
		{
			OutputMemoryStream packet_o;
			packet_o << ServerMessage::Help << 
				"****************** Commands list *****************\n"
				"/help\n"
				"/list\n"
				"/kick [username]\n"
				"/whisper [username] [message]\n"
				"/change_name [username]\n"
				"/change_color [r] [g] [b] [a] with value range being [0:1]\n"
				"/rps (rock paper scissors)\n";
			
			if (!sendPacket(packet_o, socket))
			{
				disconnect();
				state = ServerState::Stopped;

				break;
			}
		}
		else if (message.compare("/list") == 0)
		{
			std::list<std::string> users;
			std::string user_list = "********* User list *********";
			for (const auto& connectedSocket : connectedSockets)
			{
				users.push_back(connectedSocket.playerName);
			}

			users.sort();
			
			for (const auto& user : users)
			{
				user_list.append("\n").append(user);
			}

			OutputMemoryStream packet_o;
			packet_o << ServerMessage::List << user_list;

			if (!sendPacket(packet_o, socket))
			{
				disconnect();
				state = ServerState::Stopped;
			}
		}
		else if (message.find("/kick") == 0)
		{
			std::string playerName = message.substr(KICK_COMMAND_OFFSET);
			for (const auto& connectedSocket : connectedSockets)
			{
				if (connectedSocket.playerName == playerName)
				{
					OutputMemoryStream packet_o;
					packet_o << ServerMessage::Disconnect;

					if (!sendPacket(packet_o, connectedSocket.socket))
					{
						disconnect();
						state = ServerState::Stopped;
					}

					break;
				}
			}
		}
		else if (message.find("/whisper") == 0)
		{
			std::istringstream words(message);
			std::string word;
			std::vector<std::string> word_list;

			while (words >> word)
			{
				word_list.push_back(word);
			}

			// Index of the player is at 1 (command [0], playername[1], message[2])
			std::string playerName = word_list[1];
			std::string sentence;

			// Index of the message starts at 2 (command [0], playername[1], message[2])
			for (int i = 2; i < word_list.size(); ++i)
			{
				sentence.append(word_list[i]).append(" ");
			}

			// First get the socket from who is sending the message and store it.
			ConnectedSocket fromConnectedSocketSender;
			for (const auto& connectedSocket : connectedSockets)
			{
				if (connectedSocket.socket == socket)
				{
					fromConnectedSocketSender = connectedSocket;
					
					break;
				}
			}

			// Send message to the sender and receiver clients.
			for (const auto& connectedSocket : connectedSockets)
			{
				if (connectedSocket.playerName == playerName)
				{
					OutputMemoryStream packet_o;
					packet_o << ServerMessage::Whisper <<
						std::string(fromConnectedSocketSender.playerName).append(" whispers to ").
						append(connectedSocket.playerName).append(": ").append(sentence);

					if (!sendPacket(packet_o, fromConnectedSocketSender.socket) || !sendPacket(packet_o, connectedSocket.socket))
					{
						disconnect();
						state = ServerState::Stopped;
					}
				}
			}
		}
		else if (message.find("/change_name") == 0)
		{
			std::string playerName = message.substr(CHANGE_NAME_COMMAND_OFFSET);

			// First check if username already exists.
			for (const auto& connectedSocket : connectedSockets)
			{
				if (connectedSocket.playerName == playerName)
				{
					OutputMemoryStream packet_o;
					packet_o << ServerMessage::AlreadyUsedName << "There is already an user with the name " + playerName;

					if (!sendPacket(packet_o, socket))
					{
						disconnect();
						state = ServerState::Stopped;
					}
				}
			}

			// Send new username.
			for (auto& connectedSocket : connectedSockets)
			{
				if (connectedSocket.socket == socket)
				{
					connectedSocket.playerName = playerName;

					OutputMemoryStream packet_o;
					packet_o << ServerMessage::ChangeClientName << playerName;

					if (!sendPacket(packet_o, connectedSocket.socket))
					{
						disconnect();
						state = ServerState::Stopped;
					}
				}
			}
		}
		else if (message.find("/clear") == 0)
		{
			OutputMemoryStream packet_o;
			packet_o << ServerMessage::Clear;

			if (!sendPacket(packet_o, socket))
			{
				disconnect();
				state = ServerState::Stopped;
			}
		}
		else if (message.find("/change_color") == 0)
		{
			std::istringstream words(message);
			std::string word;
			std::vector<std::string> word_list;

			while (words >> word)
			{
				word_list.push_back(word);
			}

			// command [0] R [1] G [2] B[3] A[4]
			Color playerColor(std::stod(word_list[1]),
				std::stod(word_list[2]),
				std::stod(word_list[3]),
				std::stod(word_list[4]));
			
			for (auto& connectedSocket : connectedSockets)
			{
				if (connectedSocket.socket == socket)
				{
					connectedSocket.playerColor = playerColor;

					OutputMemoryStream packet_o;
					packet_o << ServerMessage::ChangeClientColor <<
						playerColor.r << playerColor.g << playerColor.b << playerColor.a;

					if (!sendPacket(packet_o, connectedSocket.socket))
					{
						disconnect();
						state = ServerState::Stopped;
					}
				}
			}
		}
		else if (message.find("/rps") == 0)
		{
			std::vector<std::string> rps = { "ROCK", "PAPER", "SCISSORS" };
			int option = rand() % 3;

			// First get the socket from who is sending the message and store it.
			ConnectedSocket fromConnectedSocketSender;
			for (const auto& connectedSocket : connectedSockets)
			{
				if (connectedSocket.socket == socket)
				{
					fromConnectedSocketSender = connectedSocket;

					break;
				}
			}

			// Then send the packet to all connectedSockets, which are all connected users.
			for (const auto& connectedSocket : connectedSockets)
			{
				OutputMemoryStream packet_o;
				packet_o << ServerMessage::RockPaperScissors << fromConnectedSocketSender.playerName + " played: " + rps[option];

				if (!sendPacket(packet_o, connectedSocket.socket))
				{
					disconnect();
					state = ServerState::Stopped;
				}
			}
		}
		/* Invalid command */
		else if (message.find("/") == 0)
		{
			OutputMemoryStream packet_o;
			packet_o << ServerMessage::InvalidCommand <<
				"Unknown command. Please enter /help for more information.";

			if (!sendPacket(packet_o, socket))
			{
				disconnect();
				state = ServerState::Stopped;
			}
		}
		else
		/* Message without command */
		{
			// First get the socket from who is sending the message and store it.
			ConnectedSocket fromConnectedSocketSender;
			for (const auto& connectedSocket : connectedSockets)
			{
				if (connectedSocket.socket == socket)
				{
					fromConnectedSocketSender = connectedSocket;

					break;
				}
			}

			// Then send the packet to all connectedSockets, which are all connected users.
			for (const auto& connectedSocket : connectedSockets)
			{
				OutputMemoryStream packet_o;

				packet_o << ServerMessage::Chat << fromConnectedSocketSender.playerName + ": " + message <<
					fromConnectedSocketSender.playerColor.r << fromConnectedSocketSender.playerColor.g <<
					fromConnectedSocketSender.playerColor.b << fromConnectedSocketSender.playerColor.a;

				if (!sendPacket(packet_o, connectedSocket.socket))
				{
					disconnect();
					state = ServerState::Stopped;

					break;
				}
			}
		}

		break;
	}

	default:
	{
		break;
	}
	}
}

void ModuleNetworkingServer::onSocketDisconnected(SOCKET socket)
{
	// Remove the connected socket from the list
	for (auto it = connectedSockets.begin(); it != connectedSockets.end(); ++it)
	{
		auto &connectedSocket = *it;
		if (connectedSocket.socket == socket)
		{
			std::string playerName = connectedSocket.playerName;

			connectedSockets.erase(it);

			// Send and notify to all clients
			for (const auto& connectedSocketC : connectedSockets)
			{
				OutputMemoryStream packet_o;

				std::string message = "********** " + playerName + " left **********";
				packet_o << ServerMessage::ClientDisconnected;
				packet_o << message;

				if (!sendPacket(packet_o, connectedSocketC.socket))
				{
					disconnect();
					state = ServerState::Stopped;
				}
			}

			break;
		}
	}
}

