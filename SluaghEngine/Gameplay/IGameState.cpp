#include "IGameState.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;

IGameState::IGameState()
{

}
IGameState::IGameState(void* state)
{

}

IGameState::~IGameState()
{

}

void IGameState::InitializeState()
{
	StartProfile;


	//code



	StopProfile;

}

void IGameState::ChangeState()
{
	StartProfile;


	//code



	StopProfile;
}

void IGameState::UpdateState()
{
	StartProfile;


	//code



	StopProfile;
}

void IGameState::EndState()
{
	StartProfile;


	//code



	StopProfile;
}
