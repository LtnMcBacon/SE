#ifndef _SE_GAMEPLAY_GAME_H
#define _SE_GAMEPLAY_GAME_H

#include <Core\Engine.h>

#include <window/IWindow.h>

#include <Gameplay\IGameState.h>
#include <Gameplay\PlayState.h>

#include <Gameplay\KeyBindings.h>


namespace SE
{
	namespace Gameplay
	{


		class Game
		{
		private:
			IGameState* state;
			Window::IWindow* input;

		public:
			void Initiate();
			void Run();

		};
	}
}
#endif 

