#ifndef SE_TEST_MATERIAL_TEST_H_
#define SE_TEST_MATERIAL_TEST_H_
#include "Test.h"

namespace SE {

	namespace Test {

		class MaterialTest : public Test 
		{
		public:
			MaterialTest();
			~MaterialTest();

			bool Run(Utilz::IConsoleBackend* console);

		};
	}
}


#endif // SE_TEST_MATERIAL_TEST_H_
