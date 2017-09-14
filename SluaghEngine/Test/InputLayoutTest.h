#ifndef INPUTLAYOUTTEST_H
#define INPUTLAYOUTTEST_H
#include "Test.h"

namespace SE {

	namespace Test {

		class InputLayoutTest : public Test 
		{
		public:
			InputLayoutTest();
			~InputLayoutTest();

			bool Run(Utilz::IConsoleBackend* console);

		};
	}
}


#endif
