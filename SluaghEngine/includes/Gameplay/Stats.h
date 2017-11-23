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
			int agi = 5;
			int whi = 5;
			int armorCap = 3;

			//str
			float health = 100.f;
			float damage = 1.f;
			float meleeMultiplier = 1.f;
			float physicalResistance = 1.f;

			//agi
			float rangedDamage = 1.f;
			float rangedMultiplier = 1.f;
			float movementSpeed = 5.f;
			//float healBonus			= 1.f;
			//float attackSpeed		= 1.f;

			//whi
			float magicDamage = 1.f;
			float magicMultiplier = 1.f;
			float magicResistance = 1.f;
			float natureResistance = 1.f;
			float fireResistance = 1.f;
			float waterResistance = 1.f;

			ArmourType armour = ArmourType::ARMOUR_TYPE_NONE;
			DamageSources weapon = DamageSources::DAMAGE_SOURCE_MELEE;
			DamageType damageType = DamageType::PHYSICAL;
		};

	}
}

#endif