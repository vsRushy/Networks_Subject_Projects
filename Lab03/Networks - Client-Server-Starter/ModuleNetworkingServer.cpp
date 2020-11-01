#include "ModuleNetworkingServer.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// ModuleNetworkingServer public methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::start(int port)
{

	// commands mapped with functions
	commandMap.insert(std::make_pair("/help", &ModuleNetworkingServer::Help));
	commandMap.insert(std::make_pair("/kick", &ModuleNetworkingServer::Kick));
	commandMap.insert(std::make_pair("/list", &ModuleNetworkingServer::List));
	commandMap.insert(std::make_pair("/whisper", &ModuleNetworkingServer::Whisper));
	commandMap.insert(std::make_pair("/change_name", &ModuleNetworkingServer::ChangeName));

	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	// - Set address reuse
	// - Bind the socket to a local interface
	// - Enter in listen mode
	// - Add the listenSocket to the managed list of sockets using addSocket()

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		reportError("Can't create socket.");
		return false;
	}

	int enable = 1;
	if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int)) == SOCKET_ERROR)
	{
		reportError("Can't set socket options.");
		return false;
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.S_un.S_addr = ADDR_ANY;

	if (bind(listenSocket, (const sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		reportError("Can't bind socket.");
		return false;
	}

	int simultaneousConnections = 5;
	if (listen(listenSocket, simultaneousConnections) == SOCKET_ERROR)
	{
		reportError("Socket can't listen.");
		return false;
	}

	addSocket(listenSocket);

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

		// Sound
		OutputMemoryStream sound_packet;
		sound_packet << ServerMessage::WelcomeSound;

		for (auto& connectedSocket : connectedSockets)
			sendPacket(sound_packet, connectedSocket.socket);
	
	    // Message
		OutputMemoryStream packet_o;

		for (auto& connectedSocket : connectedSockets)
		{


			if (connectedSocket.socket == socket)
			{
				connectedSocket.playerName = playerName;

				packet_o << ServerMessage::Welcome;
				packet_o << "**************************************************\n"
							"               WELCOME TO THE CHAT\n"
							"Please type /help to see the available commands.\n"
							"**************************************************";
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

		// Sound
		OutputMemoryStream sound_packet;
		sound_packet << ServerMessage::ChatSound;

		for (auto& connectedSocket : connectedSockets)
			sendPacket(sound_packet, connectedSocket.socket);

		// check if the first word is a command
		std::istringstream stream(message);
		std::string word1; // command
		std::string word2; // dest player
		std::string word3; // message

		stream >> word1 >> word2 >> word3; // split message into words 

		/* Command messages */
		if ((word1.at(0) == '/') && (commandMap.find(word1) != commandMap.end())) // found command in word1
			
		{
			if((this->*commandMap.at(word1))(socket, word2, word3) == false) // call the function in the map
				break;
		
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
				}
			}

			// Then send the packet to all connectedSockets, which are all connected users.
			for (const auto& connectedSocket : connectedSockets)
			{
				OutputMemoryStream packet_o;

				packet_o << ServerMessage::Chat;
				packet_o << fromConnectedSocketSender.playerName + ": " + message;

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

void ModuleNetworkingServer::onSocketDisconnected(SOCKET socket, std::string kicker)
{
	// Packet to other users
	OutputMemoryStream global_packet;
	std::string message;

	// erase
	for (auto it = connectedSockets.begin(); it != connectedSockets.end(); ++it)
	{
		auto &connectedSocket = *it;
		if (connectedSocket.socket == socket)
		{
			// Packet to disconnected user
			OutputMemoryStream individual_packet;
			global_packet << ServerMessage::Disconnect;
			sendPacket(individual_packet, connectedSocket.socket);

			// capture the name
			message = connectedSocket.playerName;

			// erase it
			connectedSockets.erase(it);
			break;
		}
	}

	// disconnect notification
	if (kicker.empty())
	{
		global_packet << ServerMessage::UserDisconnected;
		message += " has disconnected :(";
		global_packet << message.c_str();
	}
	else // kick notification
	{
		global_packet << ServerMessage::UserKicked;
		message += " has been kicked by ";
		message += kicker;
	}

	global_packet << message.c_str();

	for (auto& connectedSocket : connectedSockets)
		sendPacket(global_packet, connectedSocket.socket);

}

bool ModuleNetworkingServer::Help(SOCKET& socket, std::string p2, std::string message)
{
	bool ret = true;

	OutputMemoryStream packet_o;
	packet_o << ServerMessage::Help;
	packet_o << "*******************Commands list******************\n"
		"/help\n"
		"/kick [username]\n"
		"/list\n"
		"/whisper [username] [message]\n"
		"change_name [username]\n";

	if (!sendPacket(packet_o, socket))
	{
		disconnect();
		state = ServerState::Stopped;

		ret = false;
	}

	return ret;
}

bool ModuleNetworkingServer::Kick(SOCKET& socket, std::string p2, std::string message) // socket is the origin and p2 the kicked player
{
	bool ret = true;

	int kicker = 666, kicked = 666;
	for (int i = 0; i < connectedSockets.size(); ++i)
	{
		if (connectedSockets.at(i).playerName == p2)
			kicked = i;
		else if (connectedSockets.at(i).socket == socket)
			kicker = i;
 
	}

	onSocketDisconnected(connectedSockets.at(kicked).socket, connectedSockets.at(kicker).playerName); 

	return ret;
}

bool ModuleNetworkingServer::List(SOCKET& socket, std::string p2, std::string message)
{
	bool ret = true;

	return ret;
}

bool ModuleNetworkingServer::Whisper(SOCKET& socket, std::string p2, std::string message)
{
	bool ret = true;

	return ret;
}

bool ModuleNetworkingServer::ChangeName(SOCKET& socket, std::string p2, std::string message)
{
	bool ret = true;

	return ret;
}

