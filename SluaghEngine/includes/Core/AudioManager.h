#pragma once
#ifndef SE_CORE_AUDIOMANAGER_H
#define SE_CORE_AUDIOMANAGER_H
#include <Audio\AudioSound.h>
#include <Audio\AudioStream.h>
#include <Utilz\GUID.h>
#include <map>

// basic file operations
#include <fstream>

#ifdef _DEBUG
#pragma comment(lib, "audioD.lib")
#else
#pragma comment(lib, "audio.lib")
#endif

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
			* @brief	Create a stream and return it's ID
			*
			* @param[in] soundFile The GUID of the requested soundfile
			* @param[in] soundType The type of sound
			*
			* @retval 0+ Stream ID 
			* @retval -1 Could not find sound/sound not loaded
			*
			*/
			int CreateStream(Utilz::GUID soundFile, Audio::SoundIndexName soundType);
			/**
			* @brief	Checks if sound has finished loading
			*
			* @param[in] soundFile The GUID of the requested soundfile
			*
			* @retval 0 Sound is loaded
			* @retval -1 Sound is not loaded
			*
			*/
			int CheckIfLoaded(Utilz::GUID soundFile);
			/**
			* @brief Streams the given sound
			*
			* @param[in] streamID The stream ID
			*
			* @retval 0 Tells that streamstart was sucessful
			* @retval -1 Tells that streamstart was unsucessful
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
			int retSoundData(const Utilz::GUID& guid, void* data, size_t size);
			Audio::AudioSound audioSound;
			Audio::AudioStream audioStream;
			std::map<Utilz::GUID, int, Utilz::GUID::Compare> trackSound;
		};
	}	//namespace Core
}	//namespace SE

#endif	//SE_CORE_AUDIOMANAGER_H