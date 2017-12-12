#ifndef SE_TEST_PERFORMANCEDECAL_TEST_H_
#define SE_TEST_PERFORMANCEDECAL_TEST_H_
#include "Test.h"
#include "Core/Entity.h"

namespace SE
{
	namespace Test
	{
		class DecalPerformanceTest :
			public Test
		{
		public:
			DecalPerformanceTest();
			~DecalPerformanceTest();
			bool Run(DevConsole::IConsole* console) override;
		private:
			void MakeDecal();
			std::vector<Core::Entity> decals;
			int nrOfDecals = 1;
		};

	}
}

#endif // SE_TEST_PERFORMANCEDECAL_TEST_H_