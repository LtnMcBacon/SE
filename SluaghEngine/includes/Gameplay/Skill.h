#ifndef _SE_GAMEPLAY_SKILL_H
#define _SE_GAMEPLAY_SKILL_H

#include <Utilz\GUID.h>
#include <Gameplay\EventStructs.h>

namespace SE
{
	namespace Gameplay
	{
		struct Skill
		{
			std::string skillName = "";
			std::string skillDesc = "";
			DamageSources atkType = DamageSources::DAMAGE_SOURCE_MELEE;
			DamageType damageType = DamageType::PHYSICAL;
			Boons boon = Boons::CONDITIONAL_BOONS_NONE;
			Banes bane = Banes::CONDITIONAL_BANES_NONE;
			unsigned short int animation = 0;
			unsigned short int particle = 0;

			Utilz::GUID projectileFileGUID;
			float skillDamage = 0.f;
			float boonEffectValue = 0.f;
			float boonRange = 0.f;
			float boonDuration = 0.f;
			float baneEffectValue = 0.f;
			float baneRange = 0.f;
			float baneDuration = 0.f;
			float cooldown = 0.f;
			float currentCooldown = 0.f;
		};
	}
}
#endif