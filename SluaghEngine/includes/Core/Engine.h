#ifndef SE_CORE_ENGINE_H_
#define SE_CORE_ENGINE_H_
#include "EntityManager.h"
#include "TransformManager.h"
#include "MaterialManager.h"
#include "CollisionManager.h"
#include <Graphics\IRenderer.h>
#include <Window\IWindow.h>
#include <ResourceHandler\IResourceHandler.h>
#include "AudioManager.h"
#include "RenderableManager.h"
#include "OptionHandler.h"
#include "CameraManager.h"

#include "DebugRenderManager.h"
#include <Utilz\StackAllocator.h>
namespace SE
{
	namespace Core
	{
		/**
		* @brief A singleton containing an instance of the engine. All interaction with the engine happens through this instance.
		* @details The singleton is instanciated with the first call to GetInstance and initialized with a call to Initialize.
		**/
		class Engine
		{
		public:
			struct InitializationInfo
			{
				bool placeholder; //Rewrite this later
			};

			/**
			* @brief    Returns a reference to the instance.
			* @retval return_value_1 Returns the instance of Engine. If it is the first time you call this method in your program, make sure you call Init before making any other calls.
			* Example code:
			* @code
			*    Engine& e = Engine::GetInstance();
			* @endcode
			*/

			static Engine& GetInstance();

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
			int Init(const InitializationInfo& info);

			/**
			* @brief    Updates the state of the Core, entity cleanup, input, etc.
			*
			* @retval 0 On success.
			*/
			int Frame(double dt);

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
			* @brief    Returns a reference to the entity manager.
			* @retval return_value_0 Returns a reference to the entity manager.
			* @sa EntityManager
			*/
			inline EntityManager& GetEntityManager() const{
				return *entityManager;
			}

			/**
			* @brief    Returns a pointer to the renderer.
			* @retval return_value_0 Returns a reference to the entity manager.
			* @sa EntityManager
			*/
			inline Graphics::IRenderer* GetRenderer() const{
				return renderer;
			}

			/**
			* @brief    Returns a pointer to the window.
			* @retval return_value_0 Returns a reference to the entity manager.
			* @sa EntityManager
			*/
			inline Window::IWindow* GetWindow() const {
				return window;
			}

			/**
			* @brief    Returns a pointer to the resource handler.
			* @retval return_value_0 Returns a reference to the entity manager.
			* @sa EntityManager
			*/
			inline ResourceHandler::IResourceHandler* GetResourceHandler() const {
				return resourceHandler;
			}

			/**
			* @brief    Returns a reference to the transform manager
			* @retval return_value_0 Returns a reference to the transform manager.
			* @sa TransformManager
			*/
			inline TransformManager& GetTransformManager() const {
				return *transformManager;
			}

			/**
			* @brief    Returns a reference to the Renderable manager
			* @retval return_value_0 Returns a reference to the renderable manager.
			* @sa RenderableManager
			*/
			inline RenderableManager& GetRenderableManager() const {
				return *renderableManager;
			}

			/**
			* @brief    Returns a reference to the Material manager
			* @retval return_value_0 Returns a reference to the Material manager.
			* @sa MaterialManager
			*/
			inline MaterialManager& GetMaterialManager() const {
				return *materialManager;
			}
			/**
			* @brief    Returns a reference to the audio manager
			* @retval return_value_0 Returns a reference to the audio manager.
			* @sa AudioManager
			*/
			inline AudioManager& GetAudioManager() const {
				return *audioManager;
			}

			/**
			* @brief    Returns a reference to the collision manager
			* @retval return_value_0 Returns a reference to the collision manager.
			* @sa CollisionManager
			*/
			inline CollisionManager& GetCollisionManager()const {
				return *collisionManager;
			}

			/**
			* @brief    Returns a reference to the audio manager
			* @retval return_value_0 Returns a reference to the audio manager.
			* @sa OptionHandler
			*/
			inline OptionHandler& GetOptionHandler() const {
				return *optionHandler;
			}

			/**
			* @brief    Returns a reference to the camera manager
			* @retval return_value_0 Returns a reference to the camera manager.
			* @sa CameraManager
			*/
			inline CameraManager& GetCameraManager() const {
				return *cameraManager;
			}


			/**
			* @brief    Returns a reference to the debug render manager
			* @retval return_value_0 Returns a reference to the debug render manager.
			* @sa DebugRenderManager
			*/
			inline DebugRenderManager& GetDebugRenderManager() const {
				return *debugRenderManager;
			}
		private:
			Engine();
			Engine(const Engine& other) = delete;
			Engine(const Engine&& other) = delete;
			Engine& operator=(const Engine& rhs) = delete;
			~Engine();

			/**
			* @brief    Sets options to data in the .ini file
			*/
			void InitStartupOption();
			/**
			* @brief    Function that gets called when options has changed
			*/
			void OptionUpdate();

			EntityManager* entityManager;
			Graphics::IRenderer* renderer;
			Window::IWindow* window;
			ResourceHandler::IResourceHandler* resourceHandler;
			TransformManager* transformManager;
			RenderableManager* renderableManager;
			MaterialManager* materialManager;
			AudioManager* audioManager;
			CollisionManager* collisionManager;
			OptionHandler* optionHandler;
			CameraManager* cameraManager;
			DebugRenderManager* debugRenderManager;
			Utilz::StackAllocator* perFrameStackAllocator;



		};

	}
}
#endif // SE_CORE_ENGINE_H_
