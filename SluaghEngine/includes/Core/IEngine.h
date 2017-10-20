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
			struct InitializationInfo
			{
				SE::Window::InitializationInfo winInfo;
			};
			struct ManagerWrapper
			{

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
			* @brief Begins the frame. If this call is never made before, Frame will call it
			* @retval 0 On success
			* @retval -1 If the frame has not ended before another call to frame
			*/
			virtual int BeginFrame() = 0;

			/**
			* @brief    Updates the state of the Core, entity cleanup, input, etc.
			* @details  Calls frame in all of its managers and handlers. It also records
			* the time each manager takes to calculate its frame.
			*
			* @retval 0 On success.
			*/
			virtual int Frame() = 0;

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
			* @brief    Returns a pointer to the renderer.
			*/
			virtual Graphics::IRenderer* GetRenderer() const = 0;

			/**
			* @brief    Returns a pointer to the window.
			*/
			virtual Window::IWindow* GetWindow() const = 0;

			/**
			* @brief    Returns a pointer to the resource handler.
			*/
			virtual ResourceHandler::IResourceHandler* GetResourceHandler() const = 0;
		
			/**
			* @brief    Returns a pointer to the dev console.
			*/
			virtual DevConsole* GetDevConsole()const = 0;

			/**
			* @brief    Get a struct containing all the managers.
			**/
			virtual const ManagerWrapper& GetManagers()const = 0;

			/**
			* @brief	Return a map of with profiling information.
			*
			*/
			virtual void GetProfilingInformation(Utilz::TimeMap& map)const = 0;
			
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
	}
}

#endif //SE_INTERFACE_ENGINE_H_