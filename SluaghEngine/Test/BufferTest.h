#pragma once
#ifndef SE_TEST_BUFFER_GRAPHICS_TEST_H_
#define SE_TEST_BUFFER_GRAPHICS_TEST_H_
#include "Test.h"

#include <Utilz\GUID.h>

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
		private:
			void Load(const Utilz::GUID& guid, void* data, size_t size);
		};
	}
}


#endif //SE_TEST_BUFFER_GRAPHICS_TEST_H_