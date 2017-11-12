#ifndef SE_CORE_DATA_MANAGER_H_
#define SE_CORE_DATA_MANAGER_H_

#include <IDataManager.h>

namespace SE
{
	namespace Core
	{
		class DataManager : public IDataManager
		{
		public:
			DataManager();
			~DataManager();

			void AddValue(const Entity entity, const Utilz::GUID key, const values value)override;
			values GetValue(const Entity entity, const Utilz::GUID key, const values default_value) override;

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;
		private:
			/**
			* @brief	Allocate more memory
			*/
			void Allocate(size_t size);
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index)override;
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(const Entity& entity)override;
			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection()override;
		};
	}
}

#endif // SE_CORE_DATA_MANAGER_H_