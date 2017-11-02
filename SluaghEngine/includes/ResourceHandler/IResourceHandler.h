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

		enum class InvokeReturn1 {
			FAIL = 1 << 1,
			SUCCESS = 1 << 2
		};

		enum class LoadFlags {
			LOAD_FOR_VRAM = 1 << 0,
			LOAD_FOR_RAM = 1 << 1,
			ASYNC = 1 << 2,
			IMMUTABLE = 1 << 3,
			NO_INVOKE = 1 << 4,
			NO_LOAD = 1 << 5
		};

		enum class State {
			IN_RAM = 1 << 0,
			IN_VRAM = 1 << 1,
			LOADING = 1 << 2,
			DEAD = 1 << 3,
			FAIL = 1 << 4,
			LOADED = 1 << 5,
			IMMUTABLE = 1 << 6
		};

		enum class UnloadFlags {
			VRAM = 1 << 0,
			RAM = 1 << 1
		};

		struct Callbacks
		{
			std::function<LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)> loadCallback;
			std::function<InvokeReturn1(const Utilz::GUID&, void*, size_t)> invokeCallback;
			std::function<void(const Utilz::GUID&, void*, size_t)> destroyCallback;
		};



		enum class InvokeReturn : uint8_t
		{
			Success,
			Fail,
			DecreaseRefcount,
			Hold
		};



		typedef Utilz::Delegate<InvokeReturn(const Utilz::GUID& guid, void* data, size_t size)> LoadResourceDelegate;

		


		enum class Behavior : uint8_t
		{
			QUICK,
			LAZY
		};

		enum class UnloadingStrategy : uint8_t
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
			*
			* @warning This is a warning
			*
			* Example code:
			* @code
			*	example usage of the function. Note that links will be automatically generated to documented entities
			* @endcode
			*/
			virtual int Initialize(const InitializationInfo& initInfo) = 0;

			virtual void Shutdown() = 0;

			virtual void UpdateInfo(const InitializationInfo& initInfo) = 0;

			virtual int LoadResource(const Utilz::GUID& guid,
				const Callbacks& callbacks,
				LoadFlags loadFlags) = 0;
			virtual void UnloadResource(const Utilz::GUID& guid, UnloadFlags unloadFlags) = 0;
			virtual int LoadResource(const Utilz::GUID& guid, const LoadResourceDelegate& callback, bool async = false, Behavior behavior = Behavior::QUICK) = 0;

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
			virtual void UnloadResource(const Utilz::GUID& guid) = 0;

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
ENUM_FLAGS(SE::ResourceHandler::InvokeReturn1);
ENUM_FLAGS(SE::ResourceHandler::LoadFlags);
ENUM_FLAGS(SE::ResourceHandler::State);
ENUM_FLAGS(SE::ResourceHandler::UnloadFlags);


#endif //SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_