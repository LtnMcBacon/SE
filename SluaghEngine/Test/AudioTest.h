#pragma once
#ifndef SE_TEST_AUDIO_TEST_H_
#define SE_TEST_AUDIO_TEST_H_
#include "Test.h"



namespace SE
{
	namespace Test
	{
		class AudioTest : public Test
		{
		public:
			AudioTest();
			~AudioTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}


#endif //SE_TEST_AUDIO_TEST_H_