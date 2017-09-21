#ifndef SE_TEST_BOUNDING_TEST_H_
#define SE_TEST_BOUNDING_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class BoundingTest : public Test
		{
		public:
			BoundingTest();
			~BoundingTest();

			bool Run(Utilz::IConsoleBackend* console);
		};
	}
}


#endif // SE_TEST_BOUNDING_TEST_H_