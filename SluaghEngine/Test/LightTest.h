#pragma once
#ifndef SE_TEST_LIGHT_TEST_H_
#define SE_TEST_LIGHT_TEST_H_
#include "Test.h"



namespace SE
{
	namespace Test
	{
		class LightTest : public Test
		{
		public:
			LightTest();
			~LightTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}


#endif //SE_TEST_LIGHT_TEST_H_