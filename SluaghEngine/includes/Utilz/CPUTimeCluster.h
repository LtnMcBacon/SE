#ifndef SE_UTILZ_CPU_TIME_CLUSTER_H_
#define SE_UTILZ_CPU_TIME_CLUSTER_H_
#include <Utilz\TimeCluster.h>
#include <chrono>
namespace SE
{
	namespace Utilz
	{
		class CPUTimeCluster : public TimeCluster
		{
		public:
			CPUTimeCluster() : TimeCluster() {};
			~CPUTimeCluster() {};


			inline void Start(const Utilz::IDHash& id)override
			{
				timers[id].startTime = std::chrono::high_resolution_clock::now();
			};
			inline void Stop(const Utilz::IDHash& id)override
			{
				timers[id].stopTime = std::chrono::high_resolution_clock::now();
			}

			/**
			*
			* @brief Get the time for one timer
			**/
			template<typename TimeType = std::chrono::milliseconds>
			inline float GetTime(const Utilz::IDHash& id)
			{
				return static_cast<float>(std::chrono::duration_cast<TimeType>(timers[id].stopTime - timers[id].startTime).count());
			}


			inline void GetMap(TimeMap& map)override
			{
				for (auto& timer : timers)
					map[timer.first] = this->GetTime(timer.first);
			}
		private:
			struct Timer
			{
				std::chrono::high_resolution_clock::time_point startTime;
				std::chrono::high_resolution_clock::time_point stopTime;
				Timer() : startTime(std::chrono::high_resolution_clock::now()), stopTime(std::chrono::high_resolution_clock::now()) {};
			};

			std::map<Utilz::IDHash, Timer, Utilz::IDHash::Compare> timers;
		};
	}
}


#endif //SE_UTILZ_CPU_TIME_CLUSTER_H_