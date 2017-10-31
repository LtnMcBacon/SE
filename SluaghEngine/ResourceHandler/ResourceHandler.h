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

#define ENUM_FLAG_OPERATOR(T,X) inline T operator X (T lhs, T rhs) { return (T) (static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#define ENUM_FLAGS(T) \
enum class T; \
inline T operator ~ (T t) { return (T) (~static_cast<std::underlying_type_t <T>>(t)); } \
inline bool operator & (T lhs, T rhs) { return (static_cast<std::underlying_type_t <T>>(lhs) & static_cast<std::underlying_type_t <T>>(rhs)); } \
ENUM_FLAG_OPERATOR(T,|) \
ENUM_FLAG_OPERATOR(T,^) 
//enum class T
//ENUM_FLAG_OPERATOR(T,&)


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

			enum class LoadReturn
			{
				VRAM = 1 << 0,
					RAM = 1 << 1,
					FAIL = 1 << 2
			};

			enum class InvokeReturn1 {
				VRAM = 1 << 0,
				RAM = 1 << 1,
				FAIL = 1 << 2
			};

			enum class LoadFlags {
				LOAD_FOR_VRAM = 1 << 0,
				LOAD_FOR_RAM = 1 << 1,
				ASYNC = 1 << 2
			};

			enum class State {
				IN_RAM = 1 << 0,
				IN_VRAM = 1 << 1,
				LOADING = 1 << 2,
				DEAD = 1 << 3,
				FAIL = 1 << 4
			};

			struct Callbacks
			{
				Utilz::Delegate<LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)> loadCallback;
				Utilz::Delegate<InvokeReturn1(const Utilz::GUID&, void*, size_t)> invokeCallback;
				Utilz::Delegate<void(const Utilz::GUID&, void*, size_t)> destroyCallback;
			};

			int LoadResource(const Utilz::GUID& guid, const LoadResourceDelegate& callback, bool async = false, Behavior behavior = Behavior::QUICK)override;
			int LoadResource(const Utilz::GUID& guid,
				const Utilz::Delegate<LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)>& loadCallback,
				const Utilz::Delegate<InvokeReturn1(const Utilz::GUID&, void*, size_t)>& invokeCallback,
				LoadFlags loadFlags);
			void UnloadResource(const Utilz::GUID& guid)override;
		
		private:
			struct LoadJob
			{
				Utilz::GUID guid;
				Utilz::Delegate<LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)> loadCallback;
				Utilz::Delegate<InvokeReturn1(const Utilz::GUID&, void*, size_t)> invokeCallback;
				LoadFlags loadFlags;
				LoadJob& operator=(const LoadJob& other) { guid = other.guid; loadCallback = other.loadCallback; invokeCallback = other.invokeCallback; loadFlags = other.loadFlags; return*this; }
			};

			Utilz::CircularFiFo<LoadJob> loadJobs;

			struct InvokeJob
			{
				Utilz::GUID guid;
				Utilz::Delegate<InvokeReturn1(const Utilz::GUID&, void*, size_t)> invokeCallback;
				LoadFlags loadFlags;
				InvokeJob& operator=(const LoadJob& other) { guid = other.guid; invokeCallback = other.invokeCallback; loadFlags = other.loadFlags; return *this; }
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
				Data resourceData;
				uint32_t refRAM = 0;
				uint32_t refVRAM = 0;
				State state = State::DEAD;
			};
		
			std::vector<std::string> errors;

			int LoadSync(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback);
			int InvokeCallback(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback);

			IAssetLoader* diskLoader;

			std::map<Utilz::GUID, ResourceInfo, Utilz::GUID::Compare> guidToResourceInfo;


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
			std::recursive_mutex loadResourceLock;
		};
	}
}
ENUM_FLAGS(SE::ResourceHandler::ResourceHandler::LoadReturn);
ENUM_FLAGS(SE::ResourceHandler::ResourceHandler::InvokeReturn1);
ENUM_FLAGS(SE::ResourceHandler::ResourceHandler::LoadFlags);
ENUM_FLAGS(SE::ResourceHandler::ResourceHandler::State);

#endif //SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_