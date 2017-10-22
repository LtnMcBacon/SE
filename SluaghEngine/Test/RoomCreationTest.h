#ifndef SE_TEST_ROOM_CREATION_TEST_H
#define SE_TEST_ROOM_CREATION_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class RoomCreationTest : public Test
		{
		public:
			RoomCreationTest();
			~RoomCreationTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}

#endif