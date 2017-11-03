#pragma once
#pragma once
#ifndef SE_TEST_PLAYBACK_PROJECTILE_TEST_H
#define SE_TEST_PLAYBACK_PROJECTILE_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class PlaybackProjectileTest : public Test
		{
		public:
			PlaybackProjectileTest();
			~PlaybackProjectileTest();

			bool Run(SE::DevConsole::IConsole* console);
		};
	}
}

#endif