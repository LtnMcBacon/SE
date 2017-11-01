#include <IResourceHandler.h>
#include "ResourceHandler.h"

DECLDIR SE::ResourceHandler::IResourceHandler * SE::ResourceHandler::CreateResourceHandler()
{
	ResourceHandler rh;
	rh.Initialize({});

	rh.LoadResource("test.txt",[](auto guid, auto data, auto size, auto udata, auto usize)
	{
		std::string str((const char*)data, size);
		return ResourceHandler::LoadReturn::SUCCESS;
	},
		[](auto guid, auto data, auto size)
	{
		return ResourceHandler::InvokeReturn1::SUCCESS;
	},
	ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::ASYNC);

	rh.LoadResource("test.txt", [](auto guid, auto data, auto size, auto udata, auto usize)
	{
		std::string str((const char*)data, size);
		return ResourceHandler::LoadReturn::SUCCESS;
	},
		[](auto guid, auto data, auto size)
	{
		return ResourceHandler::InvokeReturn1::SUCCESS;
	},
		ResourceHandler::LoadFlags::LOAD_FOR_RAM);



	using namespace std::chrono_literals;
	std::this_thread::sleep_for(2s);


	rh.UnloadResource("test.txt", ResourceHandler::UnloadFlags::RAM);
	rh.UnloadResource("test.txt", ResourceHandler::UnloadFlags::VRAM);


	while (true);
	return new ResourceHandler;
}
