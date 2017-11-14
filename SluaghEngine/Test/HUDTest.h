#pragma once
#include "Test.h"

namespace SE
{
	namespace Test
	{

		class HUDTest : public Test
		{
		public:
			HUDTest();
			~HUDTest();


			bool Run(SE::DevConsole::IConsole* console);
		private:

		};

		
	}
}