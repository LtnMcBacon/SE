#ifndef _SE_GAMEPLAY_PERKDATA_H_
#define _SE_GAMEPLAY_PERKDATA_H_
#include <string>
#include <vector>
namespace SE
{
	namespace Gameplay
	{
		struct PerkData
		{
			std::string name;
			int checkSize;
			int typeSize;
			int Condition;
			std::vector<int>types;
			std::vector<int>values;
			std::vector<int>checks;
		};
	}
}


#endif