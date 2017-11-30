#ifndef SE_RESOURCE_HANDLER_IASSET_LOADER_H_
#define SE_RESOURCE_HANDLER_IASSET_LOADER_H_
#include <Utilz\GUID.h>
#include <vector>

namespace SE
{
	namespace ResourceHandler
	{
		/**
		*
		* @brief Interface class for asset loaders
		*
		* @details Load a resource by first creating the assetloader then loading resources
		* auto loader = new AssetLoader;
		* loader->Initialize();
		* loader->LoadResource(guid, &data, &size);
		* loader->LoadResource(guid2, &data, &size);
		**/
		class IAssetLoader
		{
		public:
			virtual ~IAssetLoader() {};

			/**
			* @brief	Initialize the asset loader.
			*
			* @details	You only need to do this once.
			*
			* @retval 0 On success
			* @retval -1 Failed to initialize.
			*
			*/
			virtual int Initialize() = 0;

			virtual void GetAllGUIDsWithExtension(const Utilz::GUID& ext, std::vector<Utilz::GUID>& guids)const = 0;
			virtual void GetAllGUIDsWithExtension(const Utilz::GUID& ext, std::vector<Utilz::GUID>& guids, std::vector<std::string>& names)const = 0;

			/**
			* @brief Load a resource.
			*
			* @param[in] guid The GUID of the resource to load.
			* @param[out] data A pointer where to put the data.			
			*
			* @retval 0 On success
			* @retval -1 Read failed.
			* @retval -2 Memory error.
			*
			* @warning data should not be initialized.
			*
			* Example code:
			* @code
			*	void* data;
			*	size_t size;
			*	if(loader->Exist(guid, &size))
			*		loader->LoadResource(guid, &data);
			* @endcode
			*/
			virtual int LoadResource(const Utilz::GUID& guid, void** data)const = 0;

			/**
			* @brief	Check if the asset can be loaded from this loader.
			*
			* @details	Returns true if the resource can be loaded with this loader
			* false otherwise.
			*
			* @param[in] guid GUID of the resource.
			* @param[out] size The size of the resource.
			*
			* @retval true Resource can be loaded
			* @retval false Resource can not be loaded
			*
			*/
			virtual bool Exist(const Utilz::GUID& guid, size_t* size)const = 0;
		protected:
			IAssetLoader() {};
			IAssetLoader(const IAssetLoader& other) = delete;
			IAssetLoader(const IAssetLoader&& other) = delete;
			IAssetLoader& operator=(const IAssetLoader& other) = delete;
		};
	}
}

#endif //SE_RESOURCE_HANDLER_IASSET_LOADER_H_