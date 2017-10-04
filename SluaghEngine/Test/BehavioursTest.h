#ifndef SE_TEST_BEHAVIOURS_TEST_H
#define SE_TEST_BEHAVIOURS_TEST_H

#include "Test.h"
#include <Gameplay/Behaviours.h>

namespace SE
{
	namespace Test
	{
		class BehavioursTest : public Test
		{
		public:
			BehavioursTest();
			~BehavioursTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};

		
	}
}

#endif