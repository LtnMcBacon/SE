#ifndef SE_CORE_ENGINE_H_
#define SE_CORE_ENGINE_H_
#include "Core/EntityManager.h"
#include "Core/TransformManager.h"

#include <Graphics\IRenderer.h>
#include <Window\IWindow.h>
#include <ResourceHandler\IResourceHandler.h>
#include "AudioManager.h"
#include "RenderableManager.h"

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
			* @brief	Called each frame, to update the state.
			*/
			void Frame();

			/**
			* @brief    Returns a reference to the entity manager.
			* @retval return_value_0 Returns a reference to the entity manager.
			* @sa EntityManager
			*/
			EntityManager& GetEntityManager() const;

			/**
			* @brief    Returns a pointer to the renderable manager.
			* @retval return_value_0 Returns a reference to the entity manager.
			* @sa EntityManager
			*/
			RenderableManager& GetRenderableManager() const;

			/**
			* @brief    Returns a pointer to the renderer.
			* @retval return_value_0 Returns a reference to the entity manager.
			* @sa EntityManager
			*/
			Graphics::IRenderer* GetRenderer() const;

			/**
			* @brief    Returns a pointer to the window.
			* @retval return_value_0 Returns a reference to the entity manager.
			* @sa EntityManager
			*/
			Window::IWindow* GetWindow() const;

			/**
			* @brief    Returns a pointer to the resource handler.
			* @retval return_value_0 Returns a reference to the entity manager.
			* @sa EntityManager
			*/
			ResourceHandler::IResourceHandler* GetResourceHandler() const;

			/**
			* @brief    Returns a reference to the transform manager
			* @retval return_value_0 Returns a reference to the transform manager.
			* @sa TransformManager
			*/
			TransformManager& GetTransformManager() const;

			/**
			* @brief    Returns a reference to the audio manager
			* @retval return_value_0 Returns a reference to the audio manager.
			* @sa AudioManager
			*/
			AudioManager& GetAudioManager() const;
		private:
			Engine();
			Engine(const Engine& other) = delete;
			Engine(const Engine&& other) = delete;
			Engine& operator=(const Engine& rhs) = delete;
			~Engine();

			EntityManager* entityManager;
			Graphics::IRenderer* renderer;
			Window::IWindow* window;
			ResourceHandler::IResourceHandler* resourceHandler;
			TransformManager* transformManager;
			RenderableManager* renderableManager;
			AudioManager* audioManager;
		};

	}
}
#endif // SE_CORE_ENGINE_H_
