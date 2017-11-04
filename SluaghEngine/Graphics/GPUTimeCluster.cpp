#include "GPUTimeCluster.h"
//#include <chrono>


SE::Graphics::GPUTimeCluster::GPUTimeCluster(ID3D11Device* device, ID3D11DeviceContext* dc) : SE::Utilz::TimeCluster(), device(device), dc(dc)
{
	_ASSERT_EXPR(device, "No device in GPUTimeCluster");
	_ASSERT_EXPR(dc, "No deviceContext in GPUTimeCluster");
	currentFrame = 0;
}


SE::Graphics::GPUTimeCluster::~GPUTimeCluster()
{
}

void SE::Graphics::GPUTimeCluster::Start(const Utilz::ConstexprStringAndHash & id)
{
	TimerSet& timer = timers[id];
	if (!timer.QueryStarted && !timer.QueryFinished)
	{


		if (timer.DisjointQuery[currentFrame] == nullptr)
		{
			D3D11_QUERY_DESC desc;
			desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
			desc.MiscFlags = 0;

			HRESULT hr = device->CreateQuery(&desc, &timer.DisjointQuery[currentFrame]);
			if (FAILED(hr))
				throw std::exception("Could not create D3D11_QUERY_TIMESTAMP_DISJOINT");

			desc.Query = D3D11_QUERY_TIMESTAMP;
			hr = device->CreateQuery(&desc, &timer.TimestampStartQuery[currentFrame]);
			if (FAILED(hr))
				throw std::exception("Could not create D3D11_QUERY_TIMESTAMP.");
			device->CreateQuery(&desc, &timer.TimestampEndQuery[currentFrame]);
			if (FAILED(hr))
				throw std::exception("Could not create D3D11_QUERY_TIMESTAMP.");
		}

		// Start a disjoint query.
		dc->Begin(timer.DisjointQuery[currentFrame]);

		// Take a timestap for the start time.
		dc->End(timer.TimestampStartQuery[currentFrame]);

		timer.QueryStarted = true;
	}
}

void SE::Graphics::GPUTimeCluster::Stop(const Utilz::ConstexprStringAndHash & id)
{
	TimerSet& timer = timers[id];

	if (timer.QueryStarted && !timer.QueryFinished)
	{


		// Get the end time stamp
		dc->End(timer.TimestampEndQuery[currentFrame]);

		// End the disjoint query
		dc->End(timer.DisjointQuery[currentFrame]);

		timer.QueryStarted = false;
		timer.QueryFinished = true;
	}
}

float SE::Graphics::GPUTimeCluster::GetTime(TimerSet& timer)
{
	double time = 0.0;

	if (timer.QueryFinished)
	{
		timer.QueryFinished = false;

		if (timer.DisjointQuery[currentFrame] != nullptr)
		{
			//double queryTime = 0.0;

			//auto queryStart = std::chrono::high_resolution_clock::now();
			uint64_t startTime = 0;
			while (dc->GetData(timer.TimestampStartQuery[currentFrame], &startTime, sizeof(startTime), 0) != S_OK);


			uint64_t endTime = 0;
			while (dc->GetData(timer.TimestampEndQuery[currentFrame], &endTime, sizeof(endTime), 0) != S_OK);

			D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
			while (dc->GetData(timer.DisjointQuery[currentFrame], &disjointData, sizeof(disjointData), 0) != S_OK);

			//	auto queryEnd = std::chrono::high_resolution_clock::now();


			if (disjointData.Disjoint == FALSE)
			{
				uint64_t delta = endTime - startTime;
				double freq = static_cast<double>(disjointData.Frequency);
				time = (delta / freq) * 1000.0;
			}
		}		
	}

	return static_cast<float>(time);
}

void SE::Graphics::GPUTimeCluster::GetMap(Utilz::TimeMap & map)
{
	currentFrame = (currentFrame + 1) % QueryLatency;

	for (auto& timer : timers)
	{
		map[timer.first] = GetTime(timer.second);
	}
}
