/* Sockets UDP - Client */

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


	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
	{
		PrintWSErrorAndExit("Can't create UDP socket.");
	}

	struct sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(PORT);
	const char* remoteAddrStr = IP;
	inet_pton(AF_INET, remoteAddrStr, &remoteAddr.sin_addr);

	char buffer[5];
	int buffer_len = 5 * sizeof(char);
	while (true)
	{
		memcpy(&buffer, "ping", buffer_len);

		// Send to server
		iResult = sendto(s, buffer, buffer_len, 0, (const struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
		if (iResult == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't send data.");
		}

		// receive from server
		sockaddr_in fromAddr;
		int from_len = sizeof(fromAddr);
		iResult = recvfrom(s, buffer, buffer_len, 0, (struct sockaddr*)&fromAddr, &from_len);
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