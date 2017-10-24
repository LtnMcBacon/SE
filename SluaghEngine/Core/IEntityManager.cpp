#include <IEntityManager.h>
#include "EntityManager.h"

DECLDIR_CORE SE::Core::IEntityManager * SE::Core::CreateEntityManager()
{
	return new EntityManager;
}
