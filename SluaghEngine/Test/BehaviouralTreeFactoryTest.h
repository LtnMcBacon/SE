#ifndef SE_TEST_BEHAVIOURAL_TREE_FACTORY_TEST_H
#define SE_TEST_BEHAVIOURAL_TREE_FACTORY_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class BehaviouralTreeFactoryTest : public Test
		{
		public:
			BehaviouralTreeFactoryTest();
			~BehaviouralTreeFactoryTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};


	}
}

#endif#pragma once
