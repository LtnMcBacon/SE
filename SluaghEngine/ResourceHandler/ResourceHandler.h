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
			int LoadResources(const LoadStruct& toLoad, bool async = false, Behavior behavior = Behavior::QUICK)override;
			void UnloadResource(const Utilz::GUID& guid)override;
		
		private:
			InitializationInfo initInfo;

			void LinearUnload(size_t addedSize);

			typedef void(ResourceHandler::*UnloadingStrategy)(size_t addedSize);

			UnloadingStrategy Unload;

			/**
			* @brief	Allocate more memory
			*/
			void Allocate(size_t size);
			/**
			* @brief	Remove an entry
			*/
			void Destroy(size_t index);

			enum class State : uint8_t
			{
				Loaded,
				Loading,
				Dead
			};

			struct Data
			{
				void* data;
				size_t size;
			};
			struct ResourceInfo
			{
				static const size_t size = sizeof(Data) + sizeof(uint16_t) + sizeof(State) + sizeof(Utilz::GUID);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Data* resourceData;
				uint16_t* refCount;
				State* state;
			};
		

			int LoadSync(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback);
			int InvokeCallback(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback);

			IAssetLoader* diskLoader;

			ResourceInfo resourceInfo;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToResourceInfoIndex;


			bool running = false;

			struct ToLoadInfo
			{
				Utilz::GUID guid;
				size_t resourceInfoIndex;
				LoadResourceDelegate callback;
				Behavior behavior;
			};

			/****************	To Load info	*****************/
			

			Utilz::CircularFiFo<ToLoadInfo> toLoad;
			std::thread toLoadThread;

			void ToLoadThreadEntry();


			/****************	END To Load info	*****************/


			/****************	To Invoke info	*****************/

			Utilz::CircularFiFo<ToLoadInfo> toInvoke;
			std::thread toInvokeThread;

			void ToInvokeThreadEntry();


			/****************	END To Callback info	*****************/
			std::mutex infoLock;

		};
	}
}

#endif //SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_