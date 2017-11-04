#ifndef SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#include <IResourceHandler.h>
#include "IAssetLoader.h"
#include <map>
#include <thread>
#include <mutex>
#include <stack>
#include <Utilz\Event.h>
#include <Utilz\CircularFiFo.h>
#include <Utilz\ThreadPool.h>


namespace SE
{
	namespace ResourceHandler
	{
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

			int LoadResource(const Utilz::GUID& guid, 
				const std::function<InvokeReturn(const Utilz::GUID&, void*, size_t)>& invokeCallback, 
				LoadFlags loadFlags = LoadFlags::LOAD_FOR_RAM)override;

			int LoadResource(const Utilz::GUID& guid,
				const Callbacks& callbacks,
				LoadFlags loadFlags)override;

			void UnloadResource(const Utilz::GUID& guid, UnloadFlags unloadFlags)override;


		private:
			InitializationInfo initInfo;


			/****************	Unloading		*****************/
			void LinearUnload(size_t addedSize);
			typedef void(ResourceHandler::*UnloadingStrategy)(size_t addedSize);
			UnloadingStrategy Unload = &ResourceHandler::LinearUnload;
			/****************	END Unloading	*****************/
	

			/****************	Entires			*****************/
			struct Data
			{
				void* data = nullptr;
				size_t size = 0;
			};

			struct ResourceInfo
			{
				Data RAMData;
				Data VRAMData;
				uint32_t refRAM = 0;
				uint32_t refVRAM = 0;
				Utilz::Delegate<void(const Utilz::GUID&, void*, size_t)> RAMdestroyCallback;
				Utilz::Delegate<void(const Utilz::GUID&, void*, size_t)> VRAMdestroyCallback;
				State state = State::DEAD;
			};
		
			std::map<Utilz::GUID, ResourceInfo, Utilz::GUID::Compare> guidToResourceInfo;


			/****************	END Entires		*****************/

			// Errors
			std::vector<std::string> errors;

			/****************	Loading			*****************/
			IAssetLoader* diskLoader;

			Utilz::ThreadPool* load_threadPool;
			Utilz::ThreadPool* invoke_threadPool;

			struct LoadJob
			{
				Utilz::GUID guid;
				Callbacks callbacks;
				LoadFlags loadFlags;
				LoadJob& operator=(const LoadJob& other) { guid = other.guid; callbacks = other.callbacks; loadFlags = other.loadFlags; return*this; }
			};

			bool Load(LoadJob job);


			std::mutex infoLock;
			std::mutex loadLock;
			/****************	END Loading		*****************/
		};
	}
}

#endif //SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_