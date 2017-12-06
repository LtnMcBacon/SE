#ifndef SE_GAMEPLAY_STATS_H_
#define SE_GAMEPLAY_STATS_H_
#include "EventStructs.h"

namespace SE
{
	namespace Gameplay
	{

		struct Stats
		{
			//std::string characterName;
			int str = 5;
			static const int MAX_STR = 10;
			int agi = 5;
			static const int MAX_AGI = 10;
			int whi = 5;
			static const int MAX_WHI = 10;
			int armorCap = 3;

			//str
			float health = 100.f;
			static const int MAX_HEALTH = 100;
			float damage = 1.f;
			static const int MAX_DAMAGE = 15;
			float meleeMultiplier = 1.f;
			float physicalResistance = 0.f;

			//agi
			float rangedDamage = 1.f;
			float rangedMultiplier = 1.f;
			float movementSpeed = 5.f;
			float attackSpeed		= 5.f;
			float attackCooldown = 0.f;
			//float healBonus			= 1.f;

			//whi
			float magicDamage = 1.f;
			float magicMultiplier = 1.f;
			float magicResistance = 0.f;
			float natureResistance = 0.f;
			float fireResistance = 0.f;
			float waterResistance = 0.f;

			ArmourType armour = ArmourType::ARMOUR_TYPE_NONE;
			DamageSources weapon = DamageSources::DAMAGE_SOURCE_MELEE;
			DamageType damageType = DamageType::PHYSICAL;


			inline static DamageType GetRandomDamageType()
			{
				return DamageType(std::rand() % int(DamageType::NUM_TYPES));
			}
			inline static int GetRandStr()
			{
				return std::rand() % (MAX_STR + 1) - MAX_STR;
			}
			inline static int GetRandAgi()
			{
				return std::rand() % (MAX_AGI + 1) - MAX_AGI;
			}
			inline static int GetRandWil()
			{
				return std::rand() % (MAX_WHI + 1) - MAX_WHI;
			}
			inline static int GetRandHealth()
			{
				return std::rand() % (MAX_HEALTH) + 1;
			}
			inline static int GetRandDamage()
			{
				return std::rand() % (MAX_DAMAGE) + 1;
			}
		};
	}
}

#endif