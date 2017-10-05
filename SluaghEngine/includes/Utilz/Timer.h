#pragma once
#include <windows.h>
#include <time.h>
#include <chrono>
#include <vector>
#include <Utilz/Delegate.h>
namespace SE
{
	namespace Utilz
	{
		typedef Utilz::Delegate<void()> AfterTimeCallback;
		class Timer
		{


		public:
			Timer();
			~Timer();
			void Tick();
			float GetDeltaSeconds() const;
			float GetDeltaMilliseconds() const;

			int StartStopWatch();
			float StopStopWatch();

			/**
			* @brief Binds a callback function that is called after a certain amount of time.
			* @sa Delegate
			*/
			void BindAfterTimeCallback(const AfterTimeCallback& callback, std::chrono::seconds time);
			
		private:
			std::chrono::high_resolution_clock::time_point curTime;
			std::chrono::high_resolution_clock::time_point prevTime;

			// stopwatch
			std::chrono::high_resolution_clock::time_point watchStart;
			bool watchState = false;

			// delegate delay
			struct delegateTime
			{
				AfterTimeCallback delegateCall;
				std::chrono::seconds delayTime;
				std::chrono::high_resolution_clock::time_point startTime;
			};
			std::vector<delegateTime> delegateVec;

		};
	}
}


