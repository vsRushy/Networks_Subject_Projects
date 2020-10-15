/* Sockets UDP - Client */

#pragma comment (lib, "ws2_32.lib")

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#define IP "127.0.0.1"
#define PORT 8000

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

void PrintWSErrorAndExit(const char* msg)
{
	wchar_t* s = NULL;

	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s,
		0, NULL);

	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);

	system("pause");
	exit(-1);
}

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

		iResult = sendto(s, buffer, buffer_len, 0, (const struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
		if (iResult == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't send data.");
		}

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
	iResult = shutdown(s, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't shutdown socket.");
	}

	iResult = closesocket(s);
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't close socket.");
	}

	iResult = WSACleanup();
	if (iResult != NO_ERROR)
	{
		PrintWSErrorAndExit("Can't cleanup sockets library.");
	}

	return 0;
}