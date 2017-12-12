#pragma once
#ifndef _SE_GAMEPLAY_OPTION_STATE_H
#define _SE_GAMEPLAY_OPTION_STATE_H
#include "IGameState.h"
#include <Gameplay\HUD_Parsing.h>
#include <Gameplay\PerkImporter.h>
#include <string>
#include <vector>


namespace SE
{
	namespace Gameplay
	{
		/**
		*
		* @brief The Option Class
		*
		* @details class used for the options process, more to be added later, inherits from Gamestate baseclass.
		*
		* @warning Not complete
		*
		* @sa IGameState.
		*
		**/

		class OptionState : public IGameState
		{

		public:
			OptionState();
			~OptionState();

			/**
			* @brief	The Update loop for the Option phase.
			*
			* @details	An update loop to be used during the Option phase, more info added later
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

			IGameState::State CurrentState = State::OPTION_STATE;
		private:
			bool fullscreen = false;
			bool wasFullscreen = false;
			float masterVol = 0;
			float effectVol = 0;
			float voiceVol = 0;
			float bakgroundVol = 0;

			std::map<std::string, SE::Core::Entity> buttonEnt;
			SE::Core::Entity boxNames[5];
		protected:


		};
	}
}
#endif