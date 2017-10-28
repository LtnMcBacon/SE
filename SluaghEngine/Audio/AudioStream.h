#pragma once
#ifndef SE_AUDIO_AUDIOSTREAM_H
#define SE_AUDIO_AUDIOSTREAM_H
#include "AudioStructType.h"
#include "AudioTypeDefs.h"


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
			* @brief Creates stream with streamdata
			*
			* @param[in] soundType The type of sound to set private data
			* @param[in] streamData The sound data for the stream
			*
			* @retval 0+ Stream ID
			* @retval -1 Tells that creation was unsucessful
			* @retval -2 No device was found
			*
			*/
			int CreateStream(SoundIndexName soundType, void* streamData);
			/**
			* @brief Streams the given sound
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that streamstart was sucessful
			* @retval -1 Tells that streamstart was unsucessful
			* @retval -2 No device was found
			*
			*/
			int StreamSound(int streamID);
			/**
			* @brief Stops the stream with the given ID
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that stopstream was sucessful
			* @retval -1 Tells that stopstream was unsucessful
			*
			*/
			int StopSound(int streamID);
			/**
			* @brief Streams the given sound
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that removestream was sucessful
			* @retval -1 Tells that removestream was unsucessful
			*
			*/
			int RemoveSound(int streamID);
			void Shutdown();
		private:
			std::vector<PaStream*> stream;
			std::vector<AudioOut*> sampleOut;
			std::stack<int> freeStreamID;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_AUDIO_AUDIOSTREAM_H