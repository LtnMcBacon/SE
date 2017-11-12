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
using namespace std::literals;

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

			typedef std::variant<bool, int32_t, float, std::string>  values;

			/**
			* @brief	Set the value of a variable. If new variable it will be created.
			*
			* @param[in] entity The entity.
			* @param[in] key The variable name.
			* @param[in] value The value.
			* @warning String must be passed with the suffix s, "A String"s. Otherwise it will be casted to a bool.
			*
			*/
			virtual void SetValue(const Entity entity, const Utilz::GUID key, const values value) = 0;

			/**
			* @brief	Get the value of a variable, if it does not exist return a default.
			*
			* @param[in] entity The entity.
			* @param[in] key The variable name.
			* @param[in] default_value The default value if not found.
			* @warning String must be passed with the suffix s, "A String"s. Otherwise it will be casted to a bool.
			*
			*/
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