/* Sockets UDP - Client */

#pragma comment (lib, "ws2_32.lib")

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

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

	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
	{
		PrintWSErrorAndExit("Can't create UDP socket.");
	}

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(PORT);
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	int enable = 1;
	iResult = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't set socket options.");
	}

	iResult = bind(s, (const struct sockaddr*)&bindAddr, sizeof(bindAddr));
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't bind socket.");
	}

	char buffer[5];
	int buffer_len = 5 * sizeof(char);
	while (true)
	{
		sockaddr_in fromAddr;
		int from_size = sizeof(fromAddr);

		iResult = recvfrom(s, buffer, buffer_len, 0, (sockaddr*)&fromAddr, &from_size);
		if (iResult == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't receive data.");
		}

		std::cout << buffer << std::endl;

		memcpy(&buffer, "pong", buffer_len);

		iResult = sendto(s, buffer, buffer_len, 0, (const struct sockaddr*)&fromAddr, from_size);
		if (iResult == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't send data.");
		}

		Sleep(500);
	}

	// -----------------------------------------------------------

	// Cleanup
	CleanUpSockets(s, &iResult);

	return 0;
}