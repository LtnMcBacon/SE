#include <Gameplay\PerkFactory.h>
using namespace SE;
using namespace Gameplay;
PerkFaktory::PerkFaktory()
{
}

PerkFaktory::~PerkFaktory()
{
}

void PerkFaktory::initiateFuncs()
{
	int placeHolder = 0;

	auto FireResistance = [placeHolder](std::vector<parameters> player)->void
	{
		std::vector<DamageEvent> DmgVector = player[0].player->GetDamageEvents();
		for (auto& Event: DmgVector)
		{
			if (Event.type == DamageType::FIRE)
			{
				int reduced = Event.amount *(placeHolder / 100);
				Event.amount -= reduced;
			}
		}
	}; std::function<void(std::vector<parameters> player)> FireResistanceFunc = FireResistance;
	perkFunctions.push_back(FireResistanceFunc);


	auto WaterResistance = [placeHolder](std::vector<parameters> player)->void
	{
		std::vector<DamageEvent> DmgVector = player[0].player->GetDamageEvents();

		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::WATER)
			{
				int reduced = Event.amount *(placeHolder / 100);
				Event.amount -= reduced;
			}
		}
	}; std::function<void(std::vector<parameters> player)> WaterResistanceFunc = WaterResistance;
	perkFunctions.push_back(WaterResistanceFunc);


	auto MagicResistance = [placeHolder](std::vector<parameters> player)->void
	{
		std::vector<DamageEvent> DmgVector = player[0].player->GetDamageEvents();

		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::MAGIC)
			{
				int reduced = Event.amount *(placeHolder / 100);
				Event.amount -= reduced;
			}
		}
	}; std::function<void(std::vector<parameters> player)> MagicResistanceFunc = MagicResistance;
	perkFunctions.push_back(MagicResistanceFunc);


	auto NatureResistance = [placeHolder](std::vector<parameters> player)->void
	{
		std::vector<DamageEvent> DmgVector = player[0].player->GetDamageEvents();

		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::NATURE)
			{
				int reduced = Event.amount *(placeHolder / 100);
				Event.amount -= reduced;
			}
		}
	}; std::function<void(std::vector<parameters> player)> NatureResistanceFunc = NatureResistance;
	perkFunctions.push_back(NatureResistanceFunc);


	auto PhysicalResistance = [placeHolder](std::vector<parameters> player)->void
	{
		std::vector<DamageEvent> DmgVector = player[0].player->GetDamageEvents();

		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::PHYSICAL)
			{
				int reduced = Event.amount *(placeHolder / 100);
				Event.amount -= reduced;
			}
		}
	}; std::function<void(std::vector<parameters> player)> PhysicalResistanceFunc = PhysicalResistance;
	perkFunctions.push_back(PhysicalResistanceFunc);

	auto RangedResistance = [placeHolder](std::vector<parameters> player)->void
	{
		std::vector<DamageEvent> DmgVector = player[0].player->GetDamageEvents();
		for (auto& Event : DmgVector)
		{
			if (Event.type == DamageType::RANGED)
			{
				int reduced = Event.amount *(placeHolder / 100);
				Event.amount -= reduced;
			}
		}
	}; std::function<void(std::vector<parameters> player)> RangedResistanceFunc = RangedResistance;
	perkFunctions.push_back(RangedResistanceFunc);


	auto MeleeDamage = [placeHolder](std::vector<parameters> player)->void
	{
		for (auto& projectile : (*(player[1].Projectiles)))
		{
			if (projectile.eventDamage.source == DamageSources::DAMAGE_SOURCE_MELEE)
			{
				int reduced = projectile.eventDamage.amount *(placeHolder / 100);
				projectile.eventDamage.amount -= reduced;
			}
		}
	}; std::function<void(std::vector<parameters> player)> MeleeDamageFunc = MeleeDamage;
	perkFunctions.push_back(MeleeDamageFunc);


	auto RangeDamage = [placeHolder](std::vector<parameters> player)->void
	{
		for (auto& projectile : (*(player[1].Projectiles)))
		{
			if (projectile.eventDamage.source == DamageSources::DAMAGE_SOURCE_RANGED)
			{
				int reduced = projectile.eventDamage.amount *(placeHolder / 100);
				projectile.eventDamage.amount -= reduced;
			}
		}
	}; std::function<void(std::vector<parameters> player)> RangeDamageFunc = RangeDamage;
	perkFunctions.push_back(RangeDamageFunc);


	

	

}

void PerkFaktory::initateMap()
{
	parameters a;
	PlayerUnit* play;
	a.player = play;
	



}
