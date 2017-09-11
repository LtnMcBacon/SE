#ifndef SE_RESOURCE_HANDLER_IASSET_LOADER_H_
#define SE_RESOURCE_HANDLER_IASSET_LOADER_H_
#include <Utilz\GUID.h>
namespace SE
{
	namespace ResourceHandler
	{
		class IAssetLoader
		{
		public:
			virtual ~IAssetLoader() = 0;


			virtual void LoadResource(const Utilz::GUID& guid, void** data, size_t* size) = 0;
			virtual bool Exist(const Utilz::GUID& guid) = 0;
		protected:
			IAssetLoader() {};
			IAssetLoader(const IAssetLoader& other) = delete;
			IAssetLoader(const IAssetLoader&& other) = delete;
			IAssetLoader& operator=(const IAssetLoader& other) = delete;
		};
	}
}

#endif //SE_RESOURCE_HANDLER_IASSET_LOADER_H_