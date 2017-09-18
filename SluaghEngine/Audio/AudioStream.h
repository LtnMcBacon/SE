#pragma once
#ifndef SE_AUDIO_AUDIOSTREAM_H
#define SE_AUDIO_AUDIOSTREAM_H
#include "AudioStructType.h"


namespace SE {
	namespace Audio {

		class AudioStream
		{
		public:
			AudioStream();
			~AudioStream();
			int Initialise();
			int StreamSound(SoundIndexName soundType, void* streamData);
			void Shutdown();
		private:
			std::vector<PaStream*> stream;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSTREAM_H