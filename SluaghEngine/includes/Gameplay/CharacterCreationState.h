#ifndef _SE_GAMEPLAY_CHARACTER_CREATION_STATE_H
#define _SE_GAMEPLAY_CHARACTER_CREATION_STATE_H
#include "IGameState.h"
#include <Gameplay\HUD_Parsing.h>
#include <Gameplay\PerkImporter.h>
#include <string>
#include <vector>
#include <Gameplay\PerkFactory.h>
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
			PerkFaktory Pfactory;
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
			std::string skillButtonFiles[18]
			{
				"Bat_Minion.HuD",
				"Heavy_Machine_Bow.HuD",
				"voidRift.HuD",
				"Bat_Swarm.HuD",
				"Dagger_Rain.HuD",
				"Doom_Ball.HuD",
				"Fire_Pillars.HuD",
				"Gae_Bolg.HuD",
				"Kasta_Sten.HuD",
				"Blixt.HuD",
				"Eldens_Bolero.HuD",
				"Kasta_korn.HuD",
				"Sol_explosion.HuD",
				"Spruta_Eld.HuD",
				"Storm_melodin.HuD",
				"Studs_Boll.HuD",
				"Tidens_melodi.HuD",
				"Vasst_blad.HuD",
			};

			std::string perkButtonFiles[18]
			{
				"Krigs_herre.HuD",
				"Tom_i_skallen.HuD",
				"Bokmal.HuD",
				"Bumling.HuD",
				"Eldig.HuD",
				"Frostig.HuD",
				"Trollformel_kastare.HuD",
				"Heta_grejer.HuD",
				"Mitt_i_prick.HuD",
				"Dunkel_ande.HuD",
				"Nulifiera.HuD",
				"Oljad_blixt.HuD",
				"Rocka_och_rulla.HuD",
				"Som_fisken_i_vattnet.HuD",
				"Veva_upp.HuD",
				"Vulkan.HuD",
				"Masochism.HuD",
				"Tunga_slag.HuD"
				
			};

			std::string Perkfiles[18]
			{
				 "Krigs_herre.prk",
				 "Tom_i_skallen.prk",
				 "Bokmal.prk",
				 "Bumling.prk",
				 "Eldig.prk",
				 "Frostig.prk",
				 "Trollformel_kastare.prk",
				 "Heta_grejer.prk",
				 "Mitt_i_prick.prk",
				 "Dunkel_ande.prk",
				 "Nulifiera.prk",
				 "Oljad_blixt.prk",
				 "Rocka_och_rulla.prk",
				 "Som_fisken_i_vattnet.prk",
				 "Veva_upp.prk",
				 "Vulkan.prk",
				 "Masochism.prk",
				 "Tunga_slag.prk"
			};
			
			IGameState::State CurrentState = State::CHARACTER_CREATION_STATE;
		private:

		protected:


		};


	}
}
#endif
