#pragma once
#ifndef SE_TEST_INSTANCING_TEST_H_
#define SE_TEST_INSTANCING_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class InstancingTest :
			public Test
		{
		public:
			InstancingTest();
			~InstancingTest();

			bool Run(DevConsole::IConsole* console) override;
		};

	}
}


#endif // SE_TEST_MATERIAL_MANAGER_TEST_H_