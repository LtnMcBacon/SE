#ifndef SE_TEST_ENTITY_MANAGER_TEST_H_
#define SE_TEST_ENTITY_MANAGER_TEST_H_
#include "Test.h"


namespace SE
{
	namespace Test
	{
		class EntityManagerTest : public Test
		{
		public:
			EntityManagerTest();
			~EntityManagerTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}


#endif //SE_TEST_ENTITY_MANAGER_TEST_H_