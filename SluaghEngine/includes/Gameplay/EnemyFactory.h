#ifndef SE_GAMEPLAY_ENEMY_FACTORY_H
#define SE_GAMEPLAY_ENEMY_FACTORY_H

#include <Utilz/GUID.h>
#include <map>
#include <ResourceHandler\IResourceHandler.h>

namespace SE
{
	namespace Gameplay
	{
		class BehaviouralTreeFactory;
		class EnemyUnit;
		struct GameBlackboard;


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

			/**
			* @brief	Load the description of an enemy from file
			*
			* @details	Called from LoadEnemyIntoMemory to parse the enemy data and put into the map.
			*
			* @param[in] GUID The GUID of the Enemy to be parsed.
			* @param[in] data The data recieved from the file.
			* @param[out] size The size of the data recieved from the file.
			*
			*/
			ResourceHandler::InvokeReturn LoadEnemyFromResourceHandler(const Utilz::GUID& GUID, void* data, size_t size);
			

		public:

			/**
			*
			* @brief Struct containing the data for creation of enemies.
			*
			**/
			struct EnemyToCreateDescription
			{
				Utilz::GUID GUID;					/**The GUID identifiying the enemy. */ 
				bool useVariation;					/**Whether variation should be used or not. */ 
				int nrOfEnemiesWithThisDescription;	/**Number of enemies that should be created with this description*/ 
			};

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

			/**
			* @brief	Create an enemy already loaded into the memory, using the GUID as identifier.
			*
			* @details  Create an enemy from the data previously read into memory. Bind a mesh,
			* set the entity as renderable. Does NOT set a position for the enemy!
			*
			*
			* @param[in] GUID The GUID for the enemy to be created
			* @param[in] gameBlackboard The blackboard for the game.
			* @param[in] useVariation (Optional) Defines if the enemy should use variation, or be created
			* with defaultvalues.
			*
			* @retval EnemyUnit* The enemy could be loaded.
			* @retval nullptr Something went wrong. Was the enemy loaded into memory?
			*
			* @warning LoadEnemyIntoMemory must be called first!
			* 
			* @sa CreateEnemies for the creation of more than one enemy.
			*
			*/
			EnemyUnit* CreateEnemy(Utilz::GUID GUID, GameBlackboard* gameBlackboard, bool useVariation = true);
			
			/**
			* @brief	Creates multiple enemies at once, as defined by the EnemyToCreateDescription
			*
			* @details	Takes one or more enemy descriptions and creates enemies from them. These enemies
			* are placed in the unitArray. 
			*
			*
			* @param[in] descriptions The descriptions to create enemies from.
			* @param[in] gameBlackboard The blackboard for the game.
			* @param[in] numberOfEnemies The size of the unitArray.
			* @param[out] unitArray The array to save all enemies into.
			*
			* @warning Will NOT return any kind of error! 
			* @warning numberOfEnemies MUST be the same as the total number of enemies totally existing in the 
			* EnemyToCreateDescription descriptions!
			*
			* @sa EnemyToCreateDescription for the description, CreateEnemy for when only one enemy is to be created.
			*
			*/
			void CreateEnemies(EnemyToCreateDescription* descriptions, GameBlackboard* gameBlackboard, int numberOfEnemies, EnemyUnit** unitArray);

			EnemyFactory();
			~EnemyFactory();

		};
	}
}


#endif