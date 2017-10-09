#ifndef SE_GAMEPLAY_ENEMY_FACTORY_H
#define SE_GAMEPLAY_ENEMY_FACTORY_H

#include <Utilz/GUID.h>
#include <unordered_map>

namespace SE
{
	namespace GAMEPLAY
	{
		class BehaviouralTreeFactory;
		class EnemyFactory
		{
		private:
			struct EnemyDescriptionStruct
			{
				Utilz::GUID meshGUID;
				Utilz::GUID behaviouralTreeGUID;
				
				float baseDamage;
				float baseDamageVariation;
				float baseHealth;
				float baseHealthVariation;

				float physicalResistance;
				float magicalResistance;
				float natureResistance;
				float fireResistance;
				float waterResistance;
			};


			BehaviouralTreeFactory* SEBTFactory;
			std::unordered_map<Utilz::GUID, EnemyDescriptionStruct, Utilz::GUID::Compare> enemyData;

			int LoadEnemyFromResourceHandler(const Utilz::GUID& GUID, void* data, size_t size);
			

		public:

			bool LoadEnemyIntoMemory(Utilz::GUID GUID);
			

			EnemyFactory();
			~EnemyFactory();

		};
	}
}


#endif