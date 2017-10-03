#ifndef SE_TEST_PROJECTILE_TEST_H
#define SE_TEST_PROJECTILE_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class ProjectileTest : public Test
		{
		public:
			ProjectileTest();
			~ProjectileTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}

#endif