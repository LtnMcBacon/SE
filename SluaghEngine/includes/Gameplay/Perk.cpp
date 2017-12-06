#include "Perk.h"
#include <Gameplay\PlayerUnit.h>
using namespace SE;
using namespace Gameplay;
	
SE::Gameplay::Perk::Perk()
{

	myCondition = perkConditions(intToEnum);
}

SE::Gameplay::Perk::~Perk()
{
}

bool SE::Gameplay::Perk::checkConditions(std::vector<ProjectileData>& newProjectiles, PlayerUnit* player)
{
	bool cond = false;
	switch (myCondition)
	{
	case perkConditions::NONE:
		cond = true;
		break;
	case perkConditions::PHYSICAL_TAKEN:
	{
		std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::PHYSICAL)
			{
				cond = true;
				break;
			}
		}
		break;
	}
	case perkConditions::WATER_TAKEN:
	{
		std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::WATER)
			{
				cond = true;
				break;
			}
		}
		break;
	}

	case perkConditions::NATURE_TAKEN:
	{
		std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::NATURE)
			{
				cond = true;
				break;
			}
		}
		break;
	}
	case perkConditions::MAGIC_TAKEN:
	{
		std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::MAGIC)
			{
				cond = true;
				break;
			}
		}
		break;
	}
	case perkConditions::FIRE_TAKEN:
	{
		std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::FIRE)
			{
				cond = true;
				break;
			}
		}
		break;
	}
	case perkConditions::PHYSICAL_GIVEN:
	{
		for (auto& projectile : newProjectiles)
		{
			if (projectile.eventDamage.type == DamageType::PHYSICAL)
			{
				cond = true;
				break;
			}
		}
		break;
	}
	case perkConditions::WATER_GIVEN:
		for (auto& projectile : newProjectiles)
		{
			if (projectile.eventDamage.type == DamageType::WATER)
			{
				cond = true;
				break;
			}
		}
		break;
	case perkConditions::NATURE_GIVEN:
		for (auto& projectile : newProjectiles)
		{
			if (projectile.eventDamage.type == DamageType::NATURE)
			{
				cond = true;
				break;
			}
		}
		break;
	case perkConditions::MAGIC_GIVEN:
		for (auto& projectile : newProjectiles)
		{
			if (projectile.eventDamage.type == DamageType::MAGIC)
			{
				cond = true;
				break;
			}
		}
		break;
	case perkConditions::FIRE_GIVEN:
		for (auto& projectile : newProjectiles)
		{
			if (projectile.eventDamage.type == DamageType::FIRE)
			{
				cond = true;
				break;
			}
		}
		break;
	default:
		break;
	}
	return cond;
}
