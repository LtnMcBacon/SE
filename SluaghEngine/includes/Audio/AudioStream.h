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
			/**
			* @brief Init stream
			*
			* @retval 0 Tells that creation was sucessful
			* @retval -1 Tells that creation was unsucessful
			*
			*/
			int Initialize();
			/**
			* @brief Streams the given sound
			*
			* @retval 0+ Stream ID
			* @retval -1 Tells that creation and streamstart was unsucessful
			*
			*/
			int CreateStream(SoundIndexName soundType, void* streamData);

			int PlaySound(int streamID);
			int StopSound(int streamID);
			int RemoveSound(int streamID);
		private:
			std::vector<PaStream*> stream;
			std::vector<AudioOut*> sampleOut;
			std::stack<int> freeStreamID;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSTREAM_H