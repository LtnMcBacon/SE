#ifndef _SE_GAMEPLAY_EVENT_STRUCTS_H_
#define _SE_GAMEPLAY_EVENT_STRUCTS_H_

namespace SE
{
	namespace Gameplay
	{
		/*Not documented due to not being decided yet!*/
		struct DamageEvent
		{
			/*Should these enum classes be put in a seperate header?*/
			enum class DamageSources
			{
				DAMAGE_SOURCE_MELEE,
				DAMAGE_SOURCE_RANGED,
				DAMAGE_SOURCE_MAGICAL
			} source;

			enum class DamageTypes
			{
				DAMAGE_TYPE_PHYSICAL,
				DAMAGE_TYPE_MAGICAL,
				DAMAGE_TYPE_FIRE,
				DAMAGE_TYPE_WATER,
				DAMAGE_TYPE_NATURE
			} type;

			/*Int/Float for amount?*/
			float amount;
			float originalAmount;

			DamageEvent()
			{
				source = DamageSources::DAMAGE_SOURCE_MELEE;
				type = DamageTypes::DAMAGE_TYPE_PHYSICAL;
				amount = 1;
				originalAmount = amount;
			}

			DamageEvent(DamageSources damageSource, DamageTypes damageType, float damageAmount)
			{
				source = damageSource;
				type = damageType;
				originalAmount = amount = damageAmount;
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
