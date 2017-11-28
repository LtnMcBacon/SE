#include <Gameplay\PerkFactory.h>
using namespace SE;
using namespace Gameplay;
PerkFaktory::PerkFaktory()
{
}

PerkFaktory::~PerkFaktory()
{
}

void PerkFaktory::initiateFuncs(int value,int funcEnum, bool switchPerk)
{

	if (switchPerk)
	{
		perkFuncVector.push_back(perkFunctions);
		perkFunctions.clear();
	}
	
	int placeHolder2 = 0;
	float placeHolder3 = 0;


	switch (funcEnum)
	{
	case 0:

		// not implemented
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	case 9:
		break;
	case 10:
		break;
	case 11:
		break;
	case 12:
		break;
	case 13:
		break;
	case 14:
		break;
	case 15:
		break;
	case 16:
		break;
	case 17:
		break;
	case 18:
		break;
	case 19:
		break;
	case 20:
		break;
	case 21:
		break;
	case 22:
		break;
	case 23:
		break;
	case 24:
		break;
	case 25:
		break;
	case 26:
		break;
	case 27:
		break;
	case 28:
		break;
	case 29:
		break;
	case 30:
		break;
	case 31:
		break;
	case 32:
		break;
	case 33:
		break;
	case 34:
		break;
	case 35:
		break;
	case 36:
		break;
	case 37:
		break;
	case 38:
		break;
	case 39:
		break;
	case 40:
		break;
	case 41:
		break;
	case 42:
		break;
	case 43:
		break;
	case 44:
		break;
	case 45:
		break;
	case 46:
		break;
	case 47:
		break;
	case 48:
		break;

	default:
		break;
	}

	auto PhysicalResistance = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

			for (auto& Event : DmgVector)
			{
				if (Event.type == DamageType::PHYSICAL)
				{
					int reduced = Event.amount *(value / 100);
					Event.amount -= reduced;
				}
			}
		}
		else
		{
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::PHYSICAL)
					{
						int reduced = Event.amount *(value / 100);
						Event.amount -= reduced;
					}
				}
			}
			

		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> PhysicalResistanceFunc = PhysicalResistance;
	perkFunctions.push_back(PhysicalResistanceFunc);
	perkMapping.insert({ 1,PhysicalResistanceFunc});


	auto WaterResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

			for (auto& Event : DmgVector)
			{
				if (Event.type == DamageType::WATER)
				{
					int reduced = Event.amount *(value / 100);
					Event.amount -= reduced;
				}
			}
		}
		else
		{
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::WATER)
					{
						int reduced = Event.amount *(value / 100);
						Event.amount -= reduced;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> WaterResistanceFunc = WaterResistance;
	perkFunctions.push_back(WaterResistanceFunc);
	perkMapping.insert({ 2,WaterResistanceFunc});


	auto FireResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
			for (auto& Event: DmgVector)
			{
				if (Event.type == DamageType::FIRE)
				{
					int reduced = Event.amount *(value / 100);
					Event.amount -= reduced;
				}
			}
		}
		else
		{
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::FIRE)
					{
						int reduced = Event.amount *(value / 100);
						Event.amount -= reduced;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> FireResistanceFunc = FireResistance;
	perkFunctions.push_back(FireResistanceFunc);
	perkMapping.insert({ 3,FireResistanceFunc });


	auto NatureResistance = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

			for (auto& Event : DmgVector)
			{
				if (Event.type == DamageType::NATURE)
				{
					int reduced = Event.amount *(value / 100);
					Event.amount -= reduced;
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::NATURE)
					{
						int reduced = Event.amount *(value / 100);
						Event.amount -= reduced;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> NatureResistanceFunc = NatureResistance;
	perkFunctions.push_back(NatureResistanceFunc);
	perkMapping.insert({ 4,NatureResistanceFunc});


	auto MagicResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

			for (auto& Event : DmgVector)
			{
				if (Event.type == DamageType::MAGIC)
				{
					int reduced = Event.amount *(value / 100);
					Event.amount -= reduced;
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::MAGIC)
					{
						int reduced = Event.amount *(value / 100);
						Event.amount -= reduced;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MagicResistanceFunc = MagicResistance;
	perkFunctions.push_back(MagicResistanceFunc);
	perkMapping.insert({ 5,MagicResistanceFunc });



	auto RangedResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
			for (auto& Event : DmgVector)
			{
				if (Event.type == DamageType::RANGED)
				{
					int reduced = Event.amount *(value / 100);
					Event.amount -= reduced;
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::RANGED)
					{
						int reduced = Event.amount *(value / 100);
						Event.amount -= reduced;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> RangedResistanceFunc = RangedResistance;
	perkFunctions.push_back(RangedResistanceFunc);
	perkMapping.insert({ 6,RangedResistanceFunc});


	auto StrengthFlat = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			player->AddNewStrength(value);
		}
		else
		{
			if (placeHolder3 >0)
			{
				placeHolder3 -= deltaTime;
				player->AddNewStrength(value);
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> StrengthFlatFunc = StrengthFlat;
	perkFunctions.push_back(StrengthFlatFunc);
	perkMapping.insert({ 7,StrengthFlatFunc });


	auto AgilityFlat = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			player->AddNewAgility(value);
		}
		else
		{
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				player->AddNewAgility(value);
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> AgilityFlatFunc = AgilityFlat;
	perkFunctions.push_back(AgilityFlatFunc);
	perkMapping.insert({ 8,AgilityFlatFunc });


	auto IntelligenceFlat = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			player->AddNewWhisdom(value);
		}
		else
		{
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				player->AddNewWhisdom(value);
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> IntelligenceFlatFunc = IntelligenceFlat;
	perkFunctions.push_back(IntelligenceFlatFunc);
	perkMapping.insert({ 9,IntelligenceFlatFunc });


	auto HealthFlat = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			player->AddNewMaxHealth(value);
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				player->AddNewMaxHealth(value);
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> HealthFlatFunc = HealthFlat;
	perkFunctions.push_back(HealthFlatFunc);
	perkMapping.insert({ 10,HealthFlatFunc });


	auto intMultiplier = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			if (!intMult)
			{
				int wis = player->GetBaseWhisdom();
				int newWis = wis*(value / 100);
				player->SetBaseWhisdom(newWis);
				intMult = true;
			}
		}
		
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> intMultiplierFunc = intMultiplier;
	perkFunctions.push_back(intMultiplierFunc);
	perkMapping.insert({ 11,intMultiplierFunc });


	auto strMultiplier = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			if (!strMult)
			{
				int str = player->GetBaseStrength();
				int newStr = str*(value / 100);
				player->SetBaseStrength(newStr);
				strMult = true;
			}
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> strMultiplierFunc = strMultiplier;
	perkFunctions.push_back(strMultiplierFunc);
	perkMapping.insert({ 12,strMultiplierFunc });


	auto agiMultiplier = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			if (!agiMult)
			{
				int agi = player->GetBaseAgility();
				int newAgi = agi*(value / 100);
				player->SetBaseAgility(newAgi);
				agiMult = true;
			}
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> agiMultiplierFunc = agiMultiplier;
	perkFunctions.push_back(agiMultiplierFunc);
	perkMapping.insert({ 13,agiMultiplierFunc });


	auto HealthMulti = [value, this](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)->void
	{
		if (condition)
		{

			if (!HealthMultiP)
			{
				int maxHP = player->GetBaseMaxHealth();
				int newMaxHP = maxHP * (value / 100);
				player->AddBaseMaxHealth(value);
				HealthMultiP = true;
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> HealthMultiFunc = HealthMulti;
	perkFunctions.push_back(HealthMultiFunc);
	perkMapping.insert({ 14,HealthMultiFunc });


	auto DamageMulti = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			float meleeDmg = player->GetBaseMeleeDamageMultiplier();
			float rangedDmg = player->GetBaseRangedMulitplier();
			float magicDmg = player->GetBaseMagicMulitplier();

			float newMelee = meleeDmg * (value / 100);
			float newRanged = rangedDmg * (value / 100);
			float newMagic = magicDmg * (value / 100);

			player->AddNewMeleeDamageMultiplier(newMelee);
			player->AddNewRangedMulitplier(newRanged);
			player->AddNewMagicMulitplier(newMagic);

		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				float meleeDmg = player->GetBaseMeleeDamageMultiplier();
				float rangedDmg = player->GetBaseRangedMulitplier();
				float magicDmg = player->GetBaseMagicMulitplier();

				float newMelee = meleeDmg * (value / 100);
				float newRanged = rangedDmg * (value / 100);
				float newMagic = magicDmg * (value / 100);

				player->AddNewMeleeDamageMultiplier(newMelee);
				player->AddNewRangedMulitplier(newRanged);
				player->AddNewMagicMulitplier(newMagic);
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> DamageMultiFunc = DamageMulti;
	perkFunctions.push_back(DamageMultiFunc);
	perkMapping.insert({ 15,DamageMultiFunc });



	auto MeleeDamage = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			for (auto& projectile : Projs)
			{
				if (projectile.eventDamage.source == DamageSources::DAMAGE_SOURCE_MELEE)
				{
					int reduced = projectile.eventDamage.amount *(value / 100);
					projectile.eventDamage.amount += reduced;
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				for (auto& projectile : Projs)
				{
					if (projectile.eventDamage.source == DamageSources::DAMAGE_SOURCE_MELEE)
					{
						int reduced = projectile.eventDamage.amount *(value / 100);
						projectile.eventDamage.amount += reduced;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MeleeDamageFunc = MeleeDamage;
	perkFunctions.push_back(MeleeDamageFunc);
	perkMapping.insert({ 17,MeleeDamageFunc});


	auto RangeDamage = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			for (auto& projectile : Projs)
			{
				if (projectile.eventDamage.source == DamageSources::DAMAGE_SOURCE_RANGED)
				{
					int reduced = projectile.eventDamage.amount *(value / 100);
					projectile.eventDamage.amount += reduced;
				}
			}
		}
		else
		{
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				for (auto& projectile : Projs)
				{
					if (projectile.eventDamage.source == DamageSources::DAMAGE_SOURCE_RANGED)
					{
						int reduced = projectile.eventDamage.amount *(value / 100);
						projectile.eventDamage.amount += reduced;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> RangeDamageFunc = RangeDamage;
	perkFunctions.push_back(RangeDamageFunc);
	perkMapping.insert({ 18,RangeDamageFunc});

	

	auto AttackSpeed = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			float AS = player->GetBaseAttackSpeed();
			float newAS = AS * (value / 100);
			player->AddNewAttackSpeed(newAS);
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				float AS = player->GetBaseAttackSpeed();
				float newAS = AS * (value / 100);
				player->AddNewAttackSpeed(newAS);
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> AttackSpeedFunc = AttackSpeed;
	perkFunctions.push_back(AttackSpeedFunc);
	perkMapping.insert({ 19,AttackSpeedFunc });

	
	auto ConsecutiveDmG = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
	{
		if (condition)
		{

			placeHolder3 = duration;

			placeHolder2 += value;

			float rangedDMG = player->GetBaseRangedMulitplier();
			float meleeDMG = player->GetBaseMeleeDamageMultiplier();
			float magicDMG = player->GetBaseMagicMulitplier();

			float newRangedDMG = rangedDMG + (placeHolder2 / 100);
			float newMeleeDMG = meleeDMG + (placeHolder2 / 100);
			float newMagicDMG = magicDMG + (placeHolder2 / 100);

			player->AddNewRangedMulitplier(newRangedDMG);
			player->AddNewMeleeDamageMultiplier(newMeleeDMG);
			player->AddNewMagicMulitplier(newMagicDMG);

		}
		else
		{
			
			if(placeHolder3 >0)
			{
				placeHolder3 -= deltaTime;
				float rangedDMG = player->GetBaseRangedMulitplier();
				float meleeDMG = player->GetBaseMeleeDamageMultiplier();
				float magicDMG = player->GetBaseMagicMulitplier();

				float newRangedDMG = rangedDMG + (placeHolder2 / 100);
				float newMeleeDMG = meleeDMG + (placeHolder2 / 100);
				float newMagicDMG = magicDMG + (placeHolder2 / 100);

				player->AddNewRangedMulitplier(newRangedDMG);
				player->AddNewMeleeDamageMultiplier(newMeleeDMG);
				player->AddNewMagicMulitplier(newMagicDMG);
			}
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveDmGFunc = ConsecutiveDmG;
	perkFunctions.push_back(ConsecutiveDmGFunc);
	perkMapping.insert({ 21,ConsecutiveDmGFunc });


	auto ConsecutiveAttackSpeed = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;

			placeHolder2 += value;
			float AS = player->GetBaseAttackSpeed();
			float newAS = AS * (placeHolder2 / 100);
			player->AddNewAttackSpeed(newAS);

		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				float AS = player->GetBaseAttackSpeed();
				float newAS = AS * (placeHolder2 / 100);
				player->AddNewAttackCooldown(newAS);
			}
			
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveAttackSpeedFunc = ConsecutiveAttackSpeed;
	perkFunctions.push_back(ConsecutiveAttackSpeedFunc);
	perkMapping.insert({ 22,ConsecutiveAttackSpeedFunc });


	auto ConsecutiveMeleeDmg = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;

			placeHolder2 += value;
			float AD = player->GetBaseMeleeDamageMultiplier();
			float newAD = AD + (placeHolder2 / 100);
			player->AddNewMeleeDamageMultiplier(newAD);

		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				float AD = player->GetBaseMeleeDamageMultiplier();
				float newAD = AD * (placeHolder2 / 100);
				player->AddNewMeleeDamageMultiplier(newAD);
			}
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveMeleeDmgFunc = ConsecutiveMeleeDmg;
	perkFunctions.push_back(ConsecutiveMeleeDmgFunc);
	perkMapping.insert({ 23,ConsecutiveMeleeDmgFunc });


	auto ConsecutiveRangedDmg = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;

			placeHolder2 += value;
			float AD = player->GetBaseRangedMulitplier();
			float newAD = AD + (placeHolder2 / 100);
			player->AddNewRangedMulitplier(newAD);
		}
		else
		{
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				float AD = player->GetBaseRangedMulitplier();
				float newAD = AD * (placeHolder2 / 100);
				player->AddNewRangedMulitplier(newAD);
			}
			
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveRangedDmgFunc = ConsecutiveRangedDmg;
	perkFunctions.push_back(ConsecutiveRangedDmgFunc);
	perkMapping.insert({ 24,ConsecutiveRangedDmgFunc });




	auto skillCooldown = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			if (!skillCD)
			{
				int currentCooldown = player->GetCooldown(0);
				int currentCooldown2 = player->GetCooldown(1);

				int newCD = currentCooldown * (value / 100);
				int newCD2 = currentCooldown2 * (value / 100);

				player->SetCooldown(0, newCD);
				player->SetCooldown(1, newCD2);
				skillCD = true;
			}
		}
		
		
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> skillCooldownFunc = skillCooldown;
	perkFunctions.push_back(skillCooldownFunc);
	perkMapping.insert({ 28,skillCooldownFunc });

	auto AdaptiveSkillCD = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			
			float currentCD = player->GetCurrentCooldown(0);
			float currentCD2 = player->GetCurrentCooldown(1);
		
			float newCD = currentCD - value;
			float newCD2 = currentCD2 - value;
			if (newCD < 0)
			{
				newCD = 0;
			}
			if (newCD2 < 0)
			{
				newCD2 = 0;
			}
			player->SetCurrentCooldown(0, newCD);
			player->SetCurrentCooldown(1, newCD2);
		}
	

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> AdaptiveSkillCDFunc = AdaptiveSkillCD;
	perkFunctions.push_back(AdaptiveSkillCDFunc);
	perkMapping.insert({ 29,AdaptiveSkillCDFunc });


	auto SkillDmg = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			
			if (!skillDamage)
			{
				float skillDmg1 = player->GetSkillDamage(0);
				float skillDmg2 = player->GetSkillDamage(1);

				float newSkillDmg1 = skillDmg1 *  (value / 100);
				float newSkillDmg2 = skillDmg2 *  (value / 100);
		
				player->AddSkillDamage(0, newSkillDmg1);
				player->AddSkillDamage(1, newSkillDmg2);
				
				skillDamage = true;
			}
		}
		

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> SkillDmgFunc = SkillDmg;
	perkFunctions.push_back(SkillDmgFunc);
	perkMapping.insert({ 30,SkillDmgFunc });


	auto MovementSpeed = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			float MS = player->GetBaseMovement();

			placeHolder2 = MS * (value / 100);

			player->AddNewMovement(placeHolder2);

		}
		else
		{
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;

				float MS = player->GetBaseMovement();

				placeHolder2 = MS * (value / 100);

				player->AddNewMovement(placeHolder2);
			}
			
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MovementSpeedFunc = MovementSpeed;
	perkFunctions.push_back(MovementSpeedFunc);
	perkMapping.insert({ 31,MovementSpeedFunc });


	auto maxHpPercentHeal = [value](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)->void
	{
		if (condition)
		{

			float MaxHP = player->GetNewMaxHealth();
			float Healing = MaxHP * (value / 100);
			HealingEvent Heal;
			Heal.originalAmount = Healing;
			player->AddHealingEvent(Heal);
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> maxHpPercentHealFunc = maxHpPercentHeal;
	perkFunctions.push_back(maxHpPercentHealFunc);
	perkMapping.insert({ 32,maxHpPercentHealFunc });

	auto flatHPHeal = [value](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)->void
	{
		if (condition)
		{

			HealingEvent Heal;
			Heal.originalAmount = value;
			player->AddHealingEvent(Heal);
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> flatHPHealFunc = flatHPHeal;
	perkFunctions.push_back(flatHPHealFunc);
	perkMapping.insert({ 33,flatHPHealFunc });

	auto HealImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			player->ClearHealingEvents();	
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				player->ClearHealingEvents();
			}
		}
		

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> HealImmuneFunc = HealImmune;
	perkFunctions.push_back(HealImmuneFunc);
	perkMapping.insert({ 34,HealImmuneFunc });

	auto PhysicalImmune = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
		
			for (int i = 0; i < DmgVector.size(); i++)
			{
				if (DmgVector[i].type == DamageType::PHYSICAL)
				{
					DmgVector.erase(DmgVector.begin()+i);
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (int i = 0; i < DmgVector.size(); i++)
				{
					if (DmgVector[i].type == DamageType::PHYSICAL)
					{
						DmgVector.erase(DmgVector.begin() + i);
					}
				}
			}
		}


	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> PhysicalImmuneFunc = PhysicalImmune;
	perkFunctions.push_back(PhysicalImmuneFunc);
	perkMapping.insert({ 35,PhysicalImmuneFunc });


	auto WaterImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

			for (int i = 0; i < DmgVector.size(); i++)
			{
				if (DmgVector[i].type == DamageType::WATER)
				{
					DmgVector.erase(DmgVector.begin()+i);
					i = 0;
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (int i = 0; i < DmgVector.size(); i++)
				{
					if (DmgVector[i].type == DamageType::WATER)
					{
						DmgVector.erase(DmgVector.begin() + i);
						i = 0;
					}
				}
			}
		}


	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> WaterImmuneFunc = WaterImmune;
	perkFunctions.push_back(WaterImmuneFunc);
	perkMapping.insert({ 36,WaterImmuneFunc });


	auto FireImmune = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

			for (int i = 0; i < DmgVector.size(); i++)
			{
				if (DmgVector[i].type == DamageType::FIRE)
				{
					DmgVector.erase(DmgVector.begin()+i);
					i = 0;
				}
			}
		}
		else
		{
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (int i = 0; i < DmgVector.size(); i++)
				{
					if (DmgVector[i].type == DamageType::FIRE)
					{
						DmgVector.erase(DmgVector.begin() + i);
						i = 0;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> FireImmuneFunc = FireImmune;
	perkFunctions.push_back(FireImmuneFunc);
	perkMapping.insert({ 37,FireImmuneFunc });


	auto NatureImmune = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

			for (int i = 0; i < DmgVector.size(); i++)
			{
				if (DmgVector[i].type == DamageType::NATURE)
				{
					DmgVector.erase(DmgVector.begin() + i);
					i = 0;
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (int i = 0; i < DmgVector.size(); i++)
				{
					if (DmgVector[i].type == DamageType::NATURE)
					{
						DmgVector.erase(DmgVector.begin() + i);
						i = 0;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> NatureImmuneFunc = NatureImmune;
	perkFunctions.push_back(NatureImmuneFunc);
	perkMapping.insert({ 38,NatureImmuneFunc });

	auto SlowImmune = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<ConditionEvent> CndVector = player->GetConditionEvents();


			for (int i = 0; i < CndVector.size(); i++)
			{
				if (CndVector[i].bane == Banes::CONDITIONAL_BANES_SLOW)
				{
					CndVector.erase(CndVector.begin() + i);
					i = 0;
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<ConditionEvent> CndVector = player->GetConditionEvents();


				for (int i = 0; i < CndVector.size(); i++)
				{
					if (CndVector[i].bane == Banes::CONDITIONAL_BANES_SLOW)
					{
						CndVector.erase(CndVector.begin() + i);
						i = 0;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> SlowImmuneFunc = SlowImmune;
	perkFunctions.push_back(SlowImmuneFunc);
	perkMapping.insert({ 39,SlowImmuneFunc });


	auto StunImmune = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<ConditionEvent> CndVector = player->GetConditionEvents();


			for (int i = 0; i < CndVector.size(); i++)
			{
				if (CndVector[i].bane == Banes::CONDITIONAL_BANES_STUN)
				{
					CndVector.erase(CndVector.begin() + i);
					i = 0;
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<ConditionEvent> CndVector = player->GetConditionEvents();


				for (int i = 0; i < CndVector.size(); i++)
				{
					if (CndVector[i].bane == Banes::CONDITIONAL_BANES_STUN)
					{
						CndVector.erase(CndVector.begin() + i);
						i = 0;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> StunImmuneFunc = StunImmune;
	perkFunctions.push_back(StunImmuneFunc);
	perkMapping.insert({ 40,StunImmuneFunc });


	auto MagicImmune = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

			for (int i = 0; i < DmgVector.size(); i++)
			{
				if (DmgVector[i].type == DamageType::MAGIC)
				{
					DmgVector.erase(DmgVector.begin() + i);
					i = 0;
				}
			}
		}
		else
		{
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (int i = 0; i < DmgVector.size(); i++)
				{
					if (DmgVector[i].type == DamageType::MAGIC)
					{
						DmgVector.erase(DmgVector.begin() + i);
						i = 0;
					}
				}
			}
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MagicImmuneFunc = MagicImmune;
	perkFunctions.push_back(MagicImmuneFunc);
	perkMapping.insert({ 41,MagicImmuneFunc });


	auto MeleeLock = [value,this,placeHolder2,placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{

			placeHolder3 = duration;
			meleeLock = true;

		}
		else
		{

			if (placeHolder3<=0)
			{
				meleeLock = false;

			}
			else
			{
				placeHolder3 -= deltaTime;
			}
			
			
		}
	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MeleeLockFunc = MeleeLock;
	perkFunctions.push_back(MeleeLockFunc);
	perkMapping.insert({ 43,MeleeLockFunc });


	auto RangeLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			rangeLock = true;
		}
		else
		{
			if (placeHolder3 <= 0)
			{
				rangeLock = false;
			}
			else
			{
				placeHolder3 -= deltaTime;
			}
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> RangeLockFunc = RangeLock;
	perkFunctions.push_back(RangeLockFunc);
	perkMapping.insert({ 44,RangeLockFunc });


	auto MagicLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = 5;
			magicLock = true;
		}
		else
		{
			if (placeHolder3 <= 0)
			{
				magicLock = false;
			}
			else
			{
				placeHolder3 -= deltaTime;

			}
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MagicLockFunc = MagicLock;
	perkFunctions.push_back(MagicLockFunc);
	perkMapping.insert({ 45,MagicLockFunc });


	auto WaterLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			waterLock = true;
		}
		else
		{
			if (placeHolder3 <= 0)
			{
				waterLock = false;
			}
			else
			{
				placeHolder3 -= deltaTime;

			}
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> WaterLockFunc = WaterLock;
	perkFunctions.push_back(WaterLockFunc);
	perkMapping.insert({ 46,WaterLockFunc });


	auto FireLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			fireLock = true;
		}
		else
		{
			if (placeHolder3 <= 0)
			{
				fireLock = false;
			}
			else
			{
				placeHolder3 -= deltaTime;

			}
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> FireLockFunc = FireLock;
	perkFunctions.push_back(FireLockFunc);
	perkMapping.insert({ 47,FireLockFunc });


	auto NatureLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
	{
		if (condition)
		{
			placeHolder3 = duration;
			natureLock = true;
		}
		else
		{
			if (placeHolder3 <= 0)
			{
				natureLock = false;
			}
			else
			{
				placeHolder3 -= deltaTime;
			}
		}

	}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> NatureLockFunc = NatureLock;
	perkFunctions.push_back(NatureLockFunc);
	perkMapping.insert({ 48,NatureLockFunc });



	



}

void PerkFaktory::initateMap()
{
	//parameters a;
	//PlayerUnit* play;
	//a.player = play;
	



}
