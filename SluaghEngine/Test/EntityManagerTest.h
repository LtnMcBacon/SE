#ifndef SE_TEST_ENTITY_MANAGER_TEST_H_
#define SE_TEST_ENTITY_MANAGER_TEST_H_
#include "Test.h"
#include <Core/EntityManager.h>
#include <iostream>


namespace SE
{
	namespace Test
	{
		class EntityManagerTest : public Test
		{
		public:
			EntityManagerTest();
			~EntityManagerTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}


#endif //SE_TEST_ENTITY_MANAGER_TEST_H_