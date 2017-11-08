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
			CPUTimeCluster() : TimeCluster(){};
			~CPUTimeCluster() {};


			inline void Start(const Utilz::ConstexprStringAndHash& id)override
			{
				timers[id].startTime = std::chrono::high_resolution_clock::now();
			};
			inline void Stop(const Utilz::ConstexprStringAndHash& id)override
			{
				timers[id].time = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - timers[id].startTime).count();
			}

			/**
			*
			* @brief Get the time for one timer
			**/
			template<typename Ratio = std::milli>
			inline float GetTime(const Utilz::ConstexprStringAndHash& id)
			{
				return timers[id].time;
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
				float time = 0.0f;
			};

			std::unordered_map<Utilz::ConstexprStringAndHash, Timer, Utilz::ConstexprStringAndHash::Hasher> timers;
		};
	}
}


#endif //SE_UTILZ_CPU_TIME_CLUSTER_H_