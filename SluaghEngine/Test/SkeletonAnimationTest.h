#ifndef SE_TEST_SKELETON_ANIMATION_TEST_H_
#define SE_TEST_SKELETON_ANIMATION_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class SkeletonAnimationTest :
			public Test
		{
		public:
			SkeletonAnimationTest();
			~SkeletonAnimationTest();
			bool Run(DevConsole::IConsole* console);
		};
	}
}

#endif // SE_TEST_RENDERABLE_MANAGER_TEST_H_