#ifndef SE_INTERFACE_ENGINE_H_
#define SE_INTERFACE_ENGINE_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include <Window\IWindow.h>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>
#include <Utilz\TimeCluster.h>
#include "IEntityManager.h"
#include "ITransformManager.h"
#include "IRenderableManager.h"
#include "IOptionsHandler.h"
#include "IAudioManager.h"
#include "ICollisionManager.h"
#include "ICameraManager.h"
#include "IParticleSystemManager.h"
#include "IAnimationManager.h"
#include "IMaterialManager.h"
#include "ILightManager.h"
#include "IDebugRenderManager.h"
#include "IGUIManager.h"
#include <DevConsole\IConsole.h>
#include "ITextManager.h"
#include "IEventManager.h"

namespace SE
{
	namespace Core
	{
		/**
		* @brief The interface class for the engine.
		* @details The engine is a container for the entity system and other subsystems.
		**/
		class IEngine
		{
		public:
			struct ManagerWrapper
			{
				IEntityManager* entityManager = nullptr;
				IAudioManager* audioManager = nullptr;
				ITransformManager* transformManager = nullptr;
				IParticleSystemManager* particleSystemManager = nullptr;
				ICameraManager* cameraManager = nullptr;
				ICollisionManager* collisionManager = nullptr;
				IRenderableManager* renderableManager = nullptr;
				IAnimationManager* animationManager = nullptr;
				IMaterialManager* materialManager = nullptr;
				ILightManager* lightManager = nullptr;
				IDebugRenderManager* debugRenderManager = nullptr;
				ITextManager* textManager = nullptr;
				IGUIManager* guiManager = nullptr;
				IEventManager* eventManager = nullptr;
			};
			struct Subsystems
			{
				ResourceHandler::IResourceHandler* resourceHandler = nullptr;
				Window::IWindow* window = nullptr;
				Graphics::IRenderer* renderer = nullptr;
				IOptionsHandler* optionsHandler = nullptr;
				DevConsole::IConsole* devConsole = nullptr;
			};
			struct InitializationInfo
			{
				Subsystems subSystems;
				ManagerWrapper managers;
			};
		

			/**
			* @brief    Initializes all modules used by the engine. Must be called before any other public methods.
			*
			* @details   The methods receives a  Engine::InitializationInfo which must be filled in by the user before passing it to Init.
			* @param[in] info Information about how to initialize the engine, containing informations such as fullscreen mode, resolution, etc.
			* @retval return_value_0 Returns 0 on success and non-zero otherwise.
			* @warning Make sure there is a matching call to Release in order to clean up.
			* Example code:
			* @code
			*    auto engine = CreateEngine();
			*    Engine::InitializationInfo initInfo;
			*    //...
			*    instance->Init(initInfo);
			*    //Now the instance may be used.
			*    //
			* @endcode
			* @sa InitializationInfo, Release
			*/
			virtual int Init(const InitializationInfo& info = InitializationInfo()) = 0;

			/**
			* @brief Begins the frame.
			* @retval 0 On success
			* @retval -1 If the frame has not ended before another call to BeginFrame
			*/
			virtual int BeginFrame() = 0;

			/**
			* @brief End the frame.
			* @retval 0 On success
			* @retval -1 If the frame has not started
			*/
			virtual int EndFrame() = 0;

			/**
			* @brief    Releases all resources held by the engine. Call this before exiting your program.
			* @retval return_value_0 Returns 0 on success and non-zero otherwise.
			* Example code:
			* @code
			*    //main.cpp
			*    auto engine = CreateEngine();
			*    Engine::InitializationInfo initInfo;
			*    //...
			*    engine->Init(initInfo);
			*    //Now the instance may be used.
			*    //...
			*    engine->Release();
			*    return 0;
			* @endcode
			* @sa InitializationInfo, Release
			*/
			virtual int Release() = 0;

			/**
			* @brief    Returns a reference to the subsystems.
			*/
			virtual const Subsystems& GetSubsystems() const = 0;

			/**
			* @brief    Returns a reference to the managers.
			*/
			virtual const ManagerWrapper& GetManagers() const = 0;

			/**
			* @brief	Return a map of with profiling information.
			*
			*/
			virtual void GetProfilingInformation(Utilz::TimeMap& map) = 0;
			
			virtual ~IEngine() {};

		protected:
			IEngine() {};

			IEngine(const IEngine& other) = delete;
			IEngine(const IEngine&& other) = delete;
			IEngine& operator=(const IEngine& other) = delete;
			IEngine& operator=(const IEngine&& other) = delete;
		};


		/**
		* @brief Create an instance of the engine
		**/
		DECLDIR_CORE IEngine* CreateEngine();
		/**
		* @brief Create an instance of the dev console
		**/
		DECLDIR_CORE DevConsole::IConsole* CreateConsole(Graphics::IRenderer* renderer, Window::IWindow* window);
	}
}

#endif //SE_INTERFACE_ENGINE_H_