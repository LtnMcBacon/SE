#ifndef SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#include <Utilz\GUID.h>
#include <functional>
#include <Utilz\Delegator.h>

namespace SE
{
	namespace ResourceHandler
	{		
		typedef Utilz::Delegate<void(const Utilz::GUID& guid, void* data, size_t size)> LoadResourceDelegate;
		typedef void(*LoadResourceFunctionTemplate) (const Utilz::GUID& guid, void* data, size_t size);
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
			* Example code:
			* @code
			*	r->LoadResource(Utilz::GUID("test.objtest"), ResourceHandler::LoadResourceDelegate::Make<&Load>()); // Where load is a function
			* @endcode
			* @sa LoadResourceDelegate
			*/
			virtual void LoadResource(const Utilz::GUID& guid, const LoadResourceDelegate& callback) = 0;

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

			/**
			* @brief	Add a parser to parse the data of a resource.
			*
			* @details Here you can add a parser callback that will be called when a resource has been loaded from disk. 
			* The raw loaded data is given in the rawData pointer, the parser functions job is to parse the data and return it in the parsedData pointer.
			*
			* @param[in] guid The GUID of the file extension ( The hash of the extension ex. txt)
			*
			*/
			virtual void AddParser(const Utilz::GUID& extGUID, const std::function<int(void* rawData, size_t rawSize, void** parsedData, size_t* parsedSize)>& parserFunction) = 0;
		protected:
			IResourceHandler() {};
			IResourceHandler(const IResourceHandler& other) = delete;
			IResourceHandler(const IResourceHandler&& other) = delete;
			IResourceHandler& operator=(const IResourceHandler& other) = delete;
		};
	}
}


#endif //SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_