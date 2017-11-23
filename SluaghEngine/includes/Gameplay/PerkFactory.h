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
			union parameters
			{
				PlayerUnit* player;
				std::vector<ProjectileData>* Projectiles;
			};
	
			std::vector<PerkData> PickedPerks;
			std::vector<std::function<void()>> perkBehavior;
			std::vector<std::vector<std::function<void()>>> Perks;

			std::vector<std::function<void(std::vector<parameters> player)>> perkFunctions;
			std::unordered_map<Utilz::GUID, std::function<void()>,Utilz::GUID::Hasher> perkMapping;
			

			PerkFaktory();
			~PerkFaktory();

			void initiateFuncs();
			void initateMap();


		private:

		};
	}
}

