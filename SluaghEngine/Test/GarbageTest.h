#ifndef SE_TEST_GARBAGE_TEST_H_
#define SE_TEST_GARBAGE_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class GarbageTest :
			public Test
		{
		public:
			GarbageTest();
			~GarbageTest();
			bool Run(DevConsole::IConsole* console);

			bool MassiveTest(DevConsole::IConsole* console);
			bool ProjectilesIshTest(DevConsole::IConsole* console) const;
		};
	}
}

#endif // SE_TEST_GARBAGE_TEST_H_