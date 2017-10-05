#pragma once
#ifndef SE_TEST_RECORDING_TEST_H_
#define SE_TEST_RECORDING_TEST_H_
#include "Test.h"



namespace SE
{
	namespace Test
	{
		class RecordingTest : public Test
		{
		public:
			RecordingTest();
			~RecordingTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}


#endif //SE_TEST_RECORDING_TEST_H_