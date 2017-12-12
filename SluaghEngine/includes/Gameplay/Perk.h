#pragma once
#pragma once
#include <string>
#include "PerkData.h"
#include <Gameplay\PerkData.h>
#include <vector>
#include <functional>
#include <map>
#include <Gameplay\ProjectileData.h>
#include <Gameplay\perkConditionEnum.h>
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

			
			perkConditions myCondition;
			int intToEnum;
			
			std::vector<perkFunc> perkFunctions;

			
			bool condition = false;
			
			bool checkConditions(std::vector<ProjectileData>& newProjectiles, PlayerUnit* player);

			PerkData slaughPerk;


		private:

			
			
		};
	}
}

