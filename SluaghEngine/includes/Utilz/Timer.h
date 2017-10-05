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
			Timer()
			{
				curTime = std::chrono::high_resolution_clock::now();
				prevTime = curTime;
			};
			~Timer() {};
			void Tick()
			{
				prevTime = curTime;
				curTime = std::chrono::high_resolution_clock::now();
				int index = 0;
				while (index < delegateVec.size())
				{
					if (std::chrono::duration_cast<std::chrono::seconds>(curTime - delegateVec[index].startTime) > delegateVec[index].delayTime)
					{
						delegateVec[index].delegateCall();
						delegateVec.erase(delegateVec.begin() + index);
						continue;
					}
					index++;
				}
			};

			template<typename TimeType = std::chrono::milliseconds>
			inline float GetDelta() const
			{
				return (float)std::chrono::duration_cast<TimeType>(curTime - prevTime).count();
			};

			inline int StartStopWatch()
			{
				if (watchState == false)
				{
					watchStart = std::chrono::high_resolution_clock::now();
					watchState = true;
					return 0;
				}
				return -1;
			};

			template<typename TimeType = std::chrono::milliseconds>
			inline float StopStopWatch()
			{
				if (watchState == true)
				{
					watchState = false;
					return (float)std::chrono::duration_cast<TimeType>(std::chrono::high_resolution_clock::now() - watchStart).count();
				}
				return 0.0f;
			};

			/**
			* @brief Binds a callback function that is called after a certain amount of time.
			* @sa Delegate
			*/
			void BindAfterTimeCallback(const AfterTimeCallback& callback, std::chrono::seconds time)
			{
				delegateTime tempDT;
				tempDT.delegateCall = callback;
				tempDT.delayTime = time;
				tempDT.startTime = std::chrono::high_resolution_clock::now();
				delegateVec.push_back(tempDT);
			};
			
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


