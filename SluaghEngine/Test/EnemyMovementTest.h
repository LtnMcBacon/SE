#ifndef SE_TEST_ENEMY_MOVEMENT_TEST_H
#define SE_TEST_ENEMY_MOVEMENT_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class EnemyMovementTest : public Test
		{
		public:
			EnemyMovementTest();
			~EnemyMovementTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}

#endif