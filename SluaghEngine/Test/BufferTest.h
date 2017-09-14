#pragma once
#ifndef SE_TEST_BUFFER_GRAPHICS_TEST_H_
#define SE_TEST_BUFFER_GRAPHICS_TEST_H_
#include "Test.h"

namespace SE
{
	namespace Test
	{
		class BufferTest : public Test
		{
		public:
			BufferTest();
			~BufferTest();

			bool Run(Utilz::IConsoleBackend* console);
		};
	}
}


#endif //SE_TEST_BUFFER_GRAPHICS_TEST_H_