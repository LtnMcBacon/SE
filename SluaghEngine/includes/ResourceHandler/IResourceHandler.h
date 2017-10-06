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



namespace SE
{
	namespace ResourceHandler
	{
		enum class Behavior : uint8_t
		{
			QUICK,
			LAZY
		};
		typedef Utilz::Delegate<int(const Utilz::GUID& guid, void* data, size_t size)> LoadResourceDelegate; /**Return 0 on Success but not decrement refCount, 1 to decremeant refcount, -1 if error. */                  
		typedef int(*LoadResourceFunctionTemplate) (const Utilz::GUID& guid, void* data, size_t size);
		/**
		*
		* @brief Resource Handler Interface
		*
		* @details The virtual resource handler class.
		*
		**/

		extern "C"
		{
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
				* @param[in] callback A delegate that is called when the data has been loaded.
				* @param[in] async If the resource should be loaded with async.
				* @param[in] behavior QUICK will prioritize the resource. LAZY will try to load the resource at regular intervals.
				*
				* @retval 0 On success.
				* Example code:
				* @code
				* //Static function
				* int Load(const Utilz::GUID& guid, void* data, size_t size)
				* {
				*	//...
				* }
				* resourceHandler->LoadResource("test.objtest", &Load); // Where load is a function
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
				*	resourceHandler->LoadResource("test.objtest", {this, &A::Load});
				*	resourceHandler->LoadResource("test.objtest", [](auto guid, auto data, auto size)->int{ //Do stuff//});
				* }
				* }
				*
				* @endcode
				* @sa LoadResourceDelegate
				*/
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
}



#endif //SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_