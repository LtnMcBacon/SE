#ifndef SE_UTILZ_TIME_CLUSTER_H_
#define SE_UTILZ_TIME_CLUSTER_H_
#include "GUID.h"
#include <map>
#include <chrono>
namespace SE
{
	namespace Utilz
	{
		class TimeCluster
		{
		public:
			void Start(const GUID& id)
			{
				timers[id].startTime = std::chrono::high_resolution_clock::now();
			}
			void Stop(const GUID& id)
			{
				timers[id].stopTime = std::chrono::high_resolution_clock::now();
			}

			template<typename TimeType = std::chrono::milliseconds>
			float GetTime(const GUID& id)
			{
				return static_cast<float>(std::chrono::duration_cast<TimeType>(timers[id].stopTime - timers[id].startTime).count());
			}
		private:
			struct Timer
			{
				std::chrono::high_resolution_clock::time_point startTime;
				std::chrono::high_resolution_clock::time_point stopTime;
				Timer() : startTime(std::chrono::high_resolution_clock::now()), stopTime(std::chrono::high_resolution_clock::now()) {};
			};

			std::map<GUID, Timer, GUID::Compare> timers;
					
		};
	}
}


#endif //SE_UTILZ_TIME_CLUSTER_H_