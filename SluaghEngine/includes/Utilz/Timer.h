#pragma once
#include <windows.h>
#include <time.h>
#include <chrono>
namespace SE
{
	namespace Utilz
	{
		class Timer
		{


		public:
			Timer();
			~Timer();
			void Tick();
			float GetDeltaSeconds() const;
			float GetDeltaMilliseconds() const;
			
		private:
			std::chrono::high_resolution_clock::time_point curTime;
			std::chrono::high_resolution_clock::time_point prevTime;


		};
	}
}


