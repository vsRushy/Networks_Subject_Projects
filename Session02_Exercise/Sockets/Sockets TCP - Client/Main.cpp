/* Sockets TCP - Client */

#pragma comment (lib, "ws2_32.lib")

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define IP "127.0.0.1"
#define PORT 8000

#include "..\helper.h"

int main(int argc, char* argv[])
{
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

	struct sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(PORT);
	const char* remoteAddrStr = IP;
	inet_pton(AF_INET, remoteAddrStr, &remoteAddr.sin_addr);

	iResult = connect(s, (const sockaddr*)&remoteAddr, sizeof(remoteAddr));
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't connect socket.");
	}

	char buffer[5];
	int buffer_len = 5 * sizeof(char);
	for(unsigned int i = 0; i < 5; ++i)
	{
		memcpy(&buffer, "ping", buffer_len);

		// Send to server
		iResult = send(s, buffer, buffer_len, 0);
		if (iResult == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't send data.");
		}


		// receive from server
		iResult = recv(s, buffer, buffer_len, 0);
		if (iResult == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't receive data.");
		}

		std::cout << buffer << std::endl;

		Sleep(500);
	}

	// -----------------------------------------------------------

	// Cleanup
	CleanUpSockets(s, &iResult);

	return 0;
}