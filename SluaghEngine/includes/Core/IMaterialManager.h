#ifndef SE_INTERFACE_MATERIAL_MANAGER_H_
#define SE_INTERFACE_MATERIAL_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif


#include <Utilz\GUID.h>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>

#include "IManager.h"
#include "IRenderableManager.h"
#include <DevConsole\IConsole.h>
#include "IEventManager.h"

namespace SE
{
	namespace Core
	{
		class IMaterialManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				ResourceHandler::IResourceHandler* resourceHandler;
				Graphics::IRenderer* renderer;
				DevConsole::IConsole* console;
				IEntityManager* entityManager;
				IEventManager* eventManager;
			};

			struct CreateInfo
			{
				Utilz::GUID shader;
				Utilz::GUID materialFile;
				bool bloom = true;
			};

			virtual ~IMaterialManager() {};

			/**
			* @brief	Create a texture for the entity. This is for mesh with no submeshes.
			* @param [in] entity The entity to bind the texture to.
			* @param [in] info The info used when creating the material.
			* @param [in] async If the resource should be streamed.
			* @param [in] behavior The streaming behavior.
			* @sa CreateInfo
			*/
			virtual void Create(const Entity& entity, const CreateInfo& info, bool async = false, ResourceHandler::Behavior behavior = ResourceHandler::Behavior::QUICK) = 0;

		protected:
			IMaterialManager() {};
	
			IMaterialManager(const IMaterialManager& other) = delete;
			IMaterialManager(const IMaterialManager&& other) = delete;
			IMaterialManager& operator=(const IMaterialManager& other) = delete;
			IMaterialManager& operator=(const IMaterialManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the MaterialManager
		**/
		DECLDIR_CORE IMaterialManager* CreateMaterialManager(const IMaterialManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_MATERIAL_MANAGER_H_