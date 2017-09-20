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
	return std::chrono::duration_cast<std::chrono::duration<float>>(curTime - prevTime).count();
}

float SE::Utilz::Timer::GetDeltaMilliseconds() const
{
	return GetDeltaSeconds() * 0.001f;
}
