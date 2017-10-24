#include <IParticleSystemManager.h>
#include "ParticleSystemManager.h"

DECLDIR_CORE SE::Core::IParticleSystemManager * SE::Core::CreateParticleSystemManager(const IParticleSystemManager::InitializationInfo & initInfo)
{
	return new ParticleSystemManager(initInfo);
}
