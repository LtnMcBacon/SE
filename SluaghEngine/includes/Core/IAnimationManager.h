#ifndef SE_INTERFACE_ANIMATION_MANAGER_H_
#define SE_INTERFACE_ANIMATION_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif


#include <Utilz\GUID.h>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>

#include "IManager.h"
#include "IEntityManager.h"
#include "ITransformManager.h"
#include "IRenderableManager.h"
#include <DevConsole\IConsole.h>
#include "IEventManager.h"

namespace SE
{
	namespace Core
	{
		class IAnimationManager : public IManager
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
				Utilz::GUID mesh;
				Utilz::GUID skeleton;
				size_t animationCount;
				Utilz::GUID* animations;
			};


			virtual ~IAnimationManager() {};

			virtual void CreateAnimatedObject(const Entity& entity, const CreateInfo& info) = 0;

			virtual void Start(const Entity& entity, const Utilz::GUID& animation, float speed) = 0;
			virtual void SetSpeed(const Entity& entity, float speed) = 0;
			virtual void SetKeyFrame(const Entity& entity, float keyFrame) = 0;
			virtual void Start(const Entity& entity)const = 0;
			virtual void Pause(const Entity& entity)const = 0;

			virtual void ToggleVisible(const Entity& entity, bool visible) = 0;

		protected:
			IAnimationManager() {};
	
			IAnimationManager(const IAnimationManager& other) = delete;
			IAnimationManager(const IAnimationManager&& other) = delete;
			IAnimationManager& operator=(const IAnimationManager& other) = delete;
			IAnimationManager& operator=(const IAnimationManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the AnimationManager
		**/
		DECLDIR_CORE IAnimationManager* CreateAnimationManager(const IAnimationManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_ANIMATION_MANAGER_H_