/* Sockets TCP - Client */

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


	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		PrintWSErrorAndExit("Can't create UDP socket.");
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

		iResult = send(s, buffer, buffer_len, 0);
		if (iResult == SOCKET_ERROR)
		{
			PrintWSErrorAndExit("Can't send data.");
		}

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