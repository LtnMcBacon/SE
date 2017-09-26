#ifndef SE_TEST_DEBUG_RENDER_MANAGER_TEST_H_
#define SE_TEST_DEBUG_RENDER_MANAGER_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class DebugRenderManagerTest :
			public Test
		{
		public:
			DebugRenderManagerTest();
			~DebugRenderManagerTest();

			bool Run(Utilz::IConsoleBackend* console);
		};
	}
}


#endif // SE_TEST_DEBUG_RENDER_MANAGER_TEST_H_