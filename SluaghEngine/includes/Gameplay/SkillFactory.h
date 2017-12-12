#ifndef SE_GAMEPLAY_SKILL_FACTORY_H_
#define SE_GAMEPLAY_SKILL_FACTORY_H_

#include <string>
#include <Utilz\GUID.h>

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
				std::string description = "";
				unsigned short int AtkType		= 0;
				unsigned short int Element		= 0;
				unsigned short int Boon			= 0;
				unsigned short int Bane			= 0;
				unsigned short int Animation	= 0;
				unsigned short int Particle		= 0;
			};
			struct SkillAttributes
			{
				std::string projectileReference = "";
				float skillDamage = 0.f;
				float boonEffectValue = 0.f;
				float boonRange = 0.f;
				float boonDuration = 0.f;
				float baneEffectValue = 0.f;
				float baneRange = 0.f;
				float baneDuration = 0.f;
				float cooldown = 0.f;
			};


		public:
			SkillFactory();
			int skillAmounts = 0;

			/**
			* @brief		Returns an array of 3 skill index positions
			*
			* @retval		return_value_1 unsigned int index array.
			**/
			unsigned int getRandomSkillIndex();
			/**
			* @brief		Returns the skill description.
			* 
			* @details		Function will give you only the skill description for the given
			*				skill index.
			*
			* @param[in]	The index of the skill.
			* @param[out]	name will return you the name of the skill.
			* @param[out]	typeList The input must be an array of 6 unsigned short ints. 
			*				Will return the description of the skill, must be manually
			*				converted to the enum types in skill found in player.
			*
			* @Warning		Output value typeList must be manually converted into skill enums found in player.
			**/
			void readSkillInfo(unsigned int index, std::string& name, std::string& desc, unsigned short int* typeList);
			/**
			* @brief		Returns the skills attributes.
			*
			* @details		Function will give you the attributes of the skill on the given index.
			*
			* @param[in]	index is the location of the skill in the file.
			* @param[out]	projectileReference will return you the name of the projectile to use. 
			*				This will then later be compared with a GUID.
			* @param[out]	attributes is the list of all the attributes that skill contains. Must be an array of 8 floats
			*
			* @Warning		Output value typeList must be manually converted into skill enums found in player.
			*
			* @retval		Returns the skill index position in the file
			**/
			void readAttributesFromFile(unsigned int index, SE::Utilz::GUID &projectileReference, float* attributes);

		};
	}
}

#endif