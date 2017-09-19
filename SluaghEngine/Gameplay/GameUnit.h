#ifndef _SE_GAMEPLAY_GAME_UNIT_H_
#define _SE_GAMEPLAY_GAME_UNIT_H_
#include "Core/Entity.h"
#include <vector>
#include "EventStructs.h"
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
		class GameUnit
		{
		public:
			GameUnit();
			~GameUnit();

			/**
			* @brief	Returns if entity is alive or not
			*
			* @details	if health is 0 returns false, else returns true

			* @Warning health must be put to 0 else if it is negative entity will be considered alive.
			*/
			inline bool IsAlive() const { return !!this->health; };

			/**
			* @brief	Qeueu up damage events in vectors
			*
			* @details	Adds an enum of type DamageEvent for the DamageEventVector
			*			Check EventStructs.h for enumtypes.

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
			* @details	Adds an enum of type HealingEvent for the HealingEventVector
			*			Check EventStructs.h for enumtypes.
			*
			* @param[in] HealingEvent healing being the desired HealingEvent enum to add to the qeueu
			*
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
			* @details	Adds an enum of type ConditionEvent for the ConditionEventVector
			*			Check EventStructs.h for enumtypes.
			*
			* @param[in] ConditionEvent condition being the desired ConditionEvent enum to add to the qeueu
			*
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
			* @details	Calls the .clear(); function for both the
			*			EventVector and NextFrame Vector
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
			* @details	Calls the .clear(); function for both the
			*			EventVector and NextFrame Vector
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
			* @details	Calls the .clear(); function for both the
			*			EventVector and NextFrame Vector
			*
			* @warning Dont call this if you have valuable information stored in the vectors
			*
			* Example code:
			* @code
			*	Entity_Which_Vectors_I_Want_To_Clear.ClearConditionEvents();
			*/
			void ClearConditionEvents();

			inline float GetHealth() const { return health; };

			//Transforms and the like will be created inside the EnemyFactory, and outside of this class. During the sprint, this will most likely be done in the playstate

		protected:
			Core::Entity unitEntity = {};

			//Life. Float needed?
			float health;
			
			std::vector<DamageEvent> DamageEventVector;
			std::vector<HealingEvent> HealingEventVector;
			std::vector<ConditionEvent> ConditionEventVector;

			std::vector<DamageEvent> NextFrameDamage;
			
			std::vector<HealingEvent> NextFrameHealing;
			std::vector<ConditionEvent> NextFrameCondition;

			//Weapons? Needed here?

		public:
			inline const Core::Entity& GetEntity() const
			{
				return unitEntity;
			}
			
		};
	}
}




#endif
