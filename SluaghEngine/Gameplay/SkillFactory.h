#ifndef SE_GAMEPLAY_SKILL_FACTORY_H_
#define SE_GAMEPLAY_SKILL_FACTORY_H_

#include <string>
#define SKILL_AMOUNT 1

namespace SE
{
	namespace Gameplay
	{
		class SkillFactory
		{
		private:			
			struct SkillInfo
			{
				std::string skillName			= "";
				unsigned short int AtkType		= 0;
				unsigned short int Element		= 0;
				unsigned short int Boon			= 0;
				unsigned short int Bane			= 0;
				unsigned short int Animation	= 0;
				unsigned short int Particle		= 0;
			};
			struct SkillAttributes
			{
				float skillDamage = 0.f;
				float effectValue = 0.f;
				float range = 0.f;
				float duration = 0.f;
			};

			unsigned int skillAmounts = SKILL_AMOUNT;

		public:
			unsigned int characterCreationRetriveSkill(std::string name, unsigned short int* typeList);
			void characterCreationRandomizeSkill(std::string name, unsigned short int* typeList);
			void readSkillInfo(unsigned int index, std::string name, unsigned short int* typelist);
			void readAttributesFromFile(unsigned int index, float* attributes);

		};
	}
}

#endif