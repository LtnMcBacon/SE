#ifndef SE_INTERFACE_RENDERABLE_MANAGER_H_
#define SE_INTERFACE_RENDERABLE_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif


#include <Utilz\GUID.h>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>
#include <DevConsole\IConsole.h>

#include "IManager.h"
#include "IEntityManager.h"
#include "ITransformManager.h"
#include "IEventManager.h"

namespace SE
{
	namespace Core
	{
		/**
		*
		* @brief This class is used to bind renderable objects to an entity.
		*
		* @details Use this class when you want an entity to be rendered as a object in the world.
		*
		**/
		class IRenderableManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				ResourceHandler::IResourceHandler* resourceHandler;
				Graphics::IRenderer* renderer;
				DevConsole::IConsole* console;
				IEntityManager* entityManager;
				IEventManager* eventManager;
				ITransformManager* transformManager;
			};

			struct CreateInfo
			{
				Utilz::GUID meshGUID;
				bool transparent = false;
				bool wireframe = false;
				bool shadow = false;
			};


			virtual ~IRenderableManager() {};

			/**
			* @brief	Bind a renderable object to and entity
			*
			* @param[in] entity The entity to bind the renderable object to.
			* @param[in] info The CreateInfo struct.
			* @param[in] async If the object should be loaded in async.
			* @param[in] behavior The streaming behavior.
			*
			*/
			virtual void CreateRenderableObject(const Entity& entity, const CreateInfo& info) = 0;

			virtual void CreateShadowRenderObjectInfo(size_t index, Graphics::RenderJob * info) = 0;

			/**
			* @brief	Hide/Show the renderable object
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			virtual void ToggleRenderableObject(const Entity& entity, bool visible) = 0;

			virtual void ToggleWireframe(const Entity& entity, bool wireFrame) = 0;

			virtual void ToggleTransparency(const Entity& entity, bool transparency) = 0;

			virtual void ToggleShadow(const Entity& entity, bool shadow) = 0;

		protected:
			IRenderableManager() {};
	
			IRenderableManager(const IRenderableManager& other) = delete;
			IRenderableManager(const IRenderableManager&& other) = delete;
			IRenderableManager& operator=(const IRenderableManager& other) = delete;
			IRenderableManager& operator=(const IRenderableManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the RenderableManager
		**/
		DECLDIR_CORE IRenderableManager* CreateRenderableManager(const IRenderableManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_RENDERABLE_MANAGER_H_