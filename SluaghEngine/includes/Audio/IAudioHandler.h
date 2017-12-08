#pragma once
#pragma once
#ifndef SE_CORE_IAUDIO_HANDLER_H
#define SE_CORE_IAUDIO_HANDLER_H

#if defined DLL_EXPORT_AH
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

#include "AudioTypeDefs.h"

namespace SE {
	namespace Audio {

		class IAudioHandler
		{
		public:
			IAudioHandler() {};
			virtual ~IAudioHandler() {};
			/**
			* @brief	Calls init functions in stream and sound
			*
			* @retval 0 Tells that creation was sucessful
			* @retval -1 Tells that creation was unsucessful
			*
			*/
			virtual int Initialize() = 0;
			/**
			* @brief	Loads the requested sound if not already loaded
			*
			* @param[in] soundFile The GUID of the requested soundfile
			*
			* @retval 0 Sound is already loaded
			* @retval 1 Sound is being loaded
			*
			*/
			virtual int LoadSound(void* data, size_t size) = 0;
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
			virtual int CreateStream(int soundID, SoundIndexName soundType) = 0;
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
			virtual int CreatePanStream(int soundID, SoundIndexName soundType, PanData panData) = 0;
			/**
			* @brief Streams the given sound
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that streamstart was sucessful
			* @retval -1 Tells that streamstart was unsucessful
			*
			*/
			virtual int StreamSound(int streamID) = 0;
			/**
			* @brief Stops the stream with the given ID
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that stopstream was sucessful
			* @retval -1 Tells that stopstream was unsucessful
			*
			*/
			virtual int StopSound(int streamID) = 0;
			/**
			* @brief Removes the given stream
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that removestream was sucessful
			* @retval -1 Tells that removestream was unsucessful
			*
			*/
			virtual int RemoveSound(int streamID) = 0;
			/**
			* @brief Unloads the given sound
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that removestream was sucessful
			* @retval -1 Tells that removestream was unsucessful
			*
			*/
			virtual int UnloadSound(int soundHandle) = 0;
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
			virtual int UpdateStreamPos(int streamID, PanData panData) = 0;
			
			virtual void SetSoundVol(SE::Audio::SoundVolType volType, size_t newVol) = 0;
			virtual void Shutdown() = 0;
		private:
		};

		DECLDIR IAudioHandler* CreateNewAudioHandler();

	}	//namespace Audio
}	//namespace SE

#endif	//SE_CORE_IAUDIO_HANDLER_H