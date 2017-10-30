#ifndef _SE_GAMEPLAY_GAME_H
#define _SE_GAMEPLAY_GAME_H

#include <window/IWindow.h>

#include <Gameplay\IGameState.h>
#include <Gameplay\PlayState.h>

#include <Gameplay\KeyBindings.h>

#include <Core\IEngine.h>
namespace SE
{
	namespace Gameplay
	{


		class Game
		{
		private:
			IGameState* state;
			Window::IWindow* input;
			SE::Core::IEngine* engine;

		public:
			void Initiate(Core::IEngine* engine);
			void Run();
			void Shutdown();

		};
	}
}
#endif 

