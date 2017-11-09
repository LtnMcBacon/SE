#ifndef SE_INTERFACE_PARTICLE_SYSTEM_MANAGER_H_
#define SE_INTERFACE_PARTICLE_SYSTEM_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include <Utilz\GUID.h>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>

#include "IManager.h"
#include "ITransformManager.h"
#include "IRenderableManager.h"
#include <DevConsole\IConsole.h>
namespace SE
{
	namespace Core
	{
		class IParticleSystemManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				Graphics::IRenderer* renderer;
				ResourceHandler::IResourceHandler* resourceHandler;
				DevConsole::IConsole* console;
				IEntityManager* entityManager;
				ITransformManager* transformManager;
				IRenderableManager* renderableManager;
			};

			struct CreateInfo
			{
				Utilz::GUID systemFile;
			};


			virtual ~IParticleSystemManager() {};


			/**
			* @brief	Create a particle system.
			* @param [in] entity The entity to bind system to.
			* @param [in] info The info used when creating the system.
			* @param [in] async If the resource should be streamed.
			* @param [in] behavior The streaming behavior.
			* @sa CreateInfo
			*/
			virtual void CreateSystem(const Entity& entity, const CreateInfo& info) = 0;

			/**
			* @brief	Hide/Show a particle system.
			* @param [in] entity The entity to bind system to.
			* @param [in] visible If the resource should be streamed.
			*/
			virtual void ToggleVisible(const Entity& entity, bool visible) = 0;

		protected:
			IParticleSystemManager() {};
	
			IParticleSystemManager(const IParticleSystemManager& other) = delete;
			IParticleSystemManager(const IParticleSystemManager&& other) = delete;
			IParticleSystemManager& operator=(const IParticleSystemManager& other) = delete;
			IParticleSystemManager& operator=(const IParticleSystemManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the ParticleSystemManager
		**/
		DECLDIR_CORE IParticleSystemManager* CreateParticleSystemManager(const IParticleSystemManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_PARTICLE_SYSTEM_MANAGER_H_