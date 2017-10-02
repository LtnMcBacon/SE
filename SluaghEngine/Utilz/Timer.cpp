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
}

float SE::Utilz::Timer::GetDeltaSeconds() const
{
	return (float)std::chrono::duration_cast<std::chrono::seconds>(curTime - prevTime).count();
}

float SE::Utilz::Timer::GetDeltaMilliseconds() const
{
	return (float)std::chrono::duration_cast<std::chrono::milliseconds>(curTime - prevTime).count();
}
