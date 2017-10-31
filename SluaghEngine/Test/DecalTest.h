#ifndef SE_TEST_DECAL_TEST_H_
#define SE_TEST_DECAL_TEST_H_
#include "Test.h"
namespace SE
{
	namespace Test
	{
		class DecalTest :
			public Test
		{
		public:
			DecalTest();
			~DecalTest();
			bool Run(DevConsole::IConsole* console) override;
		};

	}
}

#endif // SE_TEST_DECAL_TEST_H_