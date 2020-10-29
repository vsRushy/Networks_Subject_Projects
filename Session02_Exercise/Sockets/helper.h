#pragma comment (lib, "ws2_32.lib")

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

void CleanUpSockets(SOCKET& s, int* iResult)
{
	*iResult = shutdown(s, SD_BOTH);
	if (*iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't shutdown socket.");
	}

	*iResult = closesocket(s);
	if (*iResult == SOCKET_ERROR)
	{
		PrintWSErrorAndExit("Can't close socket.");
	}

	*iResult = WSACleanup();
	if (*iResult != NO_ERROR)
	{
		PrintWSErrorAndExit("Can't cleanup sockets library.");
	}
}