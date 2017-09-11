#ifndef SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#define SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_
#include <IResourceHandler.h>
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

			void LoadResource(const Utilz::GUID& guid, std::function<void(void)>& callback);

		private:
			struct ResourceInfo
			{

			};


			std::map<Utilz::GUID, ResourceInfo, Utilz::GUID::Compare> resourceMap;
		};
	}
}

#endif //SE_RESOURCE_HANDLER_RESOURCE_HANDLER_H_