#ifndef SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#include <Utilz\GUID.h>
#include <functional>
#include <Utilz\Delegate.h>

#if defined DLL_EXPORT_RH
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif


#define ENUM_FLAG_OPERATOR(T,X) inline T operator X (T lhs, T rhs) { return (T) (static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#define ENUM_FLAG_OPERATOR2(T,X) inline void operator X= (T& lhs, T rhs) { lhs = (T)(static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#define ENUM_FLAGS(T) \
enum class T; \
inline T operator ~ (T t) { return (T) (~static_cast<std::underlying_type_t <T>>(t)); } \
inline bool operator & (T lhs, T rhs) { return (static_cast<std::underlying_type_t <T>>(lhs) & static_cast<std::underlying_type_t <T>>(rhs));  } \
ENUM_FLAG_OPERATOR2(T,|) \
ENUM_FLAG_OPERATOR2(T,&) \
ENUM_FLAG_OPERATOR(T,|) \
ENUM_FLAG_OPERATOR(T,^)
//enum class T
//ENUM_FLAG_OPERATOR(T,&)

#include <functional>

namespace SE
{
	namespace ResourceHandler
	{

		enum class LoadReturn
		{
			SUCCESS = 1 << 0,
			FAIL = 1 << 1,
			NO_DELETE = 1 << 2
		};

		enum class InvokeReturn {
			FAIL = 1 << 1,
			SUCCESS = 1 << 2,
			DEC_RAM  = 1 << 3,
			DEC_VRAM = 1 << 4
		};

		enum class LoadFlags {
			LOAD_FOR_VRAM = 1 << 0,
			LOAD_FOR_RAM = 1 << 1,
			ASYNC = 1 << 2,
			IMMUTABLE = 1 << 3
		};

		enum class State {
			IN_RAM = 1 << 0,
			IN_VRAM = 1 << 1,
			LOADING = 1 << 2,
			DEAD = 1 << 3,
			FAIL = 1 << 4,
			LOADED = 1 << 5,
			IMMUTABLE = 1 << 6,
			RAW  = 1 << 7
		};

		enum class UnloadFlags {
			VRAM = 1 << 0,
			RAM = 1 << 1
		};

		struct Callbacks
		{
			std::function<LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)> loadCallback;
			std::function<InvokeReturn(const Utilz::GUID&, void*, size_t)> invokeCallback;
			std::function<void(const Utilz::GUID&, void*, size_t)> destroyCallback;
		};

		enum class UnloadingStrategy
		{
			Linear,
			FIFO
		};
	
		struct InitializationInfo
		{
			size_t maxMemory = 256*1024*1024;
			UnloadingStrategy unloadingStrat = UnloadingStrategy::Linear;
		};

		/**
		*
		* @brief Resource Handler Interface
		*
		* @details The virtual resource handler class. The resource handler is used to loaded all assets.
		* @sa LoadResource
		*
		**/
		class IResourceHandler
		{
		public:
			virtual ~IResourceHandler() {};

			/**
			* @brief	Initialize the resource handler
			*
			* @retval 0 On success.
			* @retval -1 Asset loader failed to load
			*/
			virtual int Initialize(const InitializationInfo& initInfo) = 0;

			virtual void Shutdown() = 0;

			virtual const InitializationInfo& GetInfo()const = 0;
			virtual void UpdateInfo(const InitializationInfo& initInfo) = 0;

			virtual int LoadResource(const Utilz::GUID& guid,
				const Callbacks& callbacks,
				LoadFlags loadFlags) = 0;
			
			virtual int LoadResource(const Utilz::GUID& guid, 
				const std::function<InvokeReturn(const Utilz::GUID&, void*, size_t)>& invokeCallback, 
				LoadFlags loadFlags = LoadFlags::LOAD_FOR_RAM) = 0;

			/**
			* @brief	Unload the given resource
			*
			* @details Decreases the refcount for the given resource.
			* When the refcount reaches 0 the resource handler may unload the resource.
			* Either by dumping the memory to disk, or just discarding it.
			*
			* @param[in] guid The GUID of the resource to be unloaded.
			* @warning This does not force the resource to unload!
			**/
			virtual void UnloadResource(const Utilz::GUID& guid, UnloadFlags unloadFlags) = 0;

		protected:
			IResourceHandler() {};
			IResourceHandler(const IResourceHandler& other) = delete;
			IResourceHandler(const IResourceHandler&& other) = delete;
			IResourceHandler& operator=(const IResourceHandler& other) = delete;
		};

		DECLDIR IResourceHandler* CreateResourceHandler();
	}
}

ENUM_FLAGS(SE::ResourceHandler::LoadReturn);
ENUM_FLAGS(SE::ResourceHandler::InvokeReturn);
ENUM_FLAGS(SE::ResourceHandler::LoadFlags);
ENUM_FLAGS(SE::ResourceHandler::State);
ENUM_FLAGS(SE::ResourceHandler::UnloadFlags);


#endif //SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_