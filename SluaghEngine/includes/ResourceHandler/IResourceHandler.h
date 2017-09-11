#ifndef SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_
#include <Utilz\GUID.h>
#include <functional>
namespace SE
{
	namespace ResourceHandler
	{		
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
			virtual ~IResourceHandler() = 0;

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

			/**
			* @brief	Load the given resource
			*
			* @param[in] guid The GUID of the resource to be loaded.
			* @param[out] callback A callback function that is called when the data has been loaded.
			*
			* Example code:
			* @code
			*	LoadResource(guid, [](void*data, size_t size){ parse_data(data, size);};
			* @endcode
			*/
			virtual void LoadResource(const Utilz::GUID& guid, const std::function<void(void* data, size_t size)>& callback) = 0;

		protected:
			IResourceHandler() {};
			IResourceHandler(const IResourceHandler& other) = delete;
			IResourceHandler(const IResourceHandler&& other) = delete;
			IResourceHandler& operator=(const IResourceHandler& other) = delete;
		};
	}
}


#endif //SE_RESOURCE_HANDLER_IRESOURCE_HANDLER_H_