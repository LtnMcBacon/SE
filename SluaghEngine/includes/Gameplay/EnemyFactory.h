#ifndef SE_GAMEPLAY_ENEMY_FACTORY_H
#define SE_GAMEPLAY_ENEMY_FACTORY_H

#include <Utilz/GUID.h>
#include <map>

namespace SE
{
	namespace Gameplay
	{
		class BehaviouralTreeFactory;
		class EnemyUnit;
		struct GameBlackboard;
		class EnemyFactory
		{
		private:
			struct EnemyDescriptionStruct
			{
				Utilz::GUID meshGUID;
				Utilz::GUID behaviouralTreeGUID;
				
				int baseDamage;
				int baseDamageVariation;
				int baseHealth;
				int baseHealthVariation;
				/*Armour type must be added, but doesn't exist in the EventStruct yet.*/
				int physicalResistance;
				int magicalResistance;
				int natureResistance;
				int fireResistance;
				int waterResistance;
			};


			BehaviouralTreeFactory* SEBTFactory;
			std::map<Utilz::GUID, EnemyDescriptionStruct, Utilz::GUID::Compare> enemyData;

			int LoadEnemyFromResourceHandler(const Utilz::GUID& GUID, void* data, size_t size);
			

		public:
			struct EnemyToCreateDescription
			{
				Utilz::GUID GUID;
				bool useVariation;
				int nrOfEnemiesWithThisDescription;
			};
			bool LoadEnemyIntoMemory(Utilz::GUID GUID);
			EnemyUnit* CreateEnemy(Utilz::GUID GUID, GameBlackboard* gameBlackboard, bool useVariation = true);
			void CreateEnemies(EnemyToCreateDescription* description, GameBlackboard* gameBlackboard, EnemyUnit** unitArray, int numberOfEnemies);

			EnemyFactory();
			~EnemyFactory();

		};
	}
}


#endif