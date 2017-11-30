#pragma once
#ifndef SE_TEST_Window_TEST_H_
#define SE_TEST_Window_TEST_H_
#include "Test.h"



namespace SE
{
	namespace Test
	{
		class WindowTest : public Test
		{
		public:
			WindowTest();
			~WindowTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}


#endif //SE_TEST_Window_TEST_H_