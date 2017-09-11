#ifndef SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#include <IResourceHandler.h>
#include "IAssetLoader.h"
#include <map>

namespace SE
{
	namespace ResourceHandler
	{

		class ResourceHandler : public IResourceHandler
		{
		public:
			ResourceHandler();
			~ResourceHandler();

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