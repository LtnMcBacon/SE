#ifndef SE_RESOURCE_HANDLER_RAW_LOADER_H_
#define SE_RESOURCE_HANDLER_RAW_LOADER_H_
#include "IAssetLoader.h"
#include <Utilz\GUID.h>
#include <map>
namespace SE
{
	namespace ResourceHandler
	{
		/**
		*
		* @brief Normal file loading.
		*
		* Loads resources the normal way, all files that can be loaded needs to be put in the 'rawLoaderEntries.txt' file with full path, one per line.
		**/
		class RawLoader : public IAssetLoader
		{
		public:
			RawLoader();
			~RawLoader();

			/**
			* @brief	Initialize the asset loader.
			*
			* @details	You only need to do this once.
			*
			* @retval 0 On success
			* @retval -1 Failed to initialize.
			*
			*/
			int Initialize();

			void GetAllGUIDsWithExtension(const Utilz::GUID& ext, std::vector<Utilz::GUID>& guids)const override;
			void GetAllGUIDsWithExtension(const Utilz::GUID& ext, std::vector<Utilz::GUID>& guids, std::vector<std::string>& names)const override;

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
			int LoadResource(const Utilz::GUID& guid, void** data)const override;

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
			bool Exist(const Utilz::GUID& guid, size_t* size)const override;
		private:
			struct Entry
			{
				Utilz::GUID ext;
				std::string path;
				std::string name;
				size_t size;
			};
			std::map<Utilz::GUID, Entry, Utilz::GUID::Compare> resourceEntires;
		};
	}
}


#endif //SE_RESOURCE_HANDLER_RAW_LOADER_H_