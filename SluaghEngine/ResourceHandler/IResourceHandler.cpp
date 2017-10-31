#include <IResourceHandler.h>
#include "ResourceHandler.h"

DECLDIR SE::ResourceHandler::IResourceHandler * SE::ResourceHandler::CreateResourceHandler()
{
	ResourceHandler rh;
	rh.Initialize({});

	rh.LoadResource("test.txt",[](auto guid, auto data, auto size, auto udata, auto usize)
	{
		std::string str((const char*)data, size);
		return ResourceHandler::LoadReturn::VRAM;
	},
		[](auto guid, auto data, auto size)
	{
		return ResourceHandler::InvokeReturn1::SUCCESS;
	},
	ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::ASYNC);
	while (true);
	return new ResourceHandler;
}
