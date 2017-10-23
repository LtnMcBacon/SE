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
			CPUTimeCluster() : TimeCluster(), currentFrame(0){};
			~CPUTimeCluster() {};


			inline void Start(const Utilz::IDHash& id)override
			{
				if (!timers[id].Started && !timers[id].Finished)
				{
					timers[id].startTime[currentFrame] = std::chrono::high_resolution_clock::now();
					timers[id].Started = true;
				}
			};
			inline void Stop(const Utilz::IDHash& id)override
			{
				if (timers[id].Started && !timers[id].Finished)
				{
					timers[id].stopTime[currentFrame] = std::chrono::high_resolution_clock::now();
					timers[id].Started = false;
					timers[id].Finished = true;
				}
			}

			/**
			*
			* @brief Get the time for one timer
			**/
			template<typename Ratio = std::milli>
			inline float GetTime(const Utilz::IDHash& id)
			{
				if (timers[id].Finished)
				{
					timers[id].Finished = false;
					timers[id].time[currentFrame] = std::chrono::duration<float, Ratio>(timers[id].stopTime[currentFrame] - timers[id].startTime[currentFrame]).count();
					
				}
				return timers[id].time[currentFrame];
			}


			inline void GetMap(TimeMap& map)override
			{
				currentFrame = (currentFrame + 1) % frameDelay;
				for (auto& timer : timers)
					map[timer.first] = this->GetTime(timer.first);
			}
		private:
			static const uint8_t frameDelay = 2;
			uint8_t currentFrame;
			struct Timer
			{
				bool Started = false;
				bool Finished = false;
				float time[frameDelay] = { 0.0f ,0.0f };
				std::chrono::high_resolution_clock::time_point startTime[frameDelay];
				std::chrono::high_resolution_clock::time_point stopTime[frameDelay];
			};

			std::map<Utilz::IDHash, Timer, Utilz::IDHash::Compare> timers;
		};
	}
}


#endif //SE_UTILZ_CPU_TIME_CLUSTER_H_