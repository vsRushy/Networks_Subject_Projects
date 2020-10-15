/* Sockets TCP - Server */

#pragma comment (lib, "ws2_32.lib")

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

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

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
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

	int max_simultaneous_connections = 1;
	iResult = listen(s, max_simultaneous_connections);
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't listen.");
	}

	sockaddr_in remoteAddr;
	int remote_size = sizeof(remoteAddr);
	SOCKET cs = accept(s, (sockaddr*)&remoteAddr, &remote_size);
	if (cs == INVALID_SOCKET)
	{
		PrintWSErrorAndExit("Can't accept socket.");
	}

	char buffer[5];
	int buffer_len = 5 * sizeof(char);
	while (true)
	{
		iResult = recv(cs, buffer, buffer_len, 0);
		if (iResult == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't receive data.");
		}

		std::cout << buffer << std::endl;

		memcpy(&buffer, "pong", buffer_len);

		iResult = send(s, buffer, buffer_len, 0);
		if (iResult == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't send data.");
		}

		Sleep(500);
	}

	// -----------------------------------------------------------

	// Cleanup
	shutdown(s, SD_BOTH);
	if (iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't shutdown socket.");
	}

	closesocket(s);
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