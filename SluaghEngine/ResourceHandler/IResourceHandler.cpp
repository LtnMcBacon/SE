#include <IResourceHandler.h>
#include "ResourceHandler.h"

DECLDIR SE::ResourceHandler::IResourceHandler * SE::ResourceHandler::CreateResourceHandler()
{
	return new ResourceHandler;
}
