#ifndef SE_GRAPHICS_GPU_TIME_CLUSTER_H_
#define SE_GRAPHICS_GPU_TIME_CLUSTER_H_
#include <d3d11.h>
#include <Utilz\TimeCluster.h>
namespace SE
{
	namespace Graphics
	{
		class GPUTimeCluster : public Utilz::TimeCluster
		{
		public:


			GPUTimeCluster(ID3D11Device* device, ID3D11DeviceContext* dc);
			~GPUTimeCluster();

			void Start(const Utilz::IDHash& id)override;
			void Stop(const Utilz::IDHash& id)override;
			
			void GetMap(Utilz::TimeMap& map)override;

		private:
			
			ID3D11Device* device;
			ID3D11DeviceContext* dc;

			static const uint8_t QueryLatency = 5;

			uint8_t currentFrame;

			struct TimerSet
			{
				ID3D11Query* DisjointQuery[QueryLatency];
				ID3D11Query* TimestampStartQuery[QueryLatency];
				ID3D11Query* TimestampEndQuery[QueryLatency];
				bool QueryStarted;
				bool QueryFinished;


				TimerSet() : QueryStarted(false), QueryFinished(false)
				{
					for (uint8_t i = 0; i < QueryLatency; i++)
					{						
							DisjointQuery[i] = nullptr;
							TimestampStartQuery[i] = nullptr;
							TimestampEndQuery[i] = nullptr;
					}
				}
				~TimerSet()
				{
					for (uint8_t i = 0; i < QueryLatency; i++)
					{
						if (DisjointQuery[i])
							DisjointQuery[i]->Release();
						if (TimestampStartQuery[i])
							TimestampStartQuery[i]->Release();
						if (TimestampEndQuery[i])
							TimestampEndQuery[i]->Release();
					}
				}
			};

			float GetTime(TimerSet& timer);


			std::unordered_map<Utilz::IDHash, TimerSet, Utilz::IDHash::Hasher> timers;
		};
	}
}


#endif