#ifndef SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#include <IResourceHandler.h>
#include "IAssetLoader.h"
#include <unordered_map>
#include <thread>
#include <mutex>
#include <stack>
#include <Utilz\Event.h>
#include <Utilz\CircularFiFo.h>
#include <Utilz\ThreadPool.h>
#include <Utilz\Concurrent_Vector.h>
#include <Utilz\Concurrent_Unordered_Map.h>
namespace SE
{
	namespace ResourceHandler
	{
		struct Data
		{
			void* data = nullptr;
			size_t size = 0;
		};

		enum class State {
			LOADING = 1 << 2,
			DEAD = 1 << 3,
			FAIL = 1 << 4,
			LOADED = 1 << 5,
			IMMUTABLE = 1 << 6,
			RAW = 1 << 7
		};


		/**
		*
		* @brief The resource handler
		*
		* @details The resource handler can process requests for loading resources.
		* It also keeps track of which resources are already loaded.
		*
		**/
		class ResourceHandler : public IResourceHandler
		{
		public:
			ResourceHandler();
			~ResourceHandler();

			int Initialize(const InitializationInfo& initInfo)override;
			void Shutdown()override;

			const InitializationInfo& GetInfo()const override;
			void UpdateInfo(const InitializationInfo& initInfo)override;

			void GetAllGUIDsWithExtension(const Utilz::GUID& ext, std::vector<Utilz::GUID>& guids)const override;
			void GetAllGUIDsWithExtension(const Utilz::GUID& ext, std::vector<Utilz::GUID>& guids, std::vector<std::string>& names)const override;

			int LoadResource(const Utilz::GUID& guid, 
				const std::function<InvokeReturn(const Utilz::GUID&, void*, size_t)>& invokeCallback, 
				LoadFlags loadFlags = LoadFlags::LOAD_FOR_RAM)override;

			int LoadResource(const Utilz::GUID& guid,
				const Callbacks& callbacks,
				LoadFlags loadFlags)override;

			void UnloadResource(const Utilz::GUID& guid, UnloadFlags unloadFlags)override;

			/**
			* @brief	Get the error messages that have accumulated. This will also clear the errors messages.
			*
			**/		
			inline void GetErrorMessages(std::vector<std::string>& errors) override
			{
				Utilz::Move(this->errors, errors);
				if (errors.size())
					int i = 0;
			}
		private:
			InitializationInfo initInfo;


			/****************	Entires			*****************/

			struct Resource_Entry
			{
				Data data;
				uint32_t ref;
				std::function<void(const Utilz::GUID&, void*, size_t)> destroyCallback;
				State state = State::DEAD;
			};

			Utilz::Concurrent_Unordered_Map<Utilz::GUID, Resource_Entry, Utilz::GUID::Hasher> guidToRAMEntry;
			Utilz::Concurrent_Unordered_Map< Utilz::GUID, Resource_Entry, Utilz::GUID::Hasher> guidToVRAMEntry;

			/****************	END Entires		*****************/

			// Errors
			Utilz::Concurrent_Vector<std::string> errors;

			/****************	Loading			*****************/
			IAssetLoader* diskLoader;
			std::mutex loadLock;
			std::mutex loadCallbackLock;
			//Utilz::ThreadPool* load_threadPool;

			// need to keep track of threads so we can join them
			std::vector< std::thread > workers;
			// the task queue
			std::queue< std::function<void()> > tasks;

			// synchronization
			std::mutex queue_mutex;
			std::condition_variable condition;
			bool stop;

			struct LoadJob
			{
				Utilz::GUID guid;
				Callbacks callbacks;
				LoadFlags loadFlags;
				LoadJob& operator=(const LoadJob& other) { guid = other.guid; callbacks = other.callbacks; loadFlags = other.loadFlags; return*this; }
			};

			int Load(Utilz::Concurrent_Unordered_Map<Utilz::GUID, Resource_Entry, Utilz::GUID::Hasher>* map, LoadJob job);

			
			/****************	END Loading		*****************/


			/****************	Unloading		*****************/
			/*void LinearEvict(std::vector<Utilz::GUID>& evictOrder, size_t addedSize, UnloadFlags flags);
			typedef void(ResourceHandler::*UnloadingStrategy)(size_t addedSize);
			UnloadingStrategy Unload = &ResourceHandler::LinearUnload;*/
			/****************	END Unloading	*****************/
		};
	}
}
ENUM_FLAGS(SE::ResourceHandler::State);
#endif //SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_