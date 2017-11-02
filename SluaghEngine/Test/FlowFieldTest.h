#ifndef SE_TEST_FLOW_FIELD_TEST_H
#define SE_TEST_FLOW_FIELD_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class FlowFieldTest : public Test
		{
		public:
			FlowFieldTest();
			~FlowFieldTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}

#endif