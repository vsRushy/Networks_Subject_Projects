#pragma once

#include <iostream>
#include <Windows.h>

namespace Audio
{
	bool PlayWindowsSound(const char* relativeFilePath);
};



/*static bool MakeBeep()
{
	Beep()
}*/