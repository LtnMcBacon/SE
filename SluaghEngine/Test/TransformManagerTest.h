#ifndef SE_TEST_TRANSFORM_MANAGER_TEST_H_
#define SE_TEST_TRANSFORM_MANAGER_TEST_H_
#include "Test.h"
#include <Core/IEngine.h>
#include <iostream>


namespace SE
{
	namespace Test
	{
		class TransformManagerTest : public Test
		{
		public:
			TransformManagerTest();
			~TransformManagerTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}


#endif //SE_TEST_EN