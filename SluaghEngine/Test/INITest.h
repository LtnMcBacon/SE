#pragma once
#ifndef SE_TEST_UTILZ_INI_TEST_H_
#define SE_TEST_UTILZ_INI_TEST_H_
#include "Test.h"

#include <Utilz\GUID.h>

namespace SE
{
	namespace Test
	{
		class INITest : public Test
		{
		public:
			INITest();
			~INITest();

			bool Run(DevConsole::IConsole* console);
		};
	}
}


#endif //SE_TEST_UTILZ_INI_TEST_H_