#ifndef _SE_GAMEPLAY_EVENT_STRUCTS_H_
#define _SE_GAMEPLAY_EVENT_STRUCTS_H_

#include <DirectXMath.h>
#include <type_traits>

#define ENUM_FLAG_OPERATOR(T,X) inline T operator X (T lhs, T rhs) { return (T) (static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#define ENUM_FLAG_OPERATOR2(T,X) inline void operator X= (T& lhs, T rhs) { lhs = (T)(static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#define ENUM_FLAGS(T) \
enum class T; \
inline T operator ~ (T t) { return (T) (~static_cast<std::underlying_type_t <T>>(t)); } \
inline bool operator & (T lhs, T rhs) { return (static_cast<std::underlying_type_t <T>>(lhs) & static_cast<std::underlying_type_t <T>>(rhs));  } \
ENUM_FLAG_OPERATOR2(T,|) \
ENUM_FLAG_OPERATOR2(T,&) \
ENUM_FLAG_OPERATOR(T,|) \
ENUM_FLAG_OPERATOR(T,^)

namespace SE
{
	namespace Gameplay
	{
		enum class Boons
		{
			CONDITIONAL_BOONS_NONE					= 1 << 0, 
			CONDITIONAL_BOONS_DAMAGE				= 1 << 1,
			CONDITIONAL_BOONS_KNOCKBACK				= 1 << 2,
			CONDITIONAL_BOONS_STUN					= 1 << 3,
			CONDITIONAL_BOONS_ROOT					= 1 << 4,
			CONDITIONAL_BOONS_PROTECTION			= 1 << 5,
			CONDITIONAL_BOONS_PHYSICAL_RESISTANCE	= 1 << 6,
			CONDITIONAL_BOONS_MAGICAL_RESISTANCE	= 1 << 7,
			CONDITIONAL_BOONS_FIRE_RESISTANCE		= 1 << 8,
			CONDITIONAL_BOONS_WATER_RESISTANCE		= 1 << 9,
			CONDITIONAL_BOONS_NATURE_RESISTANCE		= 1 << 10,
			CONDITIONAL_BOONS_CASTSPEED				= 1 << 11,
			CONDITIONAL_BOONS_SWIFTNESS				= 1 << 12,
			CONDITIONAL_BOONS_SLOW					= 1 << 13,
			CONDITIONAL_BOONS_INVULNERABILITY		= 1 << 14
		};

		enum class Banes
		{
			CONDITIONAL_BANES_NONE				= 1 << 0,
			CONDITIONAL_BANES_DAMAGE			= 1 << 1,
			CONDITIONAL_BANES_STUN				= 1 << 2,
			CONDITIONAL_BANES_ROOT				= 1 << 3,
			CONDITIONAL_BANES_BLOODLETTING		= 1 << 4,
			CONDITIONAL_BANES_UNCOVER			= 1 << 5,
			CONDITIONAL_BANES_PHYSICAL_WEAKNESS = 1 << 6,
			CONDITIONAL_BANES_MAGICAL_WEAKNESS	= 1 << 7,
			CONDITIONAL_BANES_FIRE_WEAKNESS		= 1 << 8,
			CONDITIONAL_BANES_WATER_WEAKNESS	= 1 << 9,
			CONDITIONAL_BANES_NATURE_WEAKNESS	= 1 << 10,
			CONDITIONAL_BANES_SLOW				= 1 << 11
		};

		union Condition
		{
			Boons boon;
			Banes bane;
		};

		enum class DamageSources
		{
			DAMAGE_SOURCE_SELFCAST,
			DAMAGE_SOURCE_MELEE,
			DAMAGE_SOURCE_RANGED,
			DAMAGE_SOURCE_AREA
		};

		enum class DamageType
		{
			PHYSICAL,			
			FIRE,
			WATER,
			NATURE,
			RANGED,
			MAGIC,
			NUM_TYPES
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
			float amount = 0;
			float originalAmount = 0;
			DamageSources source;
			DamageType type;

			DamageEvent()
			{
				source = DamageSources::DAMAGE_SOURCE_MELEE;
				type = DamageType::PHYSICAL;
				amount = 1;
				originalAmount = amount;
			}

			DamageEvent(DamageSources damageSource, DamageType damageType, float damageAmount)
			{
				source = damageSource;
				type = damageType;
				originalAmount = amount = damageAmount;
			}
		};

		struct HealingEvent
		{
			/*What is needed?*/
			float amount = 0;
			float originalAmount = 0;
			enum class SourceType
			{
				SOURCE_TYPE_ENEMY_DEATH,
				SOURCE_TYPE_ENEMY_HIT
			}type;

			HealingEvent(SourceType sType = SourceType::SOURCE_TYPE_ENEMY_HIT, float amountHealing = 0)
			{
				type = sType;
				amount = originalAmount = amountHealing;
			}

		};

		struct ConditionEvent
		{
			struct ConditionType
			{
				int unionType = 0;
				Condition condition;
			};

			bool isBane = 0;
			Boons boon;
			Banes bane;
			float effectValue = 0;
			float duration = 0;
			DirectX::XMFLOAT3 dirVec;

			ConditionEvent() 
			{

			}
			ConditionEvent(ConditionType conditionType, float effectValue, float d = 0.f)
			{
				if (conditionType.unionType == 0)
				{
					isBane = false;
					boon = conditionType.condition.boon;
				}
				if (conditionType.unionType == 1)
				{
					isBane = true;
					bane = conditionType.condition.bane;
				}
				
				this->effectValue = effectValue;
				duration = d;
				dirVec.x = 0;
				dirVec.y = 0;
				dirVec.z = 0;
			}
			ConditionEvent(ConditionType conditionType, DirectX::XMFLOAT3 dirVec, float effectValue, float d = 0.f)
			{
				if (conditionType.unionType == 0)
				{
					isBane = false;
					boon = conditionType.condition.boon;
				}
				if (conditionType.unionType == 1)
				{
					isBane = true;
					bane = conditionType.condition.bane;
				}

				this->effectValue = effectValue;
				duration = d;
				this->dirVec = dirVec;
			}
		};
	}
}

ENUM_FLAGS(SE::Gameplay::Boons);
ENUM_FLAGS(SE::Gameplay::Banes);

#endif
