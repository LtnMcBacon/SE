#ifndef SE_TEST_TRANSFORM_TEST_H_
#define SE_TEST_TRANSFORM_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class TransformTest : public Test
		{
		public:
			TransformTest();
			~TransformTest();

			bool Run(Utilz::IConsoleBackend* console) override;
		};


	}
}


#endif // SE_TEST_TRANSFORM_TEST_H_