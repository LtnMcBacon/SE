#include <Gameplay\PerkFactory.h>
#include <Gameplay\PlayerUnit.h>
#include "CoreInit.h"
using namespace SE;
using namespace Gameplay;
PerkFaktory::PerkFaktory()
{
}

PerkFaktory::~PerkFaktory()
{
}

std::function<void(SE::Gameplay::PlayerUnit* player, std::vector<SE::Gameplay::ProjectileData>& newProjectiles, float dt, bool condition)> PerkFaktory::initiateFuncs(int value, int funcEnum)
{
	float placeHolder2 = 0;
	float placeHolder3 = 0;


	switch (funcEnum)
	{
	case 0:

		CoreInit::subSystems.devConsole->Print("Faulty perk Error! NR: 0");
		break;
	case 1:
	{
		auto PhysicalResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
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


			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> PhysicalResistanceFunc = PhysicalResistance;
		return PhysicalResistanceFunc;
		break;
	}
		
	case 2:
	{
		auto WaterResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::WATER)
					{
						float reduced = Event.amount *(value / 100.f);
						Event.amount -= reduced;
					}
				}
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> WaterResistanceFunc = WaterResistance;
		return WaterResistanceFunc;
		break;
	}
	case 3:
	{
		auto FireResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::FIRE)
					{
						float reduced = Event.amount *(value / 100.f);
						Event.amount -= reduced;
					}
				}
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> FireResistanceFunc = FireResistance;
		return FireResistanceFunc;
		break;
	}
	case 4:
	{
		auto NatureResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::NATURE)
					{
						float reduced = Event.amount *(value / 100.f);
						Event.amount -= reduced;
					}
				}
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> NatureResistanceFunc = NatureResistance;
		return NatureResistanceFunc;
		break;
	}
	case 5:
	{
		auto MagicResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::MAGIC)
					{
						float reduced = Event.amount *(value / 100.f);
						Event.amount -= reduced;
					}
				}
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MagicResistanceFunc = MagicResistance;
		return MagicResistanceFunc;
		break;
	}
	case 6:
	{
		auto RangedResistance = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();
				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::RANGED)
					{
						float reduced = Event.amount *(value / 100.f);
						Event.amount -= reduced;
					}
				}
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> RangedResistanceFunc = RangedResistance;
		return RangedResistanceFunc;
		break;
	}
	case 7:
	{

		auto StrengthFlat = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			if (placeHolder3 >0)
			{
				placeHolder3 -= deltaTime;
				player->AddNewStrength(value);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> StrengthFlatFunc = StrengthFlat;
		return StrengthFlatFunc;
		break;
	}
	case 8:
	{

		auto AgilityFlat = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				player->AddNewAgility(value);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> AgilityFlatFunc = AgilityFlat;
		return AgilityFlatFunc;
		break;
	}
	case 9:
	{

		auto IntelligenceFlat = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
		
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				player->AddNewWhisdom(value);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> IntelligenceFlatFunc = IntelligenceFlat;
		return IntelligenceFlatFunc;
		break;
	}
	case 10:
	{

		auto HealthFlat = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				player->AddNewMaxHealth(value);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> HealthFlatFunc = HealthFlat;
		return HealthFlatFunc;
		break;
	}
	case 11:
	{

		auto intMultiplier = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				if (!intMult)
				{
					int wis = player->GetBaseWhisdom();
					float newWis = wis*(value / 100.f);
					player->SetBaseWhisdom(newWis);
					intMult = true;
				}
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> intMultiplierFunc = intMultiplier;
		return intMultiplierFunc;
		break;
	}
	case 12:
	{

		auto strMultiplier = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				if (!strMult)
				{
					int str = player->GetBaseStrength();
					float newStr = str*(value / 100.f);
					player->SetBaseStrength(newStr);
					strMult = true;
				}
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> strMultiplierFunc = strMultiplier;
		return strMultiplierFunc;
		break;
	}
	case 13:
	{

		auto agiMultiplier = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				if (!agiMult)
				{
					int agi = player->GetBaseAgility();
					float newAgi = agi*(value / 100.f);
					player->SetBaseAgility(newAgi);
					agiMult = true;
				}
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> agiMultiplierFunc = agiMultiplier;
		return agiMultiplierFunc;
		break;
	}
	case 14:
	{

		auto HealthMulti = [value, this](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)->void
		{
			if (condition)
			{

				if (!HealthMultiP)
				{
					int maxHP = player->GetBaseMaxHealth();
					float newMaxHP = maxHP * (value / 100.f);
					player->AddBaseMaxHealth(value);
					HealthMultiP = true;
				}
			}
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> HealthMultiFunc = HealthMulti;
		return HealthMultiFunc;
		break;
	}
	case 15:
	{

		auto DamageMulti = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;

			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				float meleeDmg = player->GetBaseMeleeDamageMultiplier();
				float rangedDmg = player->GetBaseRangedMulitplier();
				float magicDmg = player->GetBaseMagicMulitplier();

				float newMelee = meleeDmg * (value / 100.f);
				float newRanged = rangedDmg * (value / 100.f);
				float newMagic = magicDmg * (value / 100.f);

				player->AddNewMeleeDamageMultiplier(newMelee);
				player->AddNewRangedMulitplier(newRanged);
				player->AddNewMagicMulitplier(newMagic);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> DamageMultiFunc = DamageMulti;
		return DamageMultiFunc;
		break;
	}
	case 16:
	{

		CoreInit::subSystems.devConsole->Print("Faulty perk Error! NR: 16");
		break;
	}
	case 17:
	{

		auto MeleeDamage = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				for (auto& projectile : Projs)
				{
					if (projectile.eventDamage.source == DamageSources::DAMAGE_SOURCE_MELEE)
					{
						float reduced = projectile.eventDamage.amount *(value / 100.f);
						projectile.eventDamage.amount += reduced;
					}
				}
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MeleeDamageFunc = MeleeDamage;
		return MeleeDamageFunc;
		break;
	}
	case 18:
	{

		auto RangeDamage = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				for (auto& projectile : Projs)
				{
					if (projectile.eventDamage.source == DamageSources::DAMAGE_SOURCE_RANGED)
					{
						float reduced = projectile.eventDamage.amount *(value / 100.f);
						projectile.eventDamage.amount += reduced;
					}
				}
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> RangeDamageFunc = RangeDamage;
		return RangeDamageFunc;
		break;
	}
	case 19:
	{

		auto AttackSpeed = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				float AS = player->GetBaseAttackSpeed();
				float newAS = AS * (value / 100.f);
				player->AddNewAttackSpeed(newAS);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> AttackSpeedFunc = AttackSpeed;
		return AttackSpeedFunc;
		break;
	}
	case 20:
	{

		CoreInit::subSystems.devConsole->Print("Faulty perk Error! NR: 20");
		break;
	}
	case 21:
	{

		auto ConsecutiveDmG = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{

				placeHolder3 = duration;

				placeHolder2 += value;

				float rangedDMG = player->GetBaseRangedMulitplier();
				float meleeDMG = player->GetBaseMeleeDamageMultiplier();
				float magicDMG = player->GetBaseMagicMulitplier();

				float newRangedDMG = rangedDMG + (placeHolder2 / 100.f);
				float newMeleeDMG = meleeDMG + (placeHolder2 / 100.f);
				float newMagicDMG = magicDMG + (placeHolder2 / 100.f);

				player->AddNewRangedMulitplier(newRangedDMG);
				player->AddNewMeleeDamageMultiplier(newMeleeDMG);
				player->AddNewMagicMulitplier(newMagicDMG);

			}
			else
			{

				if (placeHolder3 >0)
				{
					placeHolder3 -= deltaTime;
					float rangedDMG = player->GetBaseRangedMulitplier();
					float meleeDMG = player->GetBaseMeleeDamageMultiplier();
					float magicDMG = player->GetBaseMagicMulitplier();

					float newRangedDMG = rangedDMG + (placeHolder2 / 100.f);
					float newMeleeDMG = meleeDMG + (placeHolder2 / 100.f);
					float newMagicDMG = magicDMG + (placeHolder2 / 100.f);

					player->AddNewRangedMulitplier(newRangedDMG);
					player->AddNewMeleeDamageMultiplier(newMeleeDMG);
					player->AddNewMagicMulitplier(newMagicDMG);
				}
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveDmGFunc = ConsecutiveDmG;
		return ConsecutiveDmGFunc;
		break;
	}
	case 22:
	{

		auto ConsecutiveAttackSpeed = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;

			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				float AS = player->GetBaseAttackSpeed();
				float newAS = AS * (placeHolder2 / 100.f);
				player->AddNewAttackCooldown(newAS);
			}

			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveAttackSpeedFunc = ConsecutiveAttackSpeed;
		return ConsecutiveAttackSpeedFunc;
		break;
	}
	case 23:
	{

		auto ConsecutiveMeleeDmg = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;

				placeHolder2 += value;
				float AD = player->GetBaseMeleeDamageMultiplier();
				float newAD = AD + (placeHolder2 / 100.f);
				player->AddNewMeleeDamageMultiplier(newAD);

			}
			else
			{
				if (placeHolder3>0)
				{
					placeHolder3 -= deltaTime;
					float AD = player->GetBaseMeleeDamageMultiplier();
					float newAD = AD * (placeHolder2 / 100.f);
					player->AddNewMeleeDamageMultiplier(newAD);
				}
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveMeleeDmgFunc = ConsecutiveMeleeDmg;
		return ConsecutiveMeleeDmgFunc;
		break;
	}
	case 24:
	{

		auto ConsecutiveRangedDmg = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;

				placeHolder2 += value;
				float AD = player->GetBaseRangedMulitplier();
				float newAD = AD + (placeHolder2 / 100.f);
				player->AddNewRangedMulitplier(newAD);
			}
			else
			{
				if (placeHolder3 > 0)
				{
					placeHolder3 -= deltaTime;
					float AD = player->GetBaseRangedMulitplier();
					float newAD = AD * (placeHolder2 / 100.f);
					player->AddNewRangedMulitplier(newAD);
				}
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveRangedDmgFunc = ConsecutiveRangedDmg;
		return ConsecutiveRangedDmgFunc;
		break;
	}
	case 25:
	{

		CoreInit::subSystems.devConsole->Print("Faulty perk Error! NR: 25");
		break;
	}
	case 26:
	{

		CoreInit::subSystems.devConsole->Print("Faulty perk Error! NR: 26");
		break;
	}
	case 27:
	{

		CoreInit::subSystems.devConsole->Print("Faulty perk Error! NR: 27");
		break;
	}
	case 28:
	{

		auto skillCooldown = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				if (!skillCD)
				{
					int currentCooldown = player->GetCooldown(0);
					int currentCooldown2 = player->GetCooldown(1);

					float newCD = currentCooldown * (value / 100.f);
					float newCD2 = currentCooldown2 * (value / 100.f);

					player->SetCooldown(0, newCD);
					player->SetCooldown(1, newCD2);
					skillCD = true;
				}
			}


		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> skillCooldownFunc = skillCooldown;
		return skillCooldownFunc;
		break;
	}
	case 29:
	{

		auto AdaptiveSkillCD = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
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
		return AdaptiveSkillCDFunc;
		break;
	}
	case 30:
	{

		auto SkillDmg = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{

				if (!skillDamage)
				{
					float skillDmg1 = player->GetSkillDamage(0);
					float skillDmg2 = player->GetSkillDamage(1);

					float newSkillDmg1 = skillDmg1 * (value / 100.f);
					float newSkillDmg2 = skillDmg2 * (value / 100.f);

					player->AddSkillDamage(0, newSkillDmg1);
					player->AddSkillDamage(1, newSkillDmg2);

					skillDamage = true;
				}
			}


		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> SkillDmgFunc = SkillDmg;
		return SkillDmgFunc;
		break;
	}
	case 31:
	{

		auto MovementSpeed = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;

			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;

				float MS = player->GetBaseMovement();

				placeHolder2 = MS * (value / 100.f);

				player->AddNewMovement(placeHolder2);
			}

			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MovementSpeedFunc = MovementSpeed;
		return MovementSpeedFunc;
		break;
	}
	case 32:
	{

		auto maxHpPercentHeal = [value](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)->void
		{
			if (condition)
			{

				float MaxHP = player->GetNewMaxHealth();
				float Healing = MaxHP * (value / 100.f);
				HealingEvent Heal;
				Heal.originalAmount = Healing;
				player->AddHealingEvent(Heal);
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> maxHpPercentHealFunc = maxHpPercentHeal;
		return maxHpPercentHealFunc;
		break;
	}
	case 33:
	{

		auto flatHPHeal = [value](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)->void
		{
			if (condition)
			{

				HealingEvent Heal;
				Heal.originalAmount = value;
				player->AddHealingEvent(Heal);
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> flatHPHealFunc = flatHPHeal;
		return flatHPHealFunc;
		break;
	}
	case 34:
	{

		auto HealImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
				
			}
			if (placeHolder3>0)
			{
				placeHolder3 -= deltaTime;
				player->ClearHealingEvents();
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> HealImmuneFunc = HealImmune;
		return HealImmuneFunc;
		break;
	}
	case 35:
	{

		auto PhysicalImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3 > 0)
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

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> PhysicalImmuneFunc = PhysicalImmune;
		return PhysicalImmuneFunc;
		break;
	}
	case 36:
	{

		auto WaterImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
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
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> WaterImmuneFunc = WaterImmune;
		return WaterImmuneFunc;
		break;
	}
	case 37:
	{

		auto FireImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
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
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> FireImmuneFunc = FireImmune;
		return FireImmuneFunc;
		break;
	}
	case 38:
	{

		auto NatureImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
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
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> NatureImmuneFunc = NatureImmune;
		return NatureImmuneFunc;
		break;
	}
	case 39:
	{

		auto SlowImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
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
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> SlowImmuneFunc = SlowImmune;
		return SlowImmuneFunc;
		break;
	}
	case 40:
	{

		auto StunImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
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
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> StunImmuneFunc = StunImmune;
		return StunImmuneFunc;
		break;
	}
	case 41:
	{

		auto MagicImmune = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
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
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MagicImmuneFunc = MagicImmune;
		return MagicImmuneFunc;
		break;
	}
	case 42:
	{

		CoreInit::subSystems.devConsole->Print("Faulty perk Error! NR: 42");
		break;
	}
	case 43:
	{

		auto MeleeLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				meleeLock = true;
			}
			else
			{
				meleeLock = false;
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MeleeLockFunc = MeleeLock;
		return MeleeLockFunc;
		break;
	}
	case 44:
	{

		auto RangeLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				rangeLock = true;
			}
			else
			{
				rangeLock = false;
			}
			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> RangeLockFunc = RangeLock;
		return RangeLockFunc;
		break;
	}
	case 45:
	{

		auto MagicLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;

			}
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				magicLock = true;
			}
			else
			{
				magicLock = false;
			}
			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> MagicLockFunc = MagicLock;
		return MagicLockFunc;
		break;
	}
	case 46:
	{

		auto WaterLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				waterLock = true;
			}
			else
			{
				waterLock = false;
			}
			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> WaterLockFunc = WaterLock;
		return WaterLockFunc;
		break;
	}
	case 47:
	{

		auto FireLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			if (placeHolder3 > 0)
			{
				placeHolder3 -= deltaTime;
				fireLock = true;
			}
			else
			{
				fireLock = false;
			}
			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> FireLockFunc = FireLock;
		return FireLockFunc;
		break;
	}
	case 48:
	{

		auto NatureLock = [value, this, placeHolder2, placeHolder3](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				placeHolder3 = duration;
			}
			else
			{
				if (placeHolder3 > 0)
				{
					placeHolder3 -= deltaTime;
					natureLock = true;
				}
				else
				{
					natureLock = false;
				}
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> NatureLockFunc = NatureLock;
		return NatureLockFunc;
		break;
	}

	default:
		CoreInit::subSystems.devConsole->Print("Faulty perk Error! NR: Out of bounds");
		break;
	}
}

void PerkFaktory::iteratePerks()
{
	
	for (int i = 0; i < PickedPerks.size(); i++)
	{
		for (int j = 0; j < PickedPerks[i].typeSize; j++)
		{
			perkBehavior.push_back(initiateFuncs(PickedPerks[i].values[j], PickedPerks[i].types[j]));
		}
		for (int p = 0; p < PickedPerks[i].checkSize; p++)
		{
			perkBehavior.push_back(initiateFuncs(0, PickedPerks[i].checks[p]));
		}
		perkFuncVector.push_back(perkBehavior);
		perkBehavior.clear();
	}
}
