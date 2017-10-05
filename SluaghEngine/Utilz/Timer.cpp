#include "Timer.h"

SE::Utilz::Timer::Timer()
{
	curTime = std::chrono::high_resolution_clock::now();
	prevTime = curTime;
}

SE::Utilz::Timer::~Timer()
{
}



void SE::Utilz::Timer::Tick()
{
	prevTime = curTime;
	curTime = std::chrono::high_resolution_clock::now();
	int index = 0;
	while (index < delegateVec.size())
	{
		if (curTime - delegateVec[index].startTime > delegateVec[index].delayTime)
		{
			delegateVec[index].delegateCall();
			delegateVec.erase(delegateVec.begin + index);
			continue;
		}
		index++;
	}
}

float SE::Utilz::Timer::GetDeltaSeconds() const
{
	return (float)std::chrono::duration_cast<std::chrono::seconds>(curTime - prevTime).count();
}

float SE::Utilz::Timer::GetDeltaMilliseconds() const
{
	return (float)std::chrono::duration_cast<std::chrono::milliseconds>(curTime - prevTime).count();
}

int SE::Utilz::Timer::StartStopWatch()
{
	if (watchState == false)
	{
		watchStart = std::chrono::high_resolution_clock::now();
		watchState = true;
		return 0;
	}
	return -1;
}

float SE::Utilz::Timer::StopStopWatch()
{
	if (watchState == true)
	{
		watchState = false;
		return (float)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - watchStart).count;
	}
	return 0.0f;
}

void SE::Utilz::Timer::BindAfterTimeCallback(const AfterTimeCallback & callback, std::chrono::seconds time)
{
	delegateTime tempDT;
	tempDT.delegateCall = callback;
	tempDT.delayTime = time;
	tempDT.startTime = std::chrono::high_resolution_clock::now();
	delegateVec.push_back(tempDT);
}
