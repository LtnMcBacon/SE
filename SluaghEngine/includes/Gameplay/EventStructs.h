#ifndef _SE_GAMEPLAY_EVENT_STRUCTS_H_
#define _SE_GAMEPLAY_EVENT_STRUCTS_H_

namespace SE
{
	namespace Gameplay
	{

		enum class Boons
		{
			CONDITIONAL_BOONS_NONE, 
			CONDITIONAL_BOONS_DAMAGE,
			CONDITIONAL_BOONS_KNOCKBACK,
			CONDITIONAL_BOONS_STUN,
			CONDITIONAL_BOONS_ROOT,
			CONDITIONAL_BOONS_PROTECTION,
			CONDITIONAL_BOONS_PHYSICAL_RESISTANCE,
			CONDITIONAL_BOONS_MAGICAL_RESISTANCE,
			CONDITIONAL_BOONS_FIRE_RESISTANCE,
			CONDITIONAL_BOONS_WATER_RESISTANCE,
			CONDITIONAL_BOONS_NATURE_RESISTANCE,
			CONDITIONAL_BOONS_CASTSPEED,
			CONDITIONAL_BOONS_SWIFTNESS,
			CONDITIONAL_BOONS_SLOW,
			CONDITIONAL_BOONS_INVULNERABILITY
		};

		enum class Banes
		{
			CONDITIONAL_BANES_NONE,
			CONDITIONAL_BANES_DAMAGE,
			CONDITIONAL_BANES_STUN,
			CONDITIONAL_BANES_ROOT,
			CONDITIONAL_BANES_BLOODLETTING,
			CONDITIONAL_BANES_UNCOVER,
			CONDITIONAL_BANES_PHYSICAL_WEAKNESS,
			CONDITIONAL_BANES_MAGICAL_WEAKNESS,
			CONDITIONAL_BANES_FIRE_WEAKNESS,
			CONDITIONAL_BANES_WATER_WEAKNESS,
			CONDITIONAL_BANES_NATURE_WEAKNESS,
			CONDITIONAL_BANES_SLOW
		};

		enum class DamageSources
		{
			DAMAGE_SOURCE_SELFCAST,
			DAMAGE_SOURCE_MELEE,
			DAMAGE_SOURCE_RANGED,
			DAMAGE_SOURCE_AREA
		};

		enum class DamageTypes
		{
			DAMAGE_TYPE_PHYSICAL,
			DAMAGE_TYPE_MAGICAL,
			DAMAGE_TYPE_FIRE,
			DAMAGE_TYPE_WATER,
			DAMAGE_TYPE_NATURE
		};

		/**
		* @breif	A class used to make scripting easier when changing the players armor type.
		* @details	The class should be used for readablitity when switching items in the inventory.
		*			The class does not inherit from other classes.
		* @sa		*Coming* inventory.
		**/
		enum class ArmourType 
		{ 
			ARMOUR_TYPE_LIGHT,
			ARMOUR_TYPE_MEDIUM,
			ARMOUR_TYPE_HEAVY,
			ARMOUR_TYPE_NONE
		};

		/*Not documented due to not being decided yet!*/
		struct DamageEvent
		{
			/*Should these enum classes be put in a seperate header?*/

			/*Int/Float for amount?*/
			int amount;

			DamageEvent()
			{
				source = DamageSources::DAMAGE_SOURCE_MELEE;
				type = DamageTypes::DAMAGE_TYPE_PHYSICAL;
				amount = 1;
			}

			DamageEvent(DamageSources damageSource, DamageTypes damageType, int damageAmount)
			{
				source = damageSource;
				type = damageType;
				amount = damageAmount;
			}
		};

		struct HealingEvent
		{
			/*What is needed?*/
			float amount;
			enum class SourceType
			{
				SOURCE_TYPE_ENEMY_DEATH,
				SOURCE_TYPE_ENEMY_HIT
			}type;
		};

		struct ConditionEvent
		{
			enum class ConditionTypes
			{
				CONDITION_TYPE_NONE,
				CONDITION_TYPE_STUN,
				CONDITION_TYPE_ROOT,
				CONDITION_TYPE_HEALTH_SET /*How the hell do we fix this?*/
			} type;
			float duration;
			ConditionEvent(ConditionTypes conditionType = ConditionTypes::CONDITION_TYPE_NONE, float d = 0.f)
			{
				type = conditionType;
				duration = d;
			}
		};

	}
}



#endif
