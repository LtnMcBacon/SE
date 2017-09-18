#pragma once
#ifndef SE_AUDIO_AUDIOMANAGER_H
#define SE_AUDIO_AUDIOMANAGER_H
#include "AudioStream.h"
#include "AudioSound.h"
#include <Utilz\GUID.h>

#include <map>

namespace SE {
	namespace Audio {

		class AudioManager
		{
		public:
			AudioManager();
			~AudioManager();
			/**
			* @brief	Calls init functions in stream and sound
			*
			* @details	Calls and makes sure that both stream and sound gets initialized
			*
			* @retval 0 Tells that display creation was sucessful
			*
			*/
			int Initialise();
			int LoadSound(Utilz::GUID soundFile, SoundIndexName soundType);
			void Shutdown();
		private:
			void retSoundData(const Utilz::GUID& guid, void* data, size_t size);
			AudioSound* audioSound;
			AudioStream* audioStream;
			std::map<Utilz::GUID, int> trackSound;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOMANAGER_H