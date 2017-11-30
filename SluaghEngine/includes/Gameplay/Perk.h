#pragma once
#pragma once
#include <string>
#include "PerkData.h"
#include <Gameplay\PerkData.h>
#include <vector>
#include <functional>
#include <map>
#include <Gameplay\ProjectileData.h>

namespace SE
{
	namespace Gameplay
	{
		class PlayerUnit;
		
		class Perk
		{
			typedef std::function<void(PlayerUnit* , std::vector<SE::Gameplay::ProjectileData>& , float , bool )> perkFunc;
		public:
			Perk();
			~Perk();

			enum class perkConditions
			{
				NONE = 0,
				PHYSICAL_TAKEN = 1,
				WATER_TAKEN = 2,
				NATURE_TAKEN = 3,
				MAGIC_TAKEN = 4,
				FIRE_TAKEN = 5,

				PHYSICAL_GIVEN = 6,
				WATER_GIVEN = 7,
				NATURE_GIVEN = 8,
				MAGIC_GIVEN = 9,
				FIRE_GIVEN = 10
			};
			perkConditions myCondition;
			int intToEnum;
			
			std::vector<perkFunc> perkFunctions;

			
			//bool HealthMultiP = false;
			//bool skillCD = false;
			//bool skillDamage = false;
			//bool intMult = false;
			//bool strMult = false;
			//bool agiMult = false;

			//bool meleeLock = false;
			//bool rangeLock = false;
			//bool magicLock = false;
			//bool waterLock = false;
			//bool fireLock = false;
			//bool natureLock = false;

			bool condition = false;
			
			bool checkConditions(std::vector<ProjectileData>& newProjectiles, PlayerUnit* player);

			


		private:

			
			
		};
	}
}

