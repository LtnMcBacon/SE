#pragma once
#ifndef SE_AUDIO_AUDIOSOUND_H
#define SE_AUDIO_AUDIOSOUND_H
#include "AudioStream.h"
#include "AudioSound.h"
#include "sndfile\sndfile.h"
#include <vector>
#include <stack>

#ifdef _DEBUG
#pragma comment (lib, "libsndfile-1.lib")
#else
#pragma comment (lib, "libsndfile-1.lib")
#endif


namespace SE {
	namespace Audio {
		enum SoundIndexName { StereoPan, BakgroundSoundLoop, BakgroundSound };

		struct AudioIn
		{
			void* soundData;
			size_t size;
		};

		struct AudioFile
		{
			char* soundData;
			size_t size;
			int currentPos;
		};

		struct AudioSample
		{
			SF_INFO info;
			float* samples;
			size_t currentPos;
		};

		class AudioSound
		{
		public:
			AudioSound();
			~AudioSound();
			/**
			* @brief	Calls init functions to initialize class and unlying classes
			*
			* @retval 0 Tells that display creation was sucessful
			*
			*/
			int Initialise();
			int LoadSound(AudioIn *inputSound);
			void Shutdown();
		private:
			std::vector<AudioSample> soundSample;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSOUND_H