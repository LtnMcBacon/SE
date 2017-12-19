#include "Test.h"

namespace SE::Test
{
#pragma once
#include "Test.h"
	class SDDLockTest :
		public Test
	{
	public:
		SDDLockTest();
		~SDDLockTest();

		bool Run(SE::DevConsole::IConsole* console);
	};

}
