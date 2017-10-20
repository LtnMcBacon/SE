#include <IEngine.h>
#include "Engine.h"

DECLDIR_CORE SE::Core::IEngine * SE::Core::CreateEngine()
{
	return new Engine;
}
