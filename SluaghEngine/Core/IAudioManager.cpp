#include <IAudioManager.h>
#include "AudioManager.h"

DECLDIR_CORE SE::Core::IAudioManager * SE::Core::CreateAudioManager(const IAudioManager::InitializationInfo & initInfo)
{
	return new AudioManager(initInfo);
}
