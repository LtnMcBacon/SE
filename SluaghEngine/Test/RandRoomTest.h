#ifndef SE_TEST_RANDROOM_TEST_H
#define SE_TEST_RANDROOM_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class RandRoomTest : public Test
		{
		public:
			RandRoomTest();
			~RandRoomTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}

#endif