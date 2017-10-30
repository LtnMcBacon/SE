#ifndef SE_GAMEPLAY_SKILL_FACTORY_H_
#define SE_GAMEPLAY_SKILL_FACTORY_H_

#include <string>

namespace SE
{
	namespace Gameplay
	{
		class SkillFactory
		{
		private:			
			
			struct SkillInfo
			{
				std::string skillName = "";
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
				float boonEffectValue = 0.f;
				float boonRange = 0.f;
				float boonDuration = 0.f;
				float baneEffectValue = 0.f;
				float baneRange = 0.f;
				float baneDuration = 0.f;
			};


		public:
			SkillFactory();
			int skillAmounts = 0;

			unsigned int readSkillInfo(std::string& name, unsigned short int* typelist);
			void readAttributesFromFile(unsigned int index, float* attributes);

		};
	}
}

#endif