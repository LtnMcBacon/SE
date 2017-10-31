#pragma once
#ifndef SE_CORE_AUDIO_HANDLER_H
#define SE_CORE_AUDIO_HANDLER_H

#include <Audio\IAudioHandler.h>
#include "AudioSound.h"
#include "AudioStream.h"
#include "AudioStructType.h"
#include <Utilz\GUID.h>
#include <map>

namespace SE {
	namespace Audio {

		class AudioHandler : public IAudioHandler
		{
		public:
			AudioHandler();
			~AudioHandler();
			/**
			* @brief	Calls init functions in stream and sound
			*
			* @retval 0 Tells that creation was sucessful
			* @retval -1 Tells that creation was unsucessful
			*
			*/
			int Initialize() override;
			/**
			* @brief	Loads the requested sound if not already loaded
			*
			* @param[in] soundFile The GUID of the requested soundfile
			*
			* @retval 0 Sound is already loaded
			* @retval 1 Sound is being loaded
			*
			*/
			int LoadSound(void* data, size_t size) override;
			/**
			* @brief	Create a stream and return it's ID
			*
			* @param[in] soundFile The GUID of the requested soundfile
			* @param[in] soundType The type of sound
			*
			* @retval 0+ Stream ID 
			* @retval -1 Could not find sound/sound not loaded
			*
			*/
			int CreateStream(int soundID, Audio::SoundIndexName soundType) override;

			/**
			* @brief	Create a stream and return it's ID
			*
			* @param[in] soundFile The GUID of the requested soundfile
			* @param[in] soundType The type of sound
			* @param[in] panData Data for stereo paning
			*
			* @retval 0+ Stream ID
			* @retval -1 Could not find sound/sound not loaded
			*
			*/
			int CreatePanStream(int soundID, Audio::SoundIndexName soundType, PanData panData) override;
			/**
			* @brief Streams the given sound
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that streamstart was sucessful
			* @retval -1 Tells that streamstart was unsucessful
			*
			*/
			int StreamSound(int streamID) override;
			/**
			* @brief Stops the stream with the given ID
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that stopstream was sucessful
			* @retval -1 Tells that stopstream was unsucessful
			*
			*/
			int StopSound(int streamID) override;
			/**
			* @brief Removes the given sound
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that removestream was sucessful
			* @retval -1 Tells that removestream was unsucessful
			*
			*/
			int RemoveSound(int streamID) override;
			/**
			* @brief Updates Pos of head and sound
			*
			* @param[in] streamID The stream ID
			* @param[in] panData Data for stereo paning
			*
			* @retval 0 Tells that UpdateStreamPos was sucessful
			* @retval -1 Tells that UpdateStreamPos was unsucessful
			*
			*/
			int UpdateStreamPos(int streamID, PanData panData) override;
			void SetSoundVol(SE::Audio::SoundVolType volType, size_t newVol) override;
			void Shutdown() override;
		private:
			Audio::AudioSound audioSound;
			Audio::AudioStream audioStream;
		};
	}	//namespace Audio
}	//namespace SE

#endif	//SE_CORE_AUDIO_HANDLER_H