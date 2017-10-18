#pragma once
#ifndef SE_TEST_RECORD_PROJECTILE_TEST_H
#define SE_TEST_RECORD_PROJECTILE_TEST_H

#include "Test.h"

namespace SE
{
	namespace Test
	{
		class RecordingProjectileTest : public Test
		{
		public:
			RecordingProjectileTest();
			~RecordingProjectileTest();

			bool Run(SE::Utilz::IConsoleBackend* console);
		};
	}
}

#endif