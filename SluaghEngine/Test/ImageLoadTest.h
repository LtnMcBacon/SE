#pragma once
#ifndef SE_TEST_IMAGELOAD_TEST_H_
#define SE_TEST_IMAGELOAD_TEST_H_
#include "Test.h"

#include <Utilz\GUID.h>

namespace SE
{
	namespace Test
	{
		class ImageLoadTest : public Test
		{
		public:
			ImageLoadTest();
			~ImageLoadTest();

			bool Run(Utilz::IConsoleBackend* console) override;
		private:
			
		};

	}
}


#endif //SE_TEST_IMAGELOAD_TEST_H_