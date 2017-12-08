#pragma once
#ifndef SE_CORE_AUDIO_MANAGER_H
#define SE_CORE_AUDIO_MANAGER_H

#include <IAudioManager.h>

#include <map>
#include <stack>
#include <unordered_map>
#include <random>

#ifdef _DEBUG
#pragma comment(lib, "AudioD.lib")
#else
#pragma comment(lib, "Audio.lib")
#endif

namespace SE {
	namespace Core {

		class AudioManager : public IAudioManager
		{
		public:
			AudioManager(const IAudioManager::InitializationInfo& initInfo);
			~AudioManager();

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

			void SetCameraEnt(const Entity& entity)override;
			void Destroy(const Entity& entity)override;
		private:
			void UnloadSound(const Utilz::GUID & soundFile);
			InitializationInfo initInfo;
			
			void GarbageCollection()override;
			void Destroy(size_t index)override;
	
			void Init();
			void SetDirty(const Entity & entity, size_t index);
			void UpdateDirtyTransforms();
			struct DirtyEntityInfo
			{
				size_t transformIndex;
				Entity entity;
			};
			std::vector<DirtyEntityInfo> dirtyEntites;

			struct SoundState
			{
				int stream;
				bool playState = false;
				Audio::SoundIndexName soundType;
			};
			struct EntityToSound
			{
				std::map<Utilz::GUID, SoundState, Utilz::GUID::Compare> guidToStream;
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

			Entity cameraEnt;
			bool PanExist = false;
			bool cameraMove = false;
			std::default_random_engine generator;
		};
	}	//namespace Core
}	//namespace SE

#endif	//SE_CORE_AUDIO_MANAGER_H