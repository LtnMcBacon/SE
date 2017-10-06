#ifndef SE_TEST_SKILL_TEST_H
#define SE_TEST_SKILL_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class SkillTest : public Test
		{
		public:
			SkillTest();
			~SkillTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}

#endif