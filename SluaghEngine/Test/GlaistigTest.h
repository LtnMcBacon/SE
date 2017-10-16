#ifndef SE_GLAISTIG_TEST_H
#define SE_GLAISTIG_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class GlaistigTest : public Test
		{
		public:
			GlaistigTest();
			~GlaistigTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}

#endif