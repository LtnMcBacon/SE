#ifndef SE_CORE_ENGINE_H_
#define SE_CORE_ENGINE_H_
#include <IEngine.h>

#include "EntityManager.h"
#include "TransformManager.h"
#include "MaterialManager.h"
#include "CollisionManager.h"
#include <Graphics\IRenderer.h>
#include <Window\IWindow.h>
#include <ResourceHandler\IResourceHandler.h>
#include "RenderableManager.h"
#include "OptionHandler.h"
#include "CameraManager.h"
#include "AudioManager.h"
#include "AnimationManager.h"
#include "LightManager.h"
#include "DevConsole.h"
#include "ParticleSystemManager.h"


#include "DebugRenderManager.h"
#include <Utilz\StackAllocator.h>
#include "GUIManager.h"
#include <Utilz\TimeCluster.h>
namespace SE
{
	namespace Core
	{
		/**
		* @brief A singleton containing an instance of the engine. All interaction with the engine happens through this instance.
		* @details The singleton is instanciated with the first call to GetInstance and initialized with a call to Initialize.
		**/
		class Engine : public IEngine
		{
		public:
			Engine();
			Engine(const Engine& other) = delete;
			Engine(const Engine&& other) = delete;
			Engine& operator=(const Engine& rhs) = delete;
			~Engine();

			/**
			* @brief    Initializes all modules used by the engine. Must be called before any other public methods (except getInstance).
			*
			* @details   The methods receives a  Engine::InitializationInfo which must be filled in by the user before passing it to Init.
			* @param[in] info Information about how to initialize the engine, containing informations such as fullscreen mode, resolution, etc.
			* @retval return_value_0 Returns 0 on success and non-zero otherwise.
			* @warning Make sure there is a matching call to Release in order to clean up.
			* Example code:
			* @code
			*    Engine& instance = Engine::GetInstance();
			*    Engine::InitializationInfo initInfo;
			*    //...
			*    instance.Init(initInfo);
			*    //Now the instance may be used.
			*    //
			* @endcode
			* @sa InitializationInfo, Release
			*/
			int Init(const InitializationInfo& info = InitializationInfo())override;


			/**
			* @brief Begins the frame. If this call is never made before, Frame will call it
			* @retval 0 On success
			* @retval -1 If the frame has not ended before another call to frame
			*/
			int BeginFrame()override;


			/**
			* @brief    Updates the state of the Core, entity cleanup, input, etc.
			* @details  Calls frame in all of its managers and handlers. It also records 
			* the time each manager takes to calculate its frame.
			*
			* @retval 0 On success.
			*/
			int Frame()override;

			/**
			* @brief    Releases all resources held by the engine. Call this before exiting your program.
			* @retval return_value_0 Returns 0 on success and non-zero otherwise.
			* Example code:
			* @code
			*    //main.cpp
			*    Engine& instance = Engine::GetInstance();
			*    Engine::InitializationInfo initInfo;
			*    //...
			*    instance.Init(initInfo);
			*    //Now the instance may be used.
			*    //...
			*    instance.Release();
			*    return 0;
			* @endcode
			* @sa InitializationInfo, Release
			*/
			int Release();

			/**
			* @brief    Returns a pointer to the renderer.
			* @sa EntityManager
			*/
			inline Graphics::IRenderer* GetRenderer() const override
			{
				return renderer;
			}

			/**
			* @brief    Returns a pointer to the window.
			* @sa EntityManager
			*/
			inline Window::IWindow* GetWindow() const override
			{
				return window;
			}

			/**
			* @brief    Returns a pointer to the resource handler.
			* @sa EntityManager
			*/
			inline ResourceHandler::IResourceHandler* GetResourceHandler() const override
			{
				return resourceHandler;
			}

			/**
			* @brief    Returns a pointer to the dev console.
			*/
			inline DevConsole* GetDevConsole()const override
			{
				return devConsole;
			}

			/**
			* @brief    Get a struct containing all the managers.
			**/
			inline const ManagerWrapper& GetManagers()const override
			{
				return managers;
			}

			/**
			* @brief	Return a map of with profiling information.
			*
			*/
			void GetProfilingInformation(Utilz::TimeMap& map)const override
			{
				timeClus->GetMap(map);
				renderer->GetProfilingInformation(map);
			}

		private:
			

			/**
			* @brief    Sets options to data in the .ini file
			*/
			void InitStartupOption();
			/**
			* @brief    Function that gets called when options has changed
			*/
			void OptionUpdate();

			void GatherErrors();

			EntityManager* entityManager;
			Graphics::IRenderer* renderer;
			Window::IWindow* window;
			ResourceHandler::IResourceHandler* resourceHandler;

			ManagerWrapper managers;

			TransformManager* transformManager;
			RenderableManager* renderableManager;
			MaterialManager* materialManager;
			AudioManager* audioManager;
			CollisionManager* collisionManager;
			OptionHandler* optionHandler;
			CameraManager* cameraManager;
			DebugRenderManager* debugRenderManager;
			AnimationManager* animationManager;
			ParticleSystemManager* particleSystemManager;
			Utilz::StackAllocator* perFrameStackAllocator;

			GUIManager* guiManager;
			LightManager* lightManager;

			DevConsole* devConsole;

			Utilz::TimeCluster* timeClus;
			bool frameBegun;
		};

	}
}
#endif // SE_CORE_ENGINE_H_
