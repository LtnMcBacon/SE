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
			bool Run(Utilz::IConsoleBackend* console);

			bool MassiveTest(Utilz::IConsoleBackend* console);
			bool ProjectilesIshTest(Utilz::IConsoleBackend* console);
		};
	}
}

#endif // SE_TEST_GARBAGE_TEST_H_