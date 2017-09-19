#pragma once
#ifndef SE_AUDIO_AUDIOSOUND_H
#define SE_AUDIO_AUDIOSOUND_H
#include "AudioStructType.h"



namespace SE {
	namespace Audio {
		

		class AudioSound
		{
		public:
			AudioSound();
			~AudioSound();
			int Initialize();
			int LoadSound(void * data, size_t size);
			void* GetSample(int soundID, SoundIndexName soundType);
			void Shutdown();
		private:
			std::vector<AudioSample> soundSample;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSOUND_H