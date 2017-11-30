#include <Audio\IAudioHandler.h>
#include "AudioHandler.h"

DECLDIR SE::Audio::IAudioHandler * SE::Audio::CreateNewAudioHandler()
{
	return new AudioHandler;
}