#ifndef SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#include <Utilz\GUID.h>
#include <functional>
#include <Utilz\Delegator.h>

namespace SE
{
	namespace ResourceHandler
	{		
		typedef Utilz::Delegate<int(const Utilz::GUID& guid, void* data, size_t size)> LoadResourceDelegate;
		typedef int(*LoadResourceFunctionTemplate) (const Utilz::GUID& guid, void* data, size_t size);
		/**
		*
		* @brief Resource Handler Interface
		*
		* @details The virtual resource handler class.
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
			virtual int Initialize() = 0;

			virtual void Shutdown() = 0;

			/**
			* @brief	Load the given resource
			*
			* @param[in] guid The GUID of the resource to be loaded.
			* @param[out] callback A delegate that is called when the data has been loaded.
			*
			* @retval 0 On success.
			* Example code:
			* @code
			* //Static function
			* int Load(const Utilz::GUID& guid, void* data, size_t size)
			* {
			*	//...
			* }
			* resourceHandler->LoadResource(Utilz::GUID("test.objtest"), ResourceHandler::LoadResourceDelegate::Make<&Load>()); // Where load is a function
			*
			* //Class method
			* class A
			* {
			* int Load(const Utilz::GUID& guid, void* data, size_t size)
			*  {
			*	/...
			*  }
			* void Run()
			* {
			*	resourceHandler->LoadResource(Utilz::GUID("test.objtest"), ResourceHandler::LoadResourceDelegate::Make<&A, A::Load>(this)); 
			* }
			* }
			* 
			* @endcode
			* @sa LoadResourceDelegate
			*/
			virtual int LoadResource(const Utilz::GUID& guid, const LoadResourceDelegate& callback, bool wait = true) = 0;

			/**
			* @brief	Unload the given resource
			*
			* @details Decreases the refcount for the given resource.
			* When the refcount reaches 0 the resource handler may unload the resource.
			* Either by dumping the memory to disk, or just discarding it.
			*
			* @param[in] guid The GUID of the resource to be unloaded.
			*
			**/
			virtual void UnloadResource(const Utilz::GUID& guid) = 0;

			protected:
			IResourceHandler() {};
			IResourceHandler(const IResourceHandler& other) = delete;
			IResourceHandler(const IResourceHandler&& other) = delete;
			IResourceHandler& operator=(const IResourceHandler& other) = delete;
		};
	}
}


#endif //SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_