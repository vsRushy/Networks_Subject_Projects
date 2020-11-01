#pragma once

#include <iostream>
#include <Windows.h>
#include <map>

#pragma comment(lib, "Winmm.lib")

namespace Audio
{
	static std::map<std::string, std::string> audioMap =
	{
		{"Welcome", "Audio/userConnected.wav"},
		{"Chat", "Audio/message.wav"},
    	{"UserDisconnected", "Audio/disconnect.wav"}
	};

	bool PlayWindowsSound(const char* relativeFilePath);
};



/*static bool MakeBeep()
{
	Beep()
}*/