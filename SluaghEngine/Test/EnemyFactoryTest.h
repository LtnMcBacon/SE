#ifndef SE_TEST_ENEMY_FACTORY_TEST_H
#define SE_TEST_ENEMY_FACTORY_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class EnemyFactoryTest : public Test
		{
		public:
			EnemyFactoryTest();
			~EnemyFactoryTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};


	}
}

#endif#pragma once
