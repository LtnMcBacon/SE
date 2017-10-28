#ifndef SE_TEST_OBJ_LOADER_TEST_H_
#define SE_TEST_OBJ_LOADER_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class ObjLoaderTest : public Test
		{
		public:
			ObjLoaderTest();
			~ObjLoaderTest();

			bool Run(DevConsole::IConsole* console);
		};

	}
}

#endif