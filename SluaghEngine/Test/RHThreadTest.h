#pragma once
#include "Test.h"

namespace SE::Test
{
	class RHThreadTest :
		public Test
	{
	public:
		RHThreadTest();
		~RHThreadTest();

		bool Run(SE::DevConsole::IConsole* console);
	};
}
