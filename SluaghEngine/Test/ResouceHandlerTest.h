#ifndef SE_TEST_RESOURCE_HANDLER_TEST_H_
#define SE_TEST_RESOURCE_HANDLER_TEST_H_
#include "Test.h"

namespace SE
{
	namespace Test
	{
		class ResouceHandlerTest : public Test
		{
		public:
			ResouceHandlerTest();
			~ResouceHandlerTest();

			bool Run(Utilz::IConsoleBackend* backend);
		};
	}
}


#endif