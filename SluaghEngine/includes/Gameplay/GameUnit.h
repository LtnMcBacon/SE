#ifndef _SE_GAMEPLAY_GAME_UNIT_H_
#define _SE_GAMEPLAY_GAME_UNIT_H_
#include <Core\Entity.h>
#include <vector>
#include "EventStructs.h"
#include <memory>

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The base class for all units in the game.
		*
		* @details This class is used as the base for the Player and Enemy classes. Here, information that is shared between the two
		* classes are placed, so to minimized duplicated code data. NOTE: So far, no need for virtual functions has been found.
		*
		* @warning Not fully implemented, due to many decisions not being done.
		*
		* @sa Enemy, Player.
		*
		**/
		class GameUnit : public std::enable_shared_from_this<GameUnit>
		{
		public:
			GameUnit();
			GameUnit(float xPos, float yPos, float maxHealth);
			virtual ~GameUnit();

			/**
			* @brief	Move the unit along a 2-Dimensional vector
			*
			* @details	This function is used to move the Unit along a 2-Dimensional
			* vector specified by the unit itself. That is, this function is used when
			* the Update() function in one way or another causes movement. NOTE: The
			* unit will move as if the world was a 2D world, but this function will translate
			* that movement into a 3D space.
			*
			*
			* @param[in] xMovement The movement in x-axis (absolute movement)
			* @param[in] yMovement The movement in y-axis (absolute movement)

			* @retval void No return value
			*
			* @warning Note that this function treats the movement in 2D-space. The "y-movement" is thus the z-movement.
			*
			*/
			void MoveEntity(float xMovement, float yMovement);
			/**
			* @brief	Put the unit at a certain position on the map.
			*
			* @details	Instantly move the unit to the defined coordinates
			* specified by the unit itself. That is, this function is used when
			* the Update() function in one way or another causes movement. NOTE: The
			* unit will move as if the world was a 2D world, but this function will translate
			* that movement into a 3D space.
			*
			*
			* @param[in] xPos New x-position for the entity
			* @param[in] yPos New y-position for the entity
			*
			* @retval void No return value
			*
			* @warning Note that this function treats the movement in 2D-space. The "y-movement" is thus the z-movement.
			*
			*/
			void PositionEntity(float xPos, float yPos);

			/**
			* @brief	Returns if entity is alive or not
			*
			* @details	if health is 0 returns false, else returns true
			*/
			inline bool IsAlive() const { return this->health > 0; };

			/**
			* @brief	Qeueu up damage events in vectors
			*
			* @details	Adds a struct of type DamageEvent to the DamageEventVector
			*			
			* @param[in] DamageEvent dmg being the desired DamageEvent enum to add to the qeueu
			*
			* Example code:
			* @code
			*	My_Dude_Took_Damage.AddDamageEvent(TheDamageHeTook);
			* @endcode
			*/
			inline void AddDamageEvent(DamageEvent dmg) { DamageEventVector.push_back(dmg); };

			/**
			* @brief	Qeueu up Healing events in vectors
			*
			* @details	Adds a struct of type HealingEvent to the HealingEventVector
			*			
			* @param[in] HealingEvent healing being the desired HealingEvent enum to add to the qeueu
			*
			* Example code:
			* @code
			*	My_Dude_Took_Healing.AddHealingEvent(TheHealingHeTook);
			* @endcode
			*/
			inline void AddHealingEvent(HealingEvent healing) { HealingEventVector.push_back(healing); };

			/**
			* @brief	Qeueu up Condition events in vectors
			*
			* @details	Adds a struct of type ConditionEvent to the ConditionEventVector
			*			
			* @param[in] ConditionEvent condition being the desired ConditionEvent enum to add to the qeueu
			*
			* Example code:
			* @code
			*	My_Dude_Took_A_Condition.AddConditionEvent(TheConditionHeTook);
			* @endcode
			*/
			inline void AddConditionEvent(ConditionEvent condition) { ConditionEventVector.push_back(condition); };

			/*
			* @brief	Clears the Damage Vector
			*
			* @details	Calls the .clear(); function for the EventVector
			*
			* @warning Dont call this if you have valuable information stored in the vectors
			*
			* Example code:
			* @code
			*	Entity_Which_Vectors_I_Want_To_Clear.ClearDamageEvents();
			*/
			void ClearDamageEvents();

			/*
			* @brief	Clears the Healing Vector
			*
			* @details	Calls the .clear(); function for the EventVector
			*
			* @warning Dont call this if you have valuable information stored in the vectors
			*
			* Example code:
			* @code
			*	Entity_Which_Vectors_I_Want_To_Clear.ClearHealingEvents();
			*/
			void ClearHealingEvents();

			/*
			* @brief	Clears the Condition Vector
			*
			* @details	Calls the .clear(); function for the EventVector
			*
			* @warning Dont call this if you have valuable information stored in the vectors
			*
			* Example code:
			* @code
			*	Entity_Which_Vectors_I_Want_To_Clear.ClearConditionEvents();
			*/
			void ClearConditionEvents();

			inline float GetHealth() const { return health; };
			inline float GetXPosition() { return xPos; };
			inline float GetYPosition() { return yPos; };
			inline float GetZPosition() { return zPos; };

			inline void SetXPosition(float value) { xPos = value; };
			inline void SetYPosition(float value) { yPos = value; };
			inline void SetZPosition(float value) { zPos = value; };

			inline std::shared_ptr<GameUnit*> GetSharedPtr() const{ return mySelf; }  ;
			//Transforms and the like will be created inside the EnemyFactory, and outside of this class. During the sprint, this will most likely be done in the playstate

		protected:
			Core::Entity unitEntity = {};

			//Life. Float needed?
			float health;
			float xPos;
			float yPos;
			float zPos;
			float extents;

			float stunDuration;


			/*Functions to move the GameUnit*/
			std::shared_ptr<GameUnit*> mySelf;
			
			
			std::vector<DamageEvent> DamageEventVector;
			std::vector<HealingEvent> HealingEventVector;
			std::vector<ConditionEvent> ConditionEventVector;

			std::vector<DamageEvent> NextFrameDamage;
			std::vector<HealingEvent> NextFrameHealing;
			std::vector<ConditionEvent> NextFrameCondition;

			//Weapons? Needed here?

		public:
			inline const Core::Entity &GetEntity() const
			{
				return unitEntity;
			}

			inline float GetExtent()
			{
				return extents;
			}

			void DestroyEntity();
			
			
		};
	}
}




#endif
