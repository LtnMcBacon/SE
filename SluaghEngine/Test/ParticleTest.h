#pragma once
#include "Test.h"
namespace SE {
	namespace Test {
		class ParticleTest : public Test
		{
		public:
			bool Run(SE::DevConsole::IConsole* console);

			ParticleTest();
			~ParticleTest();
		};
	}

}

