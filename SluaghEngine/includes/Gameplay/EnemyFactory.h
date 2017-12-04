#ifndef SE_GAMEPLAY_ENEMY_FACTORY_H
#define SE_GAMEPLAY_ENEMY_FACTORY_H

#include <Utilz/GUID.h>
#include <map>
#include <ResourceHandler\IResourceHandler.h>
#include <Core/Entity.h>
#include "EnemyUnit.h"

namespace SE
{
	namespace Gameplay
	{
		class BehaviouralTreeFactory;
		class BehaviouralTree;

		struct GameBlackboard;
		struct EnemyBlackboard;


		

		/**
		*
		* @brief The information for the enemy to be created
		*
		* @warning ALL DATA MUST BE SET!
		*
		**/
		struct EnemyCreationData
		{
			EnemyType type;
			float startX, startY;
			bool useVariation;
		};

		struct EnemyCreationStruct
		{
			std::vector<EnemyCreationData> information;
		};

		/**
		*
		* @brief The enemy factory to create enemies through.
		*
		* @warning LoadEnemyIntoMemory MUST be called before CreateEnemy.
		*
		**/
		class EnemyFactory
		{
		private:
			/**
			 * @brief Internal struct to save Enemy Descriptions
			 */
			struct EnemyDescriptionStruct
			{
				Utilz::GUID meshGUID;
				Utilz::GUID skeletonGUID;
				Utilz::GUID deathAnimationGUID;
				Utilz::GUID behaviouralTreeGUID;
				Utilz::GUID materialGUID;
				Utilz::GUID shaderGUID;
				Utilz::GUID deathSoundGUID;
				
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
			std::map<EnemyType, EnemyDescriptionStruct> enemyData;
			std::map<Utilz::GUID, EnemyType, Utilz::GUID::Compare> enemyTypes;

			inline EnemyType GUIDToEnemyType(const Utilz::GUID& GUID)
			{
				return enemyTypes[GUID];
			}

			/**
			* @brief	Load an enemy into memory. Must be called before an enemy can be created.
			*
			* @details	Load an enemy specifed by the GUID into memory. Uses LoadEnemyFromResourceHandler
			* to parse the data
			*
			* @param[in] GUID The GUID for the file containing the enemy.

			* @retval true The enemy is loaded into memory and enemies can be created.
			* @retval false The enemy could not be loaded.
			*/
			bool LoadEnemyIntoMemory(Utilz::GUID GUID);
			
			
		public:



			/**
			* @brief	Create an enemy already loaded into the memory, using the GUID as identifier.
			*
			* @details  Create an enemy from the data previously read into memory. Bind a mesh,
			* set the entity as renderable. Does NOT set a position for the enemy!
			*
			*
			* @param[in] descriptions Descriptions for the enemies to be created
			* @param[in] gameBlackboard The blackboard for the game.
			* @param[out] unitArray The array to put all created units in.
			*
			* @warning ALL DESCRIPTIONS MUST BE COMPLETELY FILLED IN!
			* 
			* @sa EnemyCreationStruct.
			*
			*/
			void CreateEnemies(const EnemyCreationStruct &descriptions, GameBlackboard* gameBlackboard, EnemyUnit** unitArray);

			/**
			 * @brief Create an entity and load all "entity related" data into the system (meshes, animations...)
			 */
			Core::Entity CreateEntityDataForEnemyType(EnemyType type);
			EnemyUnit* CreateEnemyDataForEnemyType(EnemyType type, bool useVariation);
			BehaviouralTree* CreateBehaviouralTreeForEnemyType(EnemyType type, GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard);


			EnemyFactory();
			~EnemyFactory();

		};
	}
}


#endif