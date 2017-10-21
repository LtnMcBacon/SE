#pragma once
#ifndef SE_CORE_AUDIO_MANAGER_H
#define SE_CORE_AUDIO_MANAGER_H

#include <IAudioManager.h>

#include <map>
#include <stack>
#include <unordered_map>
#include <random>

#ifdef _DEBUG
#pragma comment(lib, "audioD.lib")
#else
#pragma comment(lib, "audio.lib")
#endif

namespace SE {
	namespace Core {

		class AudioManager : public IAudioManager
		{
		public:
			AudioManager(const InitializationInfo& initInfo);
			~AudioManager() {};


			/**
			* @brief	Create a sound
			*
			* @param[in] createInfo The CreateInfo struct
			*
			*/
			void Create(const Entity& entity, const CreateInfo& createInfo) override;
			/**
			* @brief Streams the given sound
			*
			* @param[in] soundFile The sound to play
			*
			*/
			void PlaySound(const Entity& entity, const Utilz::GUID& soundFile) override;
			/**
			* @brief Stops the stream with the given ID
			*
			* @param[in] soundFile The sound to stop
			*
			*/
			void StopSound(const Entity& entity, const Utilz::GUID& soundFile) override;
			/**
			* @brief Removes the given sound
			*
			* @param[in] soundFile The sound to remove
			*
			*/
			void RemoveSound(const Entity& entity, const Utilz::GUID& soundFile) override;

			void SetSoundVol(const SE::Audio::SoundVolType& volType, size_t newVol)override;
		
			void Frame(Utilz::TimeCluster* timer)override;

		private:
			InitializationInfo initInfo;
			
			void GarbageCollection()override;
			void Destroy(size_t index)override;
			void Destroy(const Entity& entity)override;

			struct EntityToSound
			{
				std::map<Utilz::GUID, int, Utilz::GUID::Compare> guidToStream;
			};
			struct SoundInfo
			{
				int handle;
				int refCount;
			};
			Audio::IAudioHandler* audioHandler;

			std::map<Utilz::GUID, SoundInfo, Utilz::GUID::Compare> guidToSound;
			std::unordered_map<Entity, EntityToSound, EntityHasher> entToSounds;
			std::vector<Entity> soundEntity;

			std::default_random_engine generator;
		};
	}	//namespace Core
}	//namespace SE

#endif	//SE_CORE_AUDIO_MANAGER_H