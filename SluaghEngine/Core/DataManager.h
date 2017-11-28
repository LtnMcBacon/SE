#ifndef SE_CORE_DATA_MANAGER_H_
#define SE_CORE_DATA_MANAGER_H_

#include <IDataManager.h>
#include <random>

namespace SE
{
	namespace Core
	{

		class DataManager : public IDataManager
		{
	

		public:
			DataManager(const IDataManager::InitializationInfo& info);
			~DataManager();

			void SetValue(const Entity entity, const Utilz::GUID key, const values value)override;
			values GetValue(const Entity entity, const Utilz::GUID key, const values default_value) override;

			void Copy(const Entity to, const Entity from)override;
			std::optional<std::reference_wrapper< std::unordered_map<Utilz::GUID, values, Utilz::GUID::Hasher>>> GetAllValues(const Entity entity)override;
			bool WriteToFile(const Entity entity, std::ofstream& file)override;
			void CreateFromFile(const Entity entity, std::ifstream& file) override;
			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(const Entity& entity)override;
		private:
			void Destroy(size_t index)override;

			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection()override;

			void CreateData(const Entity entity);
			
			struct Entry
			{
				Entity entity;
				std::unordered_map<Utilz::GUID, values, Utilz::GUID::Hasher> values;
			};

			std::vector<Entry> entries;
			std::unordered_map<Entity, size_t, EntityHasher> entityToIndex;

			IDataManager::InitializationInfo initInfo;
			std::default_random_engine generator;

		};
	}
}

#endif // SE_CORE_DATA_MANAGER_H_