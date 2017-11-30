#include <IDataManager.h>
#include "DataManager.h"
DECLDIR_CORE SE::Core::IDataManager * SE::Core::CreateDataManager(const IDataManager::InitializationInfo & info)
{
	return new DataManager(info);
}
