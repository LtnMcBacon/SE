#include <Gameplay\PerkFactory.h>
#include <Gameplay\PlayerUnit.h>
#include "CoreInit.h"
#include <Gameplay\Items.h>
#include <Profiler.h>
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
	StartProfile;
	float consecutiveValue = 0;
	float timer = 0;
	float duration = 5.f;
	bool intMult = false;
	bool strMult = false;
	bool agiMult = false;
	bool HealthMultiP = false;
	bool skillCD = false;
	bool skillDamage = false;
	bool meleeLock = false;
	bool rangeLock = false;
	bool magicLock = false;
	bool waterLock = false;
	bool fireLock = false;
	bool natureLock = false;

	switch (funcEnum)
	{
	case 0:
		CoreInit::subSystems.devConsole->Print("Faulty perk Error! NR: 0");
		break;
	case 1:
	{
		auto PhysicalResistance = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
				std::vector<DamageEvent> DmgVector = player->GetDamageEvents();

				for (auto& Event : DmgVector)
				{
					if (Event.type == DamageType::PHYSICAL)
					{
						float reduced = Event.amount *(value / 100.f);
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
		auto WaterResistance = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
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
		auto FireResistance = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
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
		auto NatureResistance = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer>0)
			{
				timer -= deltaTime;
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
		auto MagicResistance = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer>0)
			{
				timer -= deltaTime;
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
		auto RangedResistance = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer>0)
			{
				timer -= deltaTime;
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

		auto StrengthFlat = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			if (timer >0)
			{
				timer -= deltaTime;
				player->AddNewStrength(value);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> StrengthFlatFunc = StrengthFlat;
		return StrengthFlatFunc;
		break;
	}
	case 8:
	{

		auto AgilityFlat = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
				player->AddNewAgility(value);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> AgilityFlatFunc = AgilityFlat;
		return AgilityFlatFunc;
		break;
	}
	case 9:
	{

		auto IntelligenceFlat = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
		
			if (timer > 0)
			{
				timer -= deltaTime;
				player->AddNewWhisdom(value);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> IntelligenceFlatFunc = IntelligenceFlat;
		return IntelligenceFlatFunc;
		break;
	}
	case 10:
	{

		auto HealthFlat = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			if (timer>0)
			{
				timer -= deltaTime;
				player->AddNewMaxHealth(value);
			}
			
		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> HealthFlatFunc = HealthFlat;
		return HealthFlatFunc;
		break;
	}
	case 11:
	{

		auto strMultiplier = [value, duration, consecutiveValue, timer, strMult](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
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
	case 12:
	{

		auto intMultiplier = [value, duration, consecutiveValue, timer, intMult](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
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
	case 13:
	{

		auto agiMultiplier = [value, duration, consecutiveValue, timer, agiMult](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
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

		auto HealthMulti = [value, duration, HealthMultiP](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
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

		auto DamageMulti = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;

			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
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

		auto MeleeDamage = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
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

		auto RangeDamage = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
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

		auto AttackSpeed = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			if (timer > 0)
			{
				timer -= deltaTime;
				player->AddBaseAttackMult(value / 100.f);
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

		auto ConsecutiveDmG = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{

				timer = duration;
			}
			

			if (timer >0)
			{
				timer -= deltaTime;
				consecutiveValue += value;
				float rangedDMG = player->GetBaseRangedMulitplier();
				float meleeDMG = player->GetBaseMeleeDamageMultiplier();
				float magicDMG = player->GetBaseMagicMulitplier();

				float newRangedDMG = rangedDMG + (consecutiveValue / 100.f);
				float newMeleeDMG = meleeDMG + (consecutiveValue / 100.f);
				float newMagicDMG = magicDMG + (consecutiveValue / 100.f);

				player->AddNewRangedMulitplier(newRangedDMG);
				player->AddNewMeleeDamageMultiplier(newMeleeDMG);
				player->AddNewMagicMulitplier(newMagicDMG);
			}
			else
			{
				consecutiveValue = 0;
			}
			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveDmGFunc = ConsecutiveDmG;
		return ConsecutiveDmGFunc;
		break;
	}
	case 22:
	{
		
		auto ConsecutiveAttackSpeed = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{
				timer = duration;
				consecutiveValue += value;
				player->AddBaseConsAttackMult(value / 100.f);
				
			}
			if (timer > 0)
			{
				timer -= deltaTime;
			}
			else
			{
				player->RemoveBaseConsAttackMult(consecutiveValue / 100.f);
				consecutiveValue = 0;
			}

			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveAttackSpeedFunc = ConsecutiveAttackSpeed;
		return ConsecutiveAttackSpeedFunc;
		break;
	}
	case 23:
	{

		auto ConsecutiveMeleeDmg = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{
				timer = duration;

			}
			
			if (timer>0)
			{
				timer -= deltaTime;
				consecutiveValue += value;
				float AD = player->GetBaseMeleeDamageMultiplier();
				float newAD = AD * (consecutiveValue / 100.f);
				player->AddNewMeleeDamageMultiplier(newAD);
			}
			else
			{
				consecutiveValue = 0;
			}
			

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> ConsecutiveMeleeDmgFunc = ConsecutiveMeleeDmg;
		return ConsecutiveMeleeDmgFunc;
		break;
	}
	case 24:
	{

		auto ConsecutiveRangedDmg = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{
				timer = duration;

			}
	
			if (timer > 0)
			{
				timer -= deltaTime;
				consecutiveValue += value;
				float AD = player->GetBaseRangedMulitplier();
				float newAD = AD * (consecutiveValue / 100.f);
				player->AddNewRangedMulitplier(newAD);
			}
			else
			{
				consecutiveValue = 0;
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

		auto skillCooldown = [value, duration, consecutiveValue, timer, skillCD](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				if (!skillCD)
				{
					int currentCooldown = player->GetCooldown(0);
					int currentCooldown2 = player->GetCooldown(1);

					float newCD = currentCooldown * (value / 100.f);
					float newCD2 = currentCooldown2 * (value / 100.f);

					float skill1CD = newCD + currentCooldown;
					float skill2CD = newCD2 + currentCooldown2;
					player->SetCooldown(0, skill1CD);
					player->SetCooldown(1, skill2CD);
					skillCD = true;
				}
			}


		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> skillCooldownFunc = skillCooldown;
		return skillCooldownFunc;
		break;
	}
	case 29:
	{

		auto AdaptiveSkillCD = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
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

		auto SkillDmg = [value, duration, consecutiveValue, timer, skillDamage](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
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

		auto MovementSpeed = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition) mutable->void
		{
			if (condition)
			{
				timer = duration;

			}
			
			if (timer > 0)
			{
				timer -= deltaTime;

				float MS = player->GetBaseMovement();

				consecutiveValue = MS * (value / 100.f);

				player->AddNewMovement(consecutiveValue);
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

		auto HealImmune = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
				
			}
			if (timer>0)
			{
				timer -= deltaTime;
				player->ClearHealingEvents();
			}

		}; std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)> HealImmuneFunc = HealImmune;
		return HealImmuneFunc;
		break;
	}
	case 35:
	{

		auto PhysicalImmune = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
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

		auto WaterImmune = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			if (timer>0)
			{
				timer -= deltaTime;
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

		auto FireImmune = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			if (timer > 0)
			{
				timer -= deltaTime;
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

		auto NatureImmune = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			if (timer>0)
			{
				timer -= deltaTime;
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

		auto SlowImmune = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer>0)
			{
				timer -= deltaTime;
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

		auto StunImmune = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer>0)
			{
				timer -= deltaTime;
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

		auto MagicImmune = [value, duration, consecutiveValue, timer](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer>0)
			{
				timer -= deltaTime;
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

		auto MeleeLock = [value, duration, consecutiveValue, timer, meleeLock](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
				meleeLock = true;

				auto weapons = player->GetAllItems();

				for (size_t i = 0; i < 4; i++)
				{
					auto itype = (ItemType)(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1)));
					auto isitem = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1));
					auto damageType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "DamageType", -1));

					if (isitem != -1 && itype == ItemType::WEAPON && DamageType(damageType) == DamageType::PHYSICAL)
					{
						auto p = CoreInit::managers.transformManager->GetPosition(player->GetEntity());
						p.y = 0;

						if (player->GetCurrentItem() == weapons[i])
							Item::Unequip(weapons[i], player->GetEntity());

						Item::Drop(weapons[i], p);
						player->AddItem(-1, i);

					}
				}
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

		auto RangeLock = [value, duration, consecutiveValue, timer, rangeLock](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			if (timer > 0)
			{
				timer -= deltaTime;
				rangeLock = true;

				auto weapons = player->GetAllItems();

				for (size_t i = 0; i < 4; i++)
				{
					auto itype = (ItemType)(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1)));
					auto isitem = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1));
					auto damageType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "DamageType", -1));

					if (isitem != -1 && itype == ItemType::WEAPON && DamageType(damageType) == DamageType::RANGED)
					{
						auto p = CoreInit::managers.transformManager->GetPosition(player->GetEntity());
						p.y = 0;

						if (player->GetCurrentItem() == weapons[i])
							Item::Unequip(weapons[i], player->GetEntity());

						Item::Drop(weapons[i], p);
						player->AddItem(-1, i);
					}
				}
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

		auto MagicLock = [value, duration, consecutiveValue, timer, magicLock](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;

			}
			if (timer > 0)
			{
				timer -= deltaTime;
				magicLock = true;

				auto weapons = player->GetAllItems();

				for (size_t i = 0; i < 4; i++)
				{
					auto itype = (ItemType)(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1)));
					auto isitem = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1));
					auto damageType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "DamageType", -1));

					if (isitem != -1 && itype == ItemType::WEAPON && DamageType(damageType) == DamageType::MAGIC)
					{
						auto p = CoreInit::managers.transformManager->GetPosition(player->GetEntity());
						p.y = 0;

						if (player->GetCurrentItem() == weapons[i])
							Item::Unequip(weapons[i], player->GetEntity());

						Item::Drop(weapons[i], p);
						player->AddItem(-1, i);

					}
				}
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

		auto WaterLock = [value, duration, consecutiveValue, timer, waterLock](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				timer -= deltaTime;
				waterLock = true;

				auto weapons = player->GetAllItems();

				for (size_t i = 0; i < 4; i++)
				{
					auto itype = (ItemType)(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1)));
					auto isitem = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1));
					auto damageType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "DamageType", -1));

					if (isitem != -1 && itype == ItemType::WEAPON && DamageType(damageType) == DamageType::WATER)
					{
						auto p = CoreInit::managers.transformManager->GetPosition(player->GetEntity());
						p.y = 0;

						if (player->GetCurrentItem() == weapons[i])
							Item::Unequip(weapons[i], player->GetEntity());

						Item::Drop(weapons[i], p);
						player->AddItem(-1, i);

					}
				}
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

		auto FireLock = [value, duration, consecutiveValue, timer, fireLock](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			if (timer > 0)
			{
				timer -= deltaTime;
				fireLock = true;

				auto weapons = player->GetAllItems();

				for (size_t i = 0; i < 4; i++)
				{
					auto itype = (ItemType)(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1)));
					auto isitem = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1));
					auto damageType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "DamageType", -1));


					if (isitem != -1 && itype == ItemType::WEAPON && DamageType(damageType) == DamageType::FIRE)
					{
						auto p = CoreInit::managers.transformManager->GetPosition(player->GetEntity());
						p.y = 0;

						if(player->GetCurrentItem() == weapons[i])
							Item::Unequip(weapons[i], player->GetEntity());

						Item::Drop(weapons[i], p);
						player->AddItem(-1, i);

					}
				}
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

		auto NatureLock = [value, duration, consecutiveValue, timer, natureLock](PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)mutable->void
		{
			if (condition)
			{
				timer = duration;
			}
			
			if (timer > 0)
			{
				
				timer -= deltaTime;
				natureLock = true;

				auto weapons = player->GetAllItems();

				for (size_t i = 0; i < 4; i++)
				{
					auto itype = (ItemType)(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1)));
					auto isitem = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "Item", -1));
					auto damageType = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(weapons[i], "DamageType", -1));

					if (isitem != -1 && itype == ItemType::WEAPON && DamageType(damageType) == DamageType::NATURE)
					{
						auto p = CoreInit::managers.transformManager->GetPosition(player->GetEntity());
						p.y = 0;

						if (player->GetCurrentItem() == weapons[i])
							Item::Unequip(weapons[i], player->GetEntity());

						Item::Drop(weapons[i], p);
						player->AddItem(-1, i);

					}
				}
			}
			else
			{
				natureLock = false;
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


Perk PerkFaktory::ReadPerksForSlaugh(std::string perkName)
{
	perkName.append(".prk");

	Utilz::GUID fileName = perkName;

		PerkData tempData;
	auto res = CoreInit::subSystems.resourceHandler->LoadResource((Utilz::GUID)fileName, [&tempData](auto guid, auto data, auto size)mutable
	{
		size_t head = 0;
		int nameSize;

		memcpy(&nameSize, data, sizeof(int));
		head += sizeof(int);
		char* tempName = new char[nameSize + 1];
		tempName[nameSize] = 0;

		memcpy(tempName, (char*)data + head, sizeof(char) * nameSize);
		tempData.name = tempName;
		head += sizeof(char) * nameSize;

		memcpy(&tempData.checkSize, (char*)data + head, sizeof(int));
		head += sizeof(int);

		memcpy(&tempData.typeSize, (char*)data + head, sizeof(int));
		head += sizeof(int);

		memcpy(&tempData.condition, (char*)data + head, sizeof(int));
		head += sizeof(int);

		for (size_t i = 0; i < tempData.typeSize; i++)
		{
			int type = -1;
			int value = -1;
			memcpy(&type, (char*)data + head, sizeof(int));
			head += sizeof(int);

			memcpy(&value, (char*)data + head, sizeof(int));
			head += sizeof(int);


			tempData.types.push_back(type);
			tempData.values.push_back(value);
		}
		for (size_t i = 0; i < tempData.checkSize; i++)
		{
			int check = -1;
			memcpy(&check, (char*)data + head, sizeof(int));
			head += sizeof(int);
			tempData.checks.push_back(check);
		}

		
		delete[] tempName;
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	PickedPerks.push_back(tempData);
	iteratePerks();


	Perk slaughPerk;
	

	perkConditions myCond = static_cast<perkConditions>(tempData.condition);
	slaughPerk.myCondition = myCond;
	slaughPerk.slaughPerk = tempData;

	for (auto& func : perkFuncVector)
	{
		slaughPerk.perkFunctions = func;
	}
	
	
	ProfileReturnConst(slaughPerk);
	
}
