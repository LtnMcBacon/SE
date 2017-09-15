#ifndef SE_TEST_RENDERABLE_MANAGER_TEST_H_
#define SE_TEST_RENDERABLE_MANAGER_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class RenderableManagerTest :
			public Test
		{
		public:
			RenderableManagerTest();
			~RenderableManagerTest();
			bool Run(Utilz::IConsoleBackend* console);
		};
	}
}

#endif // SE_TEST_RENDERABLE_MANAGER_TEST_H_