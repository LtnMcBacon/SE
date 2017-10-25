#include <ITextManager.h>
#include "TextManager.h"

DECLDIR_CORE SE::Core::ITextManager * SE::Core::CreateTextManager(const ITextManager::InitializationInfo & initInfo)
{
	return new TextManager(initInfo);
}