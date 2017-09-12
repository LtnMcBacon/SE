#ifndef SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#include <IResourceHandler.h>
#include "IAssetLoader.h"
#include <map>

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
			int Initialize();

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
			void LoadResource(const Utilz::GUID& guid, const std::function<void(void* data, size_t size)>& callback);

		private:
			struct ResourceInfo
			{
				void* data;
				size_t size;
			};


			IAssetLoader* diskLoader;

			std::map<Utilz::GUID, ResourceInfo, Utilz::GUID::Compare> resourceMap;
		};
	}
}

#endif //SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_