#ifndef SE_TEST_BLOOMTEST_H_
#define SE_TEST_BLOOMTEST_H_

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class BloomTest : public Test
		{
		public:
			BloomTest();
			~BloomTest();

			bool Run(Utilz::IConsoleBackend* console);
		};
	}
}

#endif
