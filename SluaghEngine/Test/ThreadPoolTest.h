#ifndef SE_TEST_THREADPOOL_H
#define SE_TEST_THREADPOOL_H

#include "Test.h"
namespace SE
{
	namespace Test
	{
		class ThreadPoolTest :
			public Test
		{
		public:
			ThreadPoolTest();
			~ThreadPoolTest();
			bool Run(DevConsole::IConsole* console) override;
		};
	}
}


#endif
