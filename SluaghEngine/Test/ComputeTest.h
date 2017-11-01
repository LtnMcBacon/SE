#ifndef SE_TEST_COMPUTE_TEST_H_
#define SE_TEST_COMPUTE_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class ComputeTest : public Test
		{
		public:
			ComputeTest();
			~ComputeTest();

			bool Run(SE::DevConsole::IConsole* console);
		};

	}
}

#endif //SE_TEST_COMPUTE_TEST_H_