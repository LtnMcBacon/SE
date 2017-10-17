#pragma once
#ifndef SE_CORE_AUDIO_MANAGER_H
#define SE_CORE_AUDIO_MANAGER_H

#include "EntityManager.h"
#include <Utilz\GUID.h>
#include <Audio\IAudioHandler.h>
#include <ResourceHandler\IResourceHandler.h>
#include <map>
#include <stack>
#include <unordered_map>
#include <random>

namespace SE {
	namespace Core {

		class AudioManager
		{
		public:
			AudioManager(ResourceHandler::IResourceHandler* resourceHandler, const EntityManager & entityManager);
			~AudioManager() {};
			/**
			* @brief	Calls init functions in audiohandlern
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
			* @brief	Create a sound and return it's ID
			*
			* @param[in] soundFile The GUID of the requested soundfile
			* @param[in] soundType The type of sound
			*
			* @retval 0+ Entitys sound ID
			* @retval -1 Could not find sound/sound not loaded or entity is dead
			*
			*/
			int CreateStream(const Entity& entity, Utilz::GUID soundFile, Audio::SoundIndexName soundType);
			/**
			* @brief	Checks if sound has finished loading
			*
			* @param[in] soundFile The GUID of the requested soundfile
			*
			* @retval 0 Sound is loaded
			* @retval -1 Sound is not loaded
			*
			*/
			int CheckIfLoaded(Utilz::GUID soundFileD);
			/**
			* @brief Streams the given sound
			*
			* @param[in] soundID The sounds ID
			*
			* @retval 0 Tells that streamstart was sucessful
			* @retval -1 Tells that streamstart was unsucessful
			*
			*/
			int StreamSound(const Entity& entity, int entSoundID);
			/**
			* @brief Stops the stream with the given ID
			*
			* @param[in] soundID The sounds ID
			*
			* @retval 0 Tells that stopstream was sucessful
			* @retval -1 Tells that stopstream was unsucessful
			*
			*/
			int StopSound(const Entity& entity, int entSoundID);
			/**
			* @brief Removes the given sound
			*
			* @param[in] soundID The sounds ID
			*
			* @retval 0 Tells that removestream was sucessful
			* @retval -1 Tells that removestream was unsucessful
			*
			*/
			int RemoveSound(const Entity& entity, int entSoundID);
			void SetSoundVol(SE::Audio::SoundVolType volType, size_t newVol);
			void Frame();
			void Shutdown();
		private:
			void GarbageCollection();
			void Destroy(size_t index);

			struct EntityToSound
			{
				size_t amountOfSound;
				std::vector<int> streamID;
				std::stack<int> freeStreamID;
			};

			ResourceHandler::InvokeReturn retSoundData(const Utilz::GUID& guid, void* data, size_t size);
			Audio::IAudioHandler* audioHandler;
			std::map<Utilz::GUID, int, Utilz::GUID::Compare> trackSound;
			std::unordered_map<Entity, EntityToSound, EntityHasher> entToSounds;
			std::vector<Entity> soundEntity;

			std::default_random_engine generator;

			ResourceHandler::IResourceHandler* resourceHandler;
			const EntityManager& entityManager;
		};
	}	//namespace Core
}	//namespace SE

#endif	//SE_CORE_AUDIO_MANAGER_H