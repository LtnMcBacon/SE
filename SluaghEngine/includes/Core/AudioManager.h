#pragma once
#ifndef SE_AUDIO_AUDIOMANAGER_H
#define SE_AUDIO_AUDIOMANAGER_H
#include <Audio\AudioSound.h>
#include <Audio\AudioStream.h>
#include <Utilz\GUID.h>
#include <map>

namespace SE {
	namespace Core {

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
			* @retval 0 Tells that creation was sucessful
			* @retval -1 Tells that creation was unsucessful
			*
			*/
			int Initialize();
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
			int playSound(Utilz::GUID soundFile, Audio::SoundIndexName soundType);
			int CheckIfLoaded(Utilz::GUID soundFile);
			void Shutdown();
		private:
			void retSoundData(const Utilz::GUID& guid, void* data, size_t size);
			Audio::AudioSound* audioSound;
			Audio::AudioStream* audioStream;
			std::map<Utilz::GUID, int, Utilz::GUID::Compare> trackSound;


			//temp or something holds audioOut pointers
			std::vector<Audio::AudioOut*> audioToStream;
		};
	}	//namespace Core
}	//namespace SE

#endif	//SE_AUDIO_AUDIOMANAGER_H