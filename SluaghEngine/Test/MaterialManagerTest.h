#ifndef SE_TEST_MATERIAL_MANAGER_TEST_H_
#define SE_TEST_MATERIAL_MANAGER_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class MaterialManagerTest :
			public Test
		{
		public:
			MaterialManagerTest();
			~MaterialManagerTest();

			bool Run(DevConsole::IConsole* console);
		};
	}
}


#endif // SE_TEST_MATERIAL_MANAGER_TEST_H_