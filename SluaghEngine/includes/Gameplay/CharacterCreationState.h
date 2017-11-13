#ifndef _SE_GAMEPLAY_CHARACTER_CREATION_STATE_H
#define _SE_GAMEPLAY_CHARACTER_CREATION_STATE_H
#include "IGameState.h"
#include <Gameplay\HUD_Parsing.h>
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

			/*void SkillBtns(int nrOfSkills);
			void PerkBtns(int nrOfPerks);*/
			void getSkills();
			void importSkillButtons();

			int selectedSkills;
			int renewSkillList;
			int nrOfSkills;

			std::vector<int> chosenSkillsIndex;
			std::vector<Skill> chosenSkills;
			std::vector<int> chosenPerksIndex;
			std::vector<Skill> chosenPerks;
			bool allSkillsSelected = false;

			//	every file to be parsed //
			std::string skillButtonFiles[2]
			{
				"FireBallSkill.HuD",
				"IceBallSkill.HuD"
			};

			std::string perkButtonFiles[2]
			{
				"LifeStealPerk.HuD",
				"VenomBladesPerk.HuD"
			};

			/*std::string skillID[2];
			{
				"Fireball",
				"IceBall"
			};

			std::string perkID[2];
			{
				"lifeSteal",
				"venomblades"
			};*/
			
			
			IGameState::State CurrentState = State::CHARACTER_CREATION_STATE;
		private:

		protected:


		};


	}
}
#endif
