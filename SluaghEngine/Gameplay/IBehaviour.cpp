#include "IBehaviour.h"
#include <Profiler.h>

using namespace SE;
using namespace GAMEPLAY;

Status IBehaviour::Tick()
{
	StartProfile;
	if (myStatus != Status::BEHAVIOUR_RUNNING)
		OnInitialization();
	myStatus = Update();
	if (myStatus != Status::BEHAVIOUR_RUNNING)
		OnTermination(myStatus);
	
	ProfileReturn(myStatus);
}
