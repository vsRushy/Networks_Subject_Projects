#include "Audio.h"

bool Audio::PlayWindowsSound(const char* relativeFilePath)
{
	return PlaySound(relativeFilePath, NULL, SND_ASYNC);
}