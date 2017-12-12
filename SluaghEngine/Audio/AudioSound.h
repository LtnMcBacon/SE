#pragma once
#ifndef SE_AUDIO_AUDIOSOUND_H
#define SE_AUDIO_AUDIOSOUND_H
#include "AudioStructType.h"
#include "AudioTypeDefs.h"

#ifdef _DEBUG
#pragma comment(lib, "utilzD.lib")
#else
#pragma comment(lib, "utilz.lib")
#endif

static bool readType = false;

namespace SE {
	namespace Audio {
		

		class AudioSound
		{
		public:
			AudioSound();
			~AudioSound();
			/**
			* @brief Init sound
			*
			* @retval 0 Tells that creation was sucessful
			* @retval -1 Tells that creation was unsucessful
			*
			*/
			int Initialize();
			/**
			* @brief	Loads the given sound data
			*
			* @param[in] sound The sound data to be loaded
			*
			* @retval 0+ Sound sample ID
			*
			*/
			int LoadSound(AudioFile* sound);
			int UnloadSound(int soundHandle);
			/**
			* @brief	Returns the requested sample
			*
			* @param[in] soundID The sound ID
			* @param[in] soundType The type of sound to set private data
			*
			* @retval void* Pointer to requested Sound sample
			*
			*/
			void* GetSample(int soundID, SoundIndexName soundType);
			void SetSoundVol(SE::Audio::SoundVolType volType, size_t newVol);
			void Shutdown();

		private:
			std::vector<AudioSample*> soundSample;
			std::stack<int> freeSample;
			
			float masterVol;
			float effectVol;
			float voiceVol;
			float voiceSVol;
			float bakgroundVol;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSOUND_H