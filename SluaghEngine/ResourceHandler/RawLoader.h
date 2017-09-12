#ifndef SE_RESOURCE_HANDLER_RAW_LOADER_H_
#define SE_RESOURCE_HANDLER_RAW_LOADER_H_
#include "IAssetLoader.h"
#include <Utilz\GUID.h>
#include <map>
namespace SE
{
	namespace ResourceHandler
	{
		class RawLoader : public IAssetLoader
		{
		public:
			RawLoader();
			~RawLoader();

			int Initialize();

			int LoadResource(const Utilz::GUID& guid, void** data, size_t* size)const;
			bool Exist(const Utilz::GUID& guid)const;
		private:
			std::map<Utilz::GUID, std::string, Utilz::GUID::Compare> resourceEntires;
		};
	}
}


#endif //SE_RESOURCE_HANDLER_RAW_LOADER_H_