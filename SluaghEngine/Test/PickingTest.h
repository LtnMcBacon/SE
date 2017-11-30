#ifndef SE_TEST_PICKING_TEST_H_
#define SE_TEST_PICKING_TEST_H_

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class PickingTest : public Test
		{
		public:
			PickingTest();
			~PickingTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}

#endif