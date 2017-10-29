#ifndef SE_TEST_SHADOW_TEST_H_
#define SE_TEST_SHADOW_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class ShadowTest :
			public Test
		{
		public:
			ShadowTest();
			~ShadowTest();
			bool Run(DevConsole::IConsole* console);
		};
	}
}

#endif 