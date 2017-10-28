#include <IOptionsHandler.h>
#include "OptionHandler.h"

DECLDIR_CORE SE::Core::IOptionsHandler * SE::Core::CreateOptionsHandler()
{
	return new OptionHandler;
}
