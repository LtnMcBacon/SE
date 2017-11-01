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
			void UpdateInfo(const InitializationInfo& initInfo)override;

			
			

			int LoadResource(const Utilz::GUID& guid, const LoadResourceDelegate& callback, bool async = false, Behavior behavior = Behavior::QUICK)override;
			int LoadResource(const Utilz::GUID& guid,
				const Callbacks& callbacks,
				LoadFlags loadFlags)override;
			void UnloadResource(const Utilz::GUID& guid)override;
			void UnloadResource(const Utilz::GUID& guid, UnloadFlags unloadFlags)override;


		private:
			struct LoadJob
			{
				Utilz::GUID guid;
				Callbacks callbacks;
				LoadFlags loadFlags;
				LoadJob& operator=(const LoadJob& other) { guid = other.guid; callbacks = other.callbacks; loadFlags = other.loadFlags; return*this; }
			};

			Utilz::CircularFiFo<LoadJob> loadJobs;

			struct InvokeJob
			{
				Utilz::GUID guid;
				Utilz::Delegate<InvokeReturn1(const Utilz::GUID&, void*, size_t)> invokeCallback;
				LoadFlags loadFlags;
				InvokeJob& operator=(const InvokeJob& other) { guid = other.guid; invokeCallback = other.invokeCallback; loadFlags = other.loadFlags; return *this; }
			};
			Utilz::CircularFiFo<InvokeJob> invokeJobs;


			InitializationInfo initInfo;

			void LinearUnload(size_t addedSize);

			typedef void(ResourceHandler::*UnloadingStrategy)(size_t addedSize);

			UnloadingStrategy Unload = &ResourceHandler::LinearUnload;

	
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
		
			std::vector<std::string> errors;

			int LoadSync(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback);
			int InvokeCallback(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback);

			IAssetLoader* diskLoader;

			std::map<Utilz::GUID, ResourceInfo, Utilz::GUID::Compare> guidToResourceInfo;


			bool running = false;

			std::thread loadThread;
			void LoadThreadEntry();

			std::thread invokeThread;
			void InvokeThreadEntry();


			/****************	END To Callback info	*****************/
			std::mutex infoLock;
			std::mutex loadLock;
		};
	}
}

#endif //SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_