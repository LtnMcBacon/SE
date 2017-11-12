#ifndef SE_CORE_INTERFACE_DATA_MANAGER_H_
#define SE_CORE_INTERFACE_DATA_MANAGER_H_


#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include "IManager.h"
#include <Utilz\GUID.h>
#include <variant>

namespace SE
{
	namespace Core
	{
		class IDataManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				IEntityManager* entityManager;
			};

			typedef std::variant<bool, uint32_t, float, std::string>  values;

			virtual void AddValue(const Entity entity, const Utilz::GUID key, const values value) = 0;
			virtual values GetValue(const Entity entity, const Utilz::GUID key, const values default_value) = 0;

		protected:
			IDataManager() {};
		};

		/**
		* @brief Create an instance of the DataManager
		**/
		DECLDIR_CORE IDataManager* CreateDataManager(const IDataManager::InitializationInfo& info);
	}
}

#endif //SE_CORE_INTERFACE_DATA_MANAGER_H_