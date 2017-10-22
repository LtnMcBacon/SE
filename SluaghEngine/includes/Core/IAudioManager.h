#ifndef SE_INTERFACE_AUDIO_MANAGER_H_
#define SE_INTERFACE_AUDIO_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include "IManager.h"
#include "IEntityManager.h"
#include <ResourceHandler\IResourceHandler.h>
#include <Audio\IAudioHandler.h>
#undef PlaySound
namespace SE
{
	namespace Core
	{
		class IAudioManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				ResourceHandler::IResourceHandler* resourceHandler;
				IEntityManager* entityManager;
			};
			struct CreateInfo
			{
				Utilz::GUID soundFile;
				Audio::SoundIndexName soundType;
			};
			virtual ~IAudioManager() {};

			/**
			* @brief	Create a sound 
			* @param[in] createInfo The CreateInfo struct
			*
			*/
			virtual void Create(const Entity& entity, const CreateInfo& createInfo) = 0;
		
			/**
			* @brief Streams the given sound
			*
			* @param[in] soundFile The sound to play
			*
			*/
			virtual void PlaySound(const Entity& entity, const Utilz::GUID& soundFile) = 0;
			/**
			* @brief Stops the stream with the given ID
			*
			* @param[in] soundFile The sound to stop
			*
			*/
			virtual void StopSound(const Entity& entity, const Utilz::GUID& soundFile) = 0;
			/**
			* @brief Removes the given sound
			*
			* @param[in] soundFile The sound to remove
			*
			*/
			virtual void RemoveSound(const Entity& entity, const Utilz::GUID& soundFile) = 0;

			virtual void SetSoundVol(const SE::Audio::SoundVolType& volType, size_t newVol) = 0;

		protected:
			IAudioManager() {};
	
			IAudioManager(const IAudioManager& other) = delete;
			IAudioManager(const IAudioManager&& other) = delete;
			IAudioManager& operator=(const IAudioManager& other) = delete;
			IAudioManager& operator=(const IAudioManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the AudioManager
		**/
		DECLDIR_CORE IAudioManager* CreateAudioManager(const IAudioManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_AUDIO_MANAGER_H_