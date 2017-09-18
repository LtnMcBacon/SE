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
			/**
			* @brief	Loads the requested sound if not already loaded
			*
			* @param[in] soundFile The GUID of the requested soundfile
			*
			* @retval 0 Sound is already loaded
			* @retval 1 Sound is being loaded
			*
			*/
			int LoadSound(Utilz::GUID soundFile);
			/**
			* @brief	Plays requsted sound
			*
			* @param[in] soundFile The GUID of the requested soundfile
			* @param[in] soundType The type of sound
			*
			* @retval 0 Sound is already loaded
			* @retval 1 Sound is being loaded
			*
			*/
			int playSound(Utilz::GUID soundFile, SoundIndexName soundType);
			void Shutdown();
		private:
			void retSoundData(const Utilz::GUID& guid, void* data, size_t size);
			AudioSound* audioSound;
			AudioStream* audioStream;
			std::map<Utilz::GUID, int, Utilz::GUID::Compare> trackSound;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOMANAGER_H