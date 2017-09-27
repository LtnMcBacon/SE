#ifndef SE_TEST_RESOURCE_HANDLER_TEST_H_
#define SE_TEST_RESOURCE_HANDLER_TEST_H_
#include "Test.h"

namespace SE
{
	namespace Test
	{
		class ResourceHandlerTest : public Test
		{
		public:
			ResourceHandlerTest();
			~ResourceHandlerTest();

			bool Run(Utilz::IConsoleBackend* backend);
		};
	}
}


#endif