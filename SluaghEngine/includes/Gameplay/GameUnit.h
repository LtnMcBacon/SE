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
			/*
			* @brief	Clears the new stats struct
			*
			* @details	resets all the values in newStat
			*
			* @warning Dont call this if you have valuable information stored in the struct
			*/
			void ClearNewStats();

			inline float GetHealth() const { return health; };
			inline float GetXPosition() { return xPos; };
			inline float GetYPosition() { return yPos; };
			inline float GetZPosition() { return zPos; };
			inline Utilz::GUID GetDeathAnimation() { return deathAnimation; };

			inline void SetXPosition(float value) { xPos = value; };
			inline void SetYPosition(float value) { yPos = value; };
			inline void SetZPosition(float value) { zPos = value; };

			inline void SetDeathAnimation(Utilz::GUID deathAnim) { deathAnimation = deathAnim; };

			//Base Stat Getters
			inline int GetBaseStrength			  (){ return this->baseStat.str; };
			inline int GetBaseAgility			  (){ return this->baseStat.agi; };
			inline int GetBaseWhisdom			  (){ return this->baseStat.whi; };
			inline int GetBaseArmourCap			  (){ return this->baseStat.armorCap; };

			inline float GetBaseMaxHealth		  (){ return this->baseStat.health; };
			inline float GetBaseDamage			  (){ return this->baseStat.damage; };
			inline float GetBaseMeleeDamageMultiplier  (){ return this->baseStat.meleeMultiplier; };
			inline float GetBasePhysicalResistance(){ return this->baseStat.damage; };
	  
			inline float GetBaseRangedDamage	  (){ return this->baseStat.rangedDamage; };
			inline float GetBaseRangedMulitplier  (){ return this->baseStat.rangedMultiplier; };
			inline float GetBaseMovement		  (){ return this->baseStat.movementSpeed; };
			inline float GetBaseAttackSpeed	      (){ return this->baseStat.attackSpeed; };

			inline float GetBaseMagicDamage		  (){ return this->baseStat.magicDamage; };
			inline float GetBaseMagicMulitplier	  (){ return this->baseStat.magicMultiplier; };
			inline float GetBaseMagicResistance	  (){ return this->baseStat.magicResistance; };
			inline float GetBaseNatureResistance  (){ return this->baseStat.natureResistance; };
			inline float GetBaseFireResistance	  (){ return this->baseStat.fireResistance; };
			inline float GetBaseWaterResistance	  (){ return this->baseStat.waterResistance; };

			inline float GetBaseAttackCooldown        (){ return this->baseStat.attackCooldown; };
			inline ArmourType GetBaseArmour			  (){ return this->baseStat.armour; };
			inline DamageSources GetBaseWeapon		  (){ return this->baseStat.weapon; };
			inline DamageType GetBaseDamageType		  (){ return this->baseStat.damageType; };

			inline float getBaseConsAttackMult() { return this->baseStat.ConsecutiveattackSpeedMultiplier; };
			inline float getBaseAttackMult() { return this->baseStat.attackSpeedMultiplier; };

			//Base Stat Setters
			inline void SetBaseStrength			 (int str)						{ this->baseStat.str = str;};
			inline void SetBaseAgility			 (int agi)						{ this->baseStat.agi = agi; };
			inline void SetBaseWhisdom			 (int whi)						{ this->baseStat.whi = whi; };
			inline void SetBaseArmourCap		 (int cap)						{ this->baseStat.armorCap = cap; };

			inline void SetBaseMaxHealth		 (float health)					{ this->baseStat.health = health; };
			inline void SetBaseDamage			 (float damage)					{ this->baseStat.damage = damage; };
			inline void SetBaseMeleeDamageMultiplier  (float multiplier)				{ this->baseStat.meleeMultiplier = multiplier; };
			inline void SetBasePhysicalResistance(float resistance)				{ this->baseStat.physicalResistance = resistance; };

			inline void SetBaseRangedDamage		 (float damage)					{ this->baseStat.rangedDamage = damage; };
			inline void SetBaseRangedMulitplier  (float multiplier)				{ this->baseStat.rangedMultiplier = multiplier; };
			inline void SetBaseMovement			 (float speed)					{ this->baseStat.movementSpeed = speed; };
			inline void SetBaseAttackSpeed		 (float speed)					{ this->baseStat.attackSpeed = speed; };

			inline void SetBaseMagicDamage		 (float damage)					{ this->baseStat.magicDamage = damage; };
			inline void SetBaseMagicMulitplier	 (float multiplier)				{ this->baseStat.magicMultiplier = multiplier; };
			inline void SetBaseMagicResistance	 (float resistance)				{ this->baseStat.magicResistance = resistance; };
			inline void SetBaseNatureResistance  (float resistance)				{ this->baseStat.natureResistance = resistance; };
			inline void SetBaseFireResistance	 (float resistance)				{ this->baseStat.fireResistance = resistance; };
			inline void SetBaseWaterResistance	 (float resistance)				{ this->baseStat.waterResistance = resistance; };

			inline void SetBaseAttackCooldown    (float cooldown)				{ this->baseStat.attackCooldown = cooldown; };
			inline void setBaseConsAttackMult	 (float attackSpeedMult)		{ this->baseStat.ConsecutiveattackSpeedMultiplier = attackSpeedMult; };
			inline void setBaseAttackMult		 (float attackSpeedMult)		{ this->baseStat.attackSpeedMultiplier = attackSpeedMult; };
			inline void	 SetBaseArmour			 (unsigned short int armour)	{ this->baseStat.armour = static_cast<ArmourType>(armour); };
			inline void  SetBaseWeapon			 (unsigned short int weapon)	{ this->baseStat.weapon = static_cast<DamageSources>(weapon); };
			inline void  SetDamageType			 (unsigned short int damageType){ this->baseStat.damageType = static_cast<DamageType>(damageType); };

			//Base Stat Adders
			inline void AddBaseStrength			 (int str)						{ this->baseStat.str += str;};
			inline void AddBaseAgility			 (int agi)						{ this->baseStat.agi += agi;};
			inline void AddBaseWhisdom			 (int whi)						{ this->baseStat.whi += whi;};
			inline void AddBaseArmourCap		 (int cap)						{ this->baseStat.armorCap += cap;};

			inline void AddBaseMaxHealth		 (float health)					{ this->baseStat.health += health; };
			inline void AddBaseDamage			 (float damage)					{ this->baseStat.damage += damage; };
			inline void AddBaseMeleeDamageMultiplier  (float multiplier)				{ this->baseStat.meleeMultiplier += multiplier; };
			inline void AddBasePhysicalResistance(float resistance)				{ this->baseStat.physicalResistance += resistance; };

			inline void AddBaseRangedDamage		 (float damage)					{ this->baseStat.rangedDamage += damage; };
			inline void AddBaseRangedMulitplier  (float multiplier)				{ this->baseStat.rangedMultiplier += multiplier; };
			inline void AddBaseMovement			 (float speed)					{ this->baseStat.movementSpeed += speed; };
			inline void AddBaseAttackSpeed		 (float speed)					{ this->baseStat.attackSpeed += speed; };

			inline void AddBaseMagicDamage		 (float damage)					{ this->baseStat.magicDamage += damage; };
			inline void AddBaseMagicMulitplier	 (float multiplier)				{ this->baseStat.magicMultiplier += multiplier; };
			inline void AddBaseMagicResistance	 (float resistance)				{ this->baseStat.magicResistance += resistance; };
			inline void AddBaseNatureResistance  (float resistance)				{ this->baseStat.natureResistance += resistance; };
			inline void AddBaseFireResistance	 (float resistance)				{ this->baseStat.fireResistance += resistance; };
			inline void AddBaseWaterResistance	 (float resistance)				{ this->baseStat.waterResistance += resistance; };

			inline void AddBaseAttackCooldown	 (float cooldown)				{ this->baseStat.attackCooldown += cooldown; };
			inline void AddBaseConsAttackMult	 (float attackSpeedMulti)		{ this->baseStat.ConsecutiveattackSpeedMultiplier += attackSpeedMulti; };
			inline void AddBaseAttackMult		 (float attackSpeedMulti)		{ this->baseStat.attackSpeedMultiplier += attackSpeedMulti; };

			//Base Stat Removers
			inline void RemoveBaseStrength			(int str)					   { this->baseStat.str -= str;};
			inline void RemoveBaseAgility			(int agi)					   { this->baseStat.agi -= agi;};
			inline void RemoveBaseWhisdom			(int whi)					   { this->baseStat.whi -= whi;};
			inline void RemoveBaseArmourCap			(int cap)					   { this->baseStat.armorCap -= cap;};

			inline void RemoveBaseMaxHealth			(float health)				   { this->baseStat.health -= health; };
			inline void RemoveBaseDamage			(float damage)				   { this->baseStat.damage -= damage; };
			inline void RemoveBaseMeleeDamageMultiplier	(float multiplier)			   { this->baseStat.meleeMultiplier -= multiplier; };
			inline void RemoveBasePhysicalResistance(float resistance)			   { this->baseStat.physicalResistance -= resistance; };

			inline void RemoveBaseRangedDamage		(float damage)				   { this->baseStat.rangedDamage -= damage; };
			inline void RemoveBaseRangedMulitplier	(float multiplier)			   { this->baseStat.rangedMultiplier -= multiplier; };
			inline void RemoveBaseMovement			(float speed)				   { this->baseStat.movementSpeed -= speed; };
			inline void RemoveBaseAttackSpeed		(float speed)				   { this->baseStat.attackSpeed -= speed; };
			inline void RemoveBaseConsAttackMult	(float attackSpeedMulti)	   { this->baseStat.ConsecutiveattackSpeedMultiplier -= attackSpeedMulti; };
			inline void RemoveBaseAttackMult		(float attackSpeedMulti)	   { this->baseStat.attackSpeedMultiplier -= attackSpeedMulti; };

			inline void RemoveBaseMagicDamage		(float damage)				   { this->baseStat.magicDamage -= damage; };
			inline void RemoveBaseMagicMulitplier	(float multiplier)			   { this->baseStat.magicMultiplier -= multiplier; };
			inline void RemoveBaseMagicResistance	(float resistance)			   { this->baseStat.magicResistance -= resistance; };
			inline void RemoveBaseNatureResistance	(float resistance)			   { this->baseStat.natureResistance -= resistance; };
			inline void RemoveBaseFireResistance	(float resistance)			   { this->baseStat.fireResistance -= resistance; };
			inline void RemoveBaseWaterResistance	(float resistance)			   { this->baseStat.waterResistance -= resistance; };


			inline void SetHealth(float amount) { health = amount; };

			virtual float GetMaxHealth() { return health; };


			inline void RemoveBaseAttackCooldown	(float cooldown)			   { this->baseStat.attackCooldown -= cooldown; };

			//New Stat Getters
			inline int GetNewStrength			 () { return this->newStat.str; };
			inline int GetNewAgility			 () { return this->newStat.agi; };
			inline int GetNewWhisdom			 () { return this->newStat.whi; };
			inline int GetNewArmourCap			 () { return this->newStat.armorCap; };

			inline float GetNewMaxHealth		 () { return this->newStat.health; };
			inline float GetNewDamage			 () { return this->newStat.damage; };
			inline float GetNewMeleeDamageMultiplier	 () { return this->newStat.meleeMultiplier; };
			inline float GetNewPhysicalResistance() { return this->newStat.damage; };

			inline float GetNewRangedDamage		 () { return this->newStat.rangedDamage; };
			inline float GetNewRangedMulitplier	 () { return this->newStat.rangedMultiplier; };
			inline float GetNewMovement			 () { return this->newStat.movementSpeed; };
			inline float GetNewAttackSpeed		 () { return this->newStat.attackSpeed; };

			inline float GetNewMagicDamage		 () { return this->newStat.magicDamage; };
			inline float GetNewMagicMulitplier	 () { return this->newStat.magicMultiplier; };
			inline float GetNewMagicResistance	 () { return this->newStat.magicResistance; };
			inline float GetNewNatureResistance	 () { return this->newStat.natureResistance; };
			inline float GetNewFireResistance	 () { return this->newStat.fireResistance; };
			inline float GetNewWaterResistance	 () { return this->newStat.waterResistance; };

			inline float GetNewAttackCooldown	() { return newStat.attackCooldown; };
			inline ArmourType GetNewArmour		() { return newStat.armour; };
			inline DamageSources GetNewWeapon	() { return newStat.weapon; };
			inline DamageType GetNewDamageType	() { return newStat.damageType; };

			//New Stat Setters
			inline void SetNewStrength			(int str)				{ this->newStat.str = str; };
			inline void SetNewAgility			(int agi)				{ this->newStat.agi = agi; };
			inline void SetNewWhisdom			(int whi)				{ this->newStat.whi = whi; };
			inline void SetNewArmourCap			(int cap)				{ this->newStat.armorCap = cap; };

			inline void SetNewMaxHealth			(float health)			{ this->newStat.health = health; };
			inline void SetNewDamage			(float damage)			{ this->newStat.damage = damage; };
			inline void SetNewMeleeDamageMultiplier	(float multiplier)	{ this->newStat.meleeMultiplier = multiplier; };
			inline void SetNewPhysicalResistance(float resistance)		{ this->newStat.physicalResistance = resistance; };

			inline void SetNewRangedDamage		(float damage)			{ this->newStat.rangedDamage = damage; };
			inline void SetNewRangedMulitplier	(float multiplier)		{ this->newStat.rangedMultiplier = multiplier; };
			inline void SetNewMovement			(float speed)			{ this->newStat.movementSpeed = speed; };
			inline void SetNewAttackSpeed		(float speed)			{ this->newStat.attackSpeed = speed; };

			inline void SetNewMagicDamage		(float damage)			{ this->newStat.magicDamage = damage; };
			inline void SetNewMagicMulitplier	(float multiplier)		{ this->newStat.magicMultiplier = multiplier; };
			inline void SetNewMagicResistance	(float resistance)		{ this->newStat.magicResistance = resistance; };
			inline void SetNewNatureResistance	(float resistance)		{ this->newStat.natureResistance = resistance; };
			inline void SetNewFireResistance	(float resistance)		{ this->newStat.fireResistance = resistance; };
			inline void SetNewWaterResistance	(float resistance)		{ this->newStat.waterResistance = resistance; };

			inline void SetNewAttackCooldown	(float cooldown)		{ newStat.attackCooldown = cooldown; };

			inline void	 SetNewArmour(unsigned short int armour)		{ this->newStat.armour = static_cast<ArmourType>(armour); };
			inline void  SetNewWeapon(unsigned short int weapon)		{ this->newStat.weapon = static_cast<DamageSources>(weapon); };
			inline void  SetNewDamageType(unsigned short int damageType){ this->newStat.damageType = static_cast<DamageType>(damageType);};

			//New Stat Adders
			inline void AddNewStrength			(int str)			{ this->newStat.str += str; };
			inline void AddNewAgility			(int agi)			{ this->newStat.agi += agi; };
			inline void AddNewWhisdom			(int whi)			{ this->newStat.whi += whi; };
			inline void AddNewArmourCap			(int cap)			{ this->newStat.armorCap += cap; };

			inline void AddNewMaxHealth			(float health)		{ this->newStat.health += health; };
			inline void AddNewDamage			(float damage)		{ this->newStat.damage += damage; };
			inline void AddNewMeleeDamageMultiplier	(float multiplier)	{ this->newStat.meleeMultiplier += multiplier; };
			inline void AddNewPhysicalResistance(float resistance)	{ this->newStat.physicalResistance += resistance; };

			inline void AddNewRangedDamage		(float damage)		{ this->newStat.rangedDamage += damage; };
			inline void AddNewRangedMulitplier	(float multiplier)	{ this->newStat.rangedMultiplier += multiplier; };
			inline void AddNewMovement			(float speed)		{ this->newStat.movementSpeed += speed; };
			inline void AddNewAttackSpeed			(float speed)		{ this->newStat.attackSpeed += speed; };

			inline void AddNewMagicDamage		(float damage)		{ this->newStat.magicDamage += damage; };
			inline void AddNewMagicMulitplier	(float multiplier)	{ this->newStat.magicMultiplier += multiplier; };
			inline void AddNewMagicResistance	(float resistance)	{ this->newStat.magicResistance += resistance; };
			inline void AddNewNatureResistance	(float resistance)	{ this->newStat.natureResistance += resistance; };
			inline void AddNewFireResistance	(float resistance)	{ this->newStat.fireResistance += resistance; };
			inline void AddNewWaterResistance	(float resistance)	{ this->newStat.waterResistance += resistance; };

			inline void AddNewAttackCooldown	(float cooldown)	{ this->newStat.attackCooldown += cooldown; };

			//New Stat Removers
			inline void RemoveNewStrength		   (int str)			{ this->newStat.str -= str; };
			inline void RemoveNewAgility		   (int agi)			{ this->newStat.agi -= agi; };
			inline void RemoveNewWhisdom		   (int whi)			{ this->newStat.whi -= whi; };
			inline void RemoveNewArmourCap		   (int cap)			{ this->newStat.armorCap -= cap; };

			inline void RemoveNewMaxHealth		   (float health)		{ this->newStat.health -= health; };
			inline void RemoveNewDamage			   (float damage)		{ this->newStat.damage -= damage; };
			inline void RemoveNewMeleeDamageMultiplier  (float multiplier)	{ this->newStat.meleeMultiplier -= multiplier; };
			inline void RemoveNewPhysicalResistance(float resistance)	{ this->newStat.physicalResistance -= resistance; };

			inline void RemoveNewRangedDamage	   (float damage)		{ this->newStat.rangedDamage -= damage; };
			inline void RemoveNewRangedMulitplier  (float multiplier)	{ this->newStat.rangedMultiplier -= multiplier; };
			inline void RemoveNewMovement		   (float speed)		{ this->newStat.movementSpeed -= speed; };
			inline void RemoveAttackSpeed		   (float speed)		{ this->newStat.attackSpeed -= speed; };

			inline void RemoveNewMagicDamage	   (float damage)		{ this->newStat.magicDamage -= damage; };
			inline void RemoveNewMagicMulitplier   (float multiplier)	{ this->newStat.magicMultiplier -= multiplier; };
			inline void RemoveNewMagicResistance   (float resistance)	{ this->newStat.magicResistance -= resistance; };
			inline void RemoveNewNatureResistance  (float resistance)	{ this->newStat.natureResistance -= resistance; };
			inline void RemoveNewFireResistance	   (float resistance)	{ this->newStat.fireResistance -= resistance; };
			inline void RemoveNewWaterResistance   (float resistance)	{ this->newStat.waterResistance -= resistance; };

			inline void RemoveNewAttackCooldown	   (float cooldown)		{ this->newStat.attackCooldown -= cooldown; };

			/**
			* @brief	Used to calculate the new strength stat changes caused by attribute changes.
			* @details	Calculates stats caused by attribute changes. Does not however calculate changes caused
			*			by weapon types, perks, skills or elements.
			**/
			void calcBaseStrChanges();
			/**
			* @brief	Used to calculate the new agility stat changes caused by attribute changes.
			* @details	Calculates stats caused by attribute changes. Does not however calculate changes caused
			*			by weapon types, perks, skills or elements.
			**/
			void calcBaseAgiChanges();
			/**
			* @brief	Used to calculate the new whisdom stat changes caused by attribute changes.
			* @details	Calculates stats caused by attribute changes. Does not however calculate changes caused
			*			by weapon types, perks, skills or elements.
			**/
			void calcBaseWhiChanges();

			/**
			* @brief	Used to calculate the new strength stat changes caused by attribute changes.
			* @details	Calculates stats caused by attribute changes. Does not however calculate changes caused
			*			by weapon types, perks, skills or elements.
			**/
			void calcNewStrChanges();
			/**
			* @brief	Used to calculate the new agility stat changes caused by attribute changes.
			* @details	Calculates stats caused by attribute changes. Does not however calculate changes caused
			*			by weapon types, perks, skills or elements.
			**/
			void calcNewAgiChanges();
			/**
			* @brief	Used to calculate the new whisdom stat changes caused by attribute changes.
			* @details	Calculates stats caused by attribute changes. Does not however calculate changes caused
			*			by weapon types, perks, skills or elements.
			**/
			void calcNewWhiChanges();

			void calcNewConsecutiveAttackSpeed();
			void calcNewAttackSpeed();
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
			Stats baseStat; //perm
			Stats newStat;  //temp

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
			inline const void SetEntity(Core::Entity ent)
			{
				unitEntity = ent;
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
