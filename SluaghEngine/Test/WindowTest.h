#pragma once
#ifndef SE_TEST_Window_TEST_H_
#define SE_TEST_Window_TEST_H_
#include "Test.h"
#include <window/Window.h>
#include <window/InterfaceWindow.h>
#include <iostream>


namespace SE
{
	namespace Test
	{
		class WindowTest : public Test
		{
		public:
			WindowTest();
			~WindowTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}


#endif //SE_TEST_Window_TEST_H_