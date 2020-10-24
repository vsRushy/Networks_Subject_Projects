/* Sockets TCP - Client */

#pragma comment (lib, "ws2_32.lib")

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define IP "127.0.0.1"
#define PORT 8000

#include "helper.h"
#define MAX_SIMUL_CONNECTIONS 100

bool  ModuleNetworkingClient::start(const char * serverAddressStr, int serverPort, const char *pplayerName)
{
	playerName = pplayerName;

	// TODO(jesus): TCP connection stuff
	// - Create the socket
	// - Create the remote address object
	// - Connect to the remote address
	// - Add the created socket to the managed list of sockets using addSocket()

	// If everything was ok... change the state
	// Initialization

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't initialize sockets library.");
	}

	// -----------------------------------------------------------


	ModuleNetworkingClient::socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (ModuleNetworkingClient::socket == INVALID_SOCKET)
	{
		PrintWSErrorAndExit("Can't create TCP socket.");
	}

	struct sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(PORT);
	const char* remoteAddrStr = IP;
	inet_pton(AF_INET, remoteAddrStr, &remoteAddr.sin_addr);

	iResult = connect(ModuleNetworkingClient::socket, (const sockaddr*)&remoteAddr, sizeof(remoteAddr));
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't connect socket.");
	}

	addSocket(ModuleNetworkingClient::socket);

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

		if ((send(ModuleNetworkingClient::socket, playerName.c_str(), playerName.length(), 0)) == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't connect socket.");
		}

	}

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

		ImGui::Text("%s connected to the server...", playerName.c_str());

		ImGui::End();
	}

	return true;
}

void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, byte * data)
{
	state = ClientState::Stopped;
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	state = ClientState::Stopped;
}

