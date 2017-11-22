#pragma once
#include <string>
#include "PerkData.h"
#include <Gameplay\PerkData.h>
#include <vector>
#include <functional>
#include <map>
#include <Gameplay\PlayerUnit.h>

namespace SE
{
	namespace Gameplay
	{

		class PerkFaktory
		{
		public:
	
			std::vector<PerkData> PickedPerks;
			std::vector<std::function<void()>> perkBehavior;
			std::vector<std::vector<std::function<void()>>> Perks;

			std::vector<std::function<void()>> perkFunctions;
			std::map<std::string, std::function<void()>> perkMapping;

			PerkFaktory();
			~PerkFaktory();

			void initiateFuncs();
			void initateMap();

		private:

		};
	}
}

