#ifndef SE_CORE_ENGINE_H_
#define SE_CORE_ENGINE_H_
#include <IEngine.h>
#include <Utilz\StackAllocator.h>
#include <Utilz\CPUTimeCluster.h>
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
			* @brief    Returns a reference to the subsystems.
			*/
			inline const Subsystems& GetSubsystems() const override
			{
				return subSystems;
			}

			/**
			* @brief    Returns a reference to the managers.
			*/
			inline const ManagerWrapper& GetManagers() const override
			{
				return managers;
			}

			/**
			* @brief    Returns a pointer to the dev console.
			*/
		/*	inline DevConsole* GetDevConsole()const override
			{
				return devConsole;
			}*/

			/**
			* @brief	Return a map of with profiling information.
			*
			*/
			void GetProfilingInformation(Utilz::TimeMap& map) override
			{
				timeClus.GetMap(map);
				subSystems.renderer->GetProfilingInformation(map);
			}

		private:
			Engine(const Engine& other) = delete;
			Engine(const Engine&& other) = delete;
			Engine& operator=(const Engine& rhs) = delete;
			Engine& operator=(const Engine&& other) = delete;

			void InitSubSystems();
			void InitManagers();

			std::vector<IManager*> managersVec;
			/**
			* @brief    Sets options to data in the .ini file
			*/
			void InitStartupOption();
			/**
			* @brief    Function that gets called when options has changed
			*/
			void OptionUpdate();

			void GatherErrors();

			Subsystems subSystems;
			ManagerWrapper managers;
		
			//	DevConsole* devConsole;

			Utilz::StackAllocator* perFrameStackAllocator;

		
	
			Utilz::CPUTimeCluster timeClus;
			bool frameBegun;
		};

	}
}
#endif // SE_CORE_ENGINE_H_
