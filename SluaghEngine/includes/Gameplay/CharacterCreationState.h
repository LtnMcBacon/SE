#ifndef _SE_GAMEPLAY_CHARACTER_CREATION_STATE_H
#define _SE_GAMEPLAY_CHARACTER_CREATION_STATE_H
#include "IGameState.h"
#include <Gameplay\HUD_Parsing.h>
#include <Gameplay\PerkImporter.h>
#include <string>
#include <vector>
//#include <SkillFactory.h>
//#include <PlayerUnit.h>
#include <Gameplay\Skill.h>

namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The Character Creation Class
		*
		* @details class used for the character creation process, more to be added later, inherits from Gamestate baseclass.
		*
		* @warning Not complete
		*
		* @sa IGameState.
		*
		**/

		class CharacterCreationState : public IGameState
		{

		public:
			CharacterCreationState();
			CharacterCreationState(Window::IWindow* Input);
			~CharacterCreationState();


			/**
			* @brief	The Update loop for the Character creation phase.
			*
			* @details	An update loop to be used during the Character Creation phase, more info added later
			*
			* @param[in,out] passableInfo is a void pointer which lets us send information inbetween the states.
			*
			* @retval State return value to which other state to switch to when done.
			*
			* @warning Not fully implemented.
			*
			*/

			State Update(void* &passableInfo);
			HUDParser fileParser;
			PerkImporter perks;
			/*void SkillBtns(int nrOfSkills);
			void PerkBtns(int nrOfPerks);*/
			void getSkills();
			void getPerks();
			std::string randomizePerk();
			void importSkillButtons();

			int selectedSkills;
			int renewSkillList;
			int selectedPerks;
			int renewPerks;

			int nrOfSkills;
			int nrOfPerks;
			std::vector<int> chosenSkillsIndex;
			std::vector<Skill> chosenSkills;
			std::vector<std::string> chosenPerkName;
			std::vector<PerkData> chosenPerks;
			bool allSkillsSelected = false;

			//	every file to be parsed //
			std::string skillButtonFiles[9]
			{
				"FireBallSkill.HuD",
				"IceBallSkill.HuD",
				"MagicBallSkill.HuD",
				"NatureBallSkill.HuD",
				"WaterBallSkill.HuD",
				"PhysicalBallSkill.HuD",
				"ArcaneBallSkill.HuD",
				"MudBallSkill.HuD",
				"FistingBallSkill.HuD"
			};

			std::string perkButtonFiles[2]
			{
				"LifeStealPerk.HuD",
				"VenomBladesPerk.HuD"
			};

			std::string Perkfiles[2]
			{

				 "fuckYou.prk",
				 "testPerkData.prk"

			};
			
			IGameState::State CurrentState = State::CHARACTER_CREATION_STATE;
		private:

		protected:


		};


	}
}
#endif
