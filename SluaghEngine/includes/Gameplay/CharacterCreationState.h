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
			void importPerkButtons();

			void interpretPerks(PerkData perk);

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
				"Bat_Minion.HuD",
				"Heavy_Machine_Bow.HuD",
				"voidRift.HuD",
				"Bat_Swarm.HuD",
				"Dagger_Rain.HuD",
				"Doom_Ball.HuD",
				"Fire_Pillars.HuD",
				"Gae_Bolg.HuD",
				"Kasta_Sten.HuD"
			};

			std::string perkButtonFiles[5]
			{
				"Fire_Resistance.HuD",
				"Life_Steal.HuD",
				"Melee_Damage.HuD",
				"Range_Damage.HuD",
				"Stat_Increase.HuD"
				
			};

			std::string Perkfiles[5]
			{
				 "Fire_Resistance.prk",
				 "Life_Steal.prk",
				 "Melee_Damage.prk",
				 "Range_Damage.prk",
				 "Stat_Increase.prk"
			};
			
			IGameState::State CurrentState = State::CHARACTER_CREATION_STATE;
		private:

		protected:


		};


	}
}
#endif
