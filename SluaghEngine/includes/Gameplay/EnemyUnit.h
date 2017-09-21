#ifndef _SE_GAMEPLAY_ENEMY_UNIT_H_
#define _SE_GAMEPLAY_ENEMY_UNIT_H_

#include "GameUnit.h"

namespace SE
{
	namespace Gameplay
	{
		class FlowField;
		/**
		*
		* @brief The base class for all enemies
		*
		* @details This class contains the update functions and logic for the enemies in Sluagh.
		* It inheritates from GameUnit.
		* 
		* It will also contain the following once done:
		*	-The Behavioural Tree
		*	-The GameplayBlackboard
		*	-The EnemyBlackboard
		*	
		* Enemies will eventually inherit from this class, but there should only be a few, very specific, functions that
		* would need to be overloaded (for example some kind of "SpecialAttack" function, might be able to fix this with dynamically
		* created functions (could be cool to try, but yeah... try to think when we did that last)).
		*
		* @warning Only a shell this iteration! It'll have a simple "move" action in PerformAction that moves the entity around the
		* map (according to the flowfield), but no real logic will be included.
		*
		* @sa GameUnit
		*
		**/
		class EnemyUnit : public GameUnit
		{
		private:

			/**
			* @brief	Resolve the events that has been added this frame.
			*
			* @details	Go through all the events that has been added to the
			* EnemyUnit this frame. Each enemy will handle this in different
			* ways (for instance, immunities against damage are recorded here),
			* and thus this function is virtual. Note that an event is added
			* through a fucntion in the "GameUnit" class.
			* 
			* @retval void No return value
			*
			* @warning Not implemented at all yet! Will be called from the "Update" function,
			* and resolved depending on the enemy type.
			*
			*/
			virtual void ResolveEvents();

			/**
			* @brief	Decide the action the EnemyUnit will perform this frame.
			*
			* @details	Go through the BehaviouralTree and decide what action the unit will perform
			* this frame. Note that the different enemy types has different BehaviouralTrees, but these
			* are put here through a common interface. This interface should return an action to take,
			* and the action should be performed during the "PerformAction" call.
			*
			* @retval void No return value
			*
			* @warning Not implemented! Will always return "MOVE" action, no matter what!
			*
			*/
			void DecideAction();

			/**
			* @brief	Perform the action decided through the DecideAction() call.
			*
			* @details	Here, the logic of each action the enemy can perform lies.
			* Note that an action will be a seperate class, and that the combination of action will
			* create an enemy. 
			*
			* For instance, a "move action" will check the FlowField and move the enemy in the direction
			* annotated by the flowfield.
			*
			* @param [in] dt Delta time for this frame
			* 
			* @retval void No return value
			*
			* @warning NOT IMPLEMENTED! Will always perform a "move" action!
			*
			*/
			virtual void PerformAction(float dt);

		public:
			/*Should flowfield be a part of EnemyUnit?
			 * That is, should "EnemyUnit" hold a reference
			 * to the current (From the enemy's sight) room's flowfield,
			 * or should it be sent in every update frame?
			 */

			 /**
			 * @brief	Update the enemy
			 *
			 * @details	This function calls all the update functions
			 * for an enemy. Each update part should be defined in enemy,
			 * and update parts special for a single class should either
			 * be defined inside a virtual function in enemy or called 
			 * from a virtual function in enemy. Update should under no 
			 * circumstances be changed to virtual!
			 *
			 * @param [in] dt Delta time for this frame
			 *
			 * @retval void No value
			 
			 * @warning Due to design decisions not being taken as of now, the parameters has yet to be defined. Thus, the param
			 * comment can not be used for documentation at this moment.
			 *
			 */
			void Update(float dt/*FlowField, Outgoing events?*/);


			enum class EnemyActions
			{
				ENEMY_ACTION_NOTHING,
				ENEMY_ACTION_MOVE
			};

		private:
			EnemyUnit() {};
			EnemyUnit(const EnemyUnit& other) = delete;
			EnemyUnit(const EnemyUnit&& other) = delete;
			EnemyUnit& operator=(const EnemyUnit& rhs) = delete;

			EnemyActions entityAction = EnemyActions::ENEMY_ACTION_NOTHING;
			FlowField* flowFieldForRoom = nullptr;;

		public:
			//EnemyUnit(); <- Create a "real" constructor
			EnemyUnit(FlowField* roomFlowField, float xPos, float yPos, float maxHealth);
			~EnemyUnit();
		};

	}
}




#endif
