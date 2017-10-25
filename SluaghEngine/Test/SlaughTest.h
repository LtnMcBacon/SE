#ifndef SE_SLAUGH_TEST_H
#define SE_SLAUGH_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class SlaughTest : public Test
		{
		public:
			SlaughTest();
			~SlaughTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}

#endif