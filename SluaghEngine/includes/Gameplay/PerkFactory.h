#pragma once
#include <string>
#include "PerkData.h"
#include <Gameplay\PerkData.h>
#include <vector>
#include <functional>
#include <map>
#include <Gameplay\PlayerUnit.h>
#include <Gameplay\ProjectileData.h>

namespace SE
{
	namespace Gameplay
	{

		class PerkFaktory
		{
		public:
			
			PlayerUnit* player;
			std::vector<ProjectileData>* Projectiles;
			
	
			std::vector<PerkData> PickedPerks;
			std::vector<std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs,float deltaTime,bool condition)>> perkBehavior;
			std::vector<std::vector<std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs, float deltaTime, bool condition)>>> perkFuncVector;

			std::vector<std::function<void(PlayerUnit* player, std::vector<ProjectileData>& Projs,float deltaTime,bool condition)>> perkFunctions;

			std::unordered_map<int, std::function<void(PlayerUnit* player,std::vector<ProjectileData>& Projs,float deltaTime,bool condition)>,Utilz::GUID::Hasher> perkMapping;
			
			bool HealthMultiP = false;
			bool skillCD = false;
			bool skillDamage = false;
			bool intMult = false;
			bool strMult = false;
			bool agiMult = false;

			bool meleeLock = false;
			bool rangeLock = false;
			bool magicLock = false;
			bool waterLock = false;
			bool fireLock = false;
			bool natureLock = false;

			float duration = 5;


			////	PERK ROADMAP \\\\

			enum PerkRoadMap
			{
				//LIFESTEAL
				LifestealPercent = 0,									// not done
				//RESISTANCES
				Physical_Resistance_Percent_Increase = 1,				// done
				Water_Resistance_Percent_Increase = 2,					// done
				Fire_Resistance_Percent_Increase = 3,					// done
				Nature_Resistance_Percent_Increase = 4,					// done
				Magic_Resistance_Percent_Increase = 5,					// done
				Ranged_Resistance_Percent_Increase= 6,					// done
				//STAT INCREASE
				Strength_flat_increase = 7,								// done
				Agillity_flat_increase = 8,								// done
				Intelligence_flat_increase = 9,							// done				
				Health_flat_increase = 10,								// done
				Strength_Percent_increase = 11,							// done
				Agility_Percent_increase = 12,							// done
				Intelligence_Percent_increase = 13,						// done
				Health_Percent_increase = 14,							// done
				//DAMAGE INCREASE
				Damage_Percent_increase = 15,							// done
				Melee_range_Percent_increase = 16,						// not done
				Melee_damage_Percent_increase = 17,						// done
				Range_damage_Percent_increase = 18,						// done
				Attack_speed_Percent_increase = 19,						// done
				Melee_attack_speed_Percent_increase = 20,				// obsolete
				Consecutive_damage_Percent_increase = 21,				// done
				Consecutive_attack_speed_Percent_increase = 22,			// done
				Consecutive_melee_damage_Percent_increase = 23,			// done
				Consecutive_range_damage_Percent_increase = 24,			// done
				Consecutive_melee_attack_speed_Percent_increase = 25,	// obsolete
				Consecutive_range_attack_speed_Percent_increase = 26,	// obsolete
				//SKILL COOLDOWNS N DMG
				Number_of_skills = 27,									// obsolete
				Skill_cooldown = 28,									// done
				Adaptive_Skill_Cooldown = 29,							// done
				Skill_damage_Percent_increase = 30,						// done
				Movement_speed_Percent_increase = 31,					// done
				//UTILITY
				Max_HP_Percent_healing = 32,							// done
				Flat_HP_healing = 33,									// done
				//IMMUNITIES
				Heal_immune = 34,										// done
				Physical_immune = 35,									// done
				Water_immune = 36,										// done
				Fire_immune = 37,										// done
				Nature_immune = 38,										// done
				Slow_immune = 39,										// done
				Stun_immune = 40,										// done
				Magic_immune = 41,										// obsolete?
				Knockback_immune = 42,
				//LOCKS
				Melee_lock = 43,										// done
				Range_lock = 44,										// done
				Magic_lock = 45,										// done
				Water_lock = 46,										// done
				Fire_lock = 47,											// done
				Nature_lock = 48										// done
			};



			 

			PerkFaktory();
			~PerkFaktory();

			void initiateFuncs(int value, int funcEnum, bool switchPerk);
			void initateMap();


		private:

		};
	}
}

