#include "ResouceHandlerTest.h"
#include <ResourceHandler\IResourceHandler.h>
#include <ResourceHandler\ResourceHandler.h>
#include <string>
#ifdef _DEBUG
#pragma comment(lib, "ResourceHandlerD.lib")
#else
#pragma comment(lib, "ResourceHandler.lib")
#endif

using namespace SE::Test;
ResouceHandlerTest::ResouceHandlerTest()
{
}


ResouceHandlerTest::~ResouceHandlerTest()
{
}

bool SE::Test::ResouceHandlerTest::Run(Utilz::IConsoleBackend * backend)
{
	ResourceHandler::IResourceHandler* r = new ResourceHandler::ResourceHandler();

	r->Initialize();
	Utilz::GUID guid = Utilz::GUID("test.txt");
	r->LoadResource(Utilz::GUID("test.txt"), [backend](void* data, size_t size) 
	{
		backend->Print("Content: %s\n", data);
		if (std::string((char*)data) != "1337")
		{
			return false;
		}
	});


	return true;
}
