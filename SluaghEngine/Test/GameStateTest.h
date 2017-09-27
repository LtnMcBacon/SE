#ifndef SE_TEST_GAME_STATE_TEST_H
#define SE_TEST_GAME_STATE_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class GameStateTest : public Test
		{
		public:
			GameStateTest();
			~GameStateTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}

#endif