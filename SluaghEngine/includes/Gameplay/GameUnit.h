#ifndef _SE_GAMEPLAY_GAME_UNIT_H_
#define _SE_GAMEPLAY_GAME_UNIT_H_
#include <Core\Entity.h>
#include <vector>
#include "EventStructs.h"
#include <memory>
#include <DirectXMath.h>
#include "Utilz/GUID.h"
#include "Stats.h"

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
			* @details	If this is called on a "dead" entity, the entities deathAnim (if applicable) is performed. Once the death
			* anim has finished, the function returns false.
			*/
			bool IsAlive();

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
			inline Utilz::GUID GetDeathAnimation() { return deathAnimation; };

			inline void SetXPosition(float value) { xPos = value; };
			inline void SetYPosition(float value) { yPos = value; };
			inline void SetZPosition(float value) { zPos = value; };

			inline void SetDeathAnimation(Utilz::GUID deathAnim) { deathAnimation = deathAnim; };

			//Stat Getters
			inline int GetBaseStrength			  (){ return baseStat.str; };
			inline int GetBaseAgility			  (){ return baseStat.agi; };
			inline int GetBaseWhisdom			  (){ return baseStat.whi; };
			inline int GetArmourCap				  (){ return baseStat.armorCap; };

			inline float GetBaseMaxHealth		  (){ return baseStat.health; };
			inline float GetBaseDamage			  (){ return baseStat.damage; };
			inline float GetBaseDamageMultipyier  (){ return baseStat.meleeMultiplier; };
			inline float GetBasePhysicalResistance(){ return baseStat.damage; };
	  
			inline float GetBaseRangedDamage	  (){ return baseStat.rangedDamage; };
			inline float GetBaseRangedMulitplier  (){ return baseStat.rangedMultiplier; };
			inline float GetBaseMovement		  (){ return baseStat.movementSpeed; };

			inline float GetBaseMagicDamage		  (){ return baseStat.magicDamage; };
			inline float GetBaseMagicMulitplier	  (){ return baseStat.magicMultiplier; };
			inline float GetBaseMagicResistance	  (){ return baseStat.magicResistance; };
			inline float GetBaseNatureResistance  (){ return baseStat.natureResistance; };
			inline float GetBaseFireResistance	  (){ return baseStat.fireResistance; };
			inline float GetBaseWaterResistance	  (){ return baseStat.waterResistance; };

			inline ArmourType GetArmour			  (){ return baseStat.armour; };
			inline DamageSources GetWeapon		  (){ return baseStat.weapon; };
			inline DamageType GetDamageType		  (){ return baseStat.damageType; };

			//Stat Setters
			inline void SetBaseStrength			 (int str)						{ this->baseStat.str = str;};
			inline void SetBaseAgility			 (int agi)						{ this->baseStat.agi = agi; };
			inline void SetBaseWhisdom			 (int whi)						{ this->baseStat.whi = whi; };
			inline void SetArmourCap			 (int cap)						{ this->baseStat.armorCap = cap; };

			inline void SetBaseMaxHealth		 (float health)					{ this->baseStat.health = health; };
			inline void SetBaseDamage			 (float damage)					{ this->baseStat.damage = damage; };
			inline void SetBaseDamageMultipyier  (float multiplier)				{ this->baseStat.meleeMultiplier = multiplier; };
			inline void SetBasePhysicalResistance(float resistance)				{ this->baseStat.physicalResistance = resistance; };

			inline void SetBaseRangedDamage		 (float damage)					{ this->baseStat.rangedDamage = damage; };
			inline void SetBaseRangedMulitplier  (float multiplier)				{ this->baseStat.rangedMultiplier = multiplier; };
			inline void SetBaseMovement			 (float speed)					{ this->baseStat.movementSpeed = speed; };

			inline void SetBaseMagicDamage		 (float damage)					{ this->baseStat.magicDamage = damage; };
			inline void SetBaseMagicMulitplier	 (float multiplier)				{ this->baseStat.magicMultiplier = multiplier; };
			inline void SetBaseMagicResistance	 (float resistance)				{ this->baseStat.magicResistance = resistance; };
			inline void SetBaseNatureResistance  (float resistance)				{ this->baseStat.natureResistance = resistance; };
			inline void SetBaseFireResistance	 (float resistance)				{ this->baseStat.fireResistance = resistance; };
			inline void SetBaseWaterResistance	 (float resistance)				{ this->baseStat.waterResistance = resistance; };

			inline void	 SetArmour				 (unsigned short int armour)	{ this->baseStat.armour = static_cast<ArmourType>(armour); };
			inline void  SetWeapon				 (unsigned short int weapon)	{ this->baseStat.weapon = static_cast<DamageSources>(weapon);
			};
			inline void  SetDamageType			 (unsigned short int damageType){ this->baseStat.damageType = static_cast<DamageType>(damageType);
			};

			//Stat Adders
			inline void AddBaseStrength			 (int str)						{ this->baseStat.str += str;};
			inline void AddBaseAgility			 (int agi)						{ this->baseStat.agi += agi;};
			inline void AddBaseWhisdom			 (int whi)						{ this->baseStat.whi += whi;};
			inline void AddArmourCap			 (int cap)						{ this->baseStat.armorCap += cap;};

			inline void AddBaseMaxHealth		 (float health)					{ this->baseStat.health += health; };
			inline void AddBaseDamage			 (float damage)					{ this->baseStat.damage += damage; };
			inline void AddBaseDamageMultipyier  (float multiplier)				{ this->baseStat.meleeMultiplier += multiplier; };
			inline void AddBasePhysicalResistance(float resistance)				{ this->baseStat.physicalResistance += resistance; };

			inline void AddBaseRangedDamage		 (float damage)					{ this->baseStat.rangedDamage += damage; };
			inline void AddBaseRangedMulitplier  (float multiplier)				{ this->baseStat.rangedMultiplier += multiplier; };
			inline void AddBaseMovement			 (float speed)					{ this->baseStat.movementSpeed += speed; };

			inline void AddBaseMagicDamage		 (float damage)					{ this->baseStat.magicDamage += damage; };
			inline void AddBaseMagicMulitplier	 (float multiplier)				{ this->baseStat.magicMultiplier += multiplier; };
			inline void AddBaseMagicResistance	 (float resistance)				{ this->baseStat.magicResistance += resistance; };
			inline void AddBaseNatureResistance  (float resistance)				{ this->baseStat.natureResistance += resistance; };
			inline void AddBaseFireResistance	 (float resistance)				{ this->baseStat.fireResistance += resistance; };
			inline void AddBaseWaterResistance	 (float resistance)				{ this->baseStat.waterResistance += resistance; };

			//Stat Removers
			inline void RemoveBaseStrength			(int str)					   { this->baseStat.str -= str;};
			inline void RemoveBaseAgility			(int agi)					   { this->baseStat.agi -= agi;};
			inline void RemoveBaseWhisdom			(int whi)					   { this->baseStat.whi -= whi;};
			inline void RemoveArmourCap				(int cap)					   { this->baseStat.armorCap -= cap;};

			inline void RemoveBaseMaxHealth			(float health)				   { this->baseStat.health -= health; };
			inline void RemoveBaseDamage			(float damage)				   { this->baseStat.damage -= damage; };
			inline void RemoveBaseDamageMultipyier	(float multiplier)			   { this->baseStat.meleeMultiplier -= multiplier; };
			inline void RemoveBasePhysicalResistance(float resistance)			   { this->baseStat.physicalResistance -= resistance; };

			inline void RemoveBaseRangedDamage		(float damage)				   { this->baseStat.rangedDamage -= damage; };
			inline void RemoveBaseRangedMulitplier	(float multiplier)			   { this->baseStat.rangedMultiplier -= multiplier; };
			inline void RemoveBaseMovement			(float speed)				   { this->baseStat.movementSpeed -= speed; };

			inline void RemoveBaseMagicDamage		(float damage)				   { this->baseStat.magicDamage -= damage; };
			inline void RemoveBaseMagicMulitplier	(float multiplier)			   { this->baseStat.magicMultiplier -= multiplier; };
			inline void RemoveBaseMagicResistance	(float resistance)			   { this->baseStat.magicResistance -= resistance; };
			inline void RemoveBaseNatureResistance	(float resistance)			   { this->baseStat.natureResistance -= resistance; };
			inline void RemoveBaseFireResistance	(float resistance)			   { this->baseStat.fireResistance -= resistance; };
			inline void RemoveBaseWaterResistance	(float resistance)			   { this->baseStat.waterResistance -= resistance; };
			inline void SetHealth(float amount) { health = amount; };

			virtual float GetMaxHealth() { return health; };


			/**
			* @brief To be documented
			*/
			void AddForce(float force[2]);
			void AddForce(float x, float y);
			

			inline bool UnitHealthAboveZero() { return health > 0.f; };


			inline std::shared_ptr<GameUnit*> GetSharedPtr() const{ return mySelf; }  ;
			//Transforms and the like will be created inside the EnemyFactory, and outside of this class. During the sprint, this will most likely be done in the playstate

			inline Stats& GetNewStat() { return newStat; };

		protected:
			Stats baseStat;
			Stats newStat;

			Core::Entity unitEntity = {};

			Utilz::GUID deathAnimation = "";
			bool deathAnimationRunning = false;

			//Life. Float needed?
			float health;
			float xPos;
			float yPos;
			float zPos;
			float extents;

			float stunDuration;

			float force[2];

			void DiminishForce(float dt);

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
