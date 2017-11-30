#ifndef SE_TEST_PLAYER_MOVEMENT_TEST_H
#define SE_TEST_PLAYER_MOVEMENT_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class PlayerMovementTest : public Test
		{
		public:
			PlayerMovementTest();
			~PlayerMovementTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}

#endif