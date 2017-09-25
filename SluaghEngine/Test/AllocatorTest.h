#pragma once
#ifndef SE_TEST_ALLOCATOR_TEST_H_
#define SE_TEST_ALLOCATOR_TEST_H_
#include "Test.h"



namespace SE
{
	namespace Test
	{
		class AllocatorTest : public Test
		{
		public:
			AllocatorTest();
			~AllocatorTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}


#endif //SE_TEST_ALLOCATOR_TEST_H_