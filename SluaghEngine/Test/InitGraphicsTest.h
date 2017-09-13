#ifndef SE_TEST_INIT_GRAPHICS_TEST_H_
#define SE_TEST_INIT_GRAPHICS_TEST_H_
#include "Test.h"

namespace SE
{
	namespace Test
	{
		class InitGraphicsTest : public Test
		{
		public:
			InitGraphicsTest();
			~InitGraphicsTest();

			bool Run(Utilz::IConsoleBackend* console);
		};
	}
}


#endif //SE_TEST_INIT_GRAPHICS_TEST_H_