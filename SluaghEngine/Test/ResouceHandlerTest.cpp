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
static bool result = false;
void Load(const SE::Utilz::GUID& guid, void* data, size_t size)
{
	std::string r = (char*)data;
	if (r.substr(0, size) == "1337")
	{
		result = true;
	}
}


bool SE::Test::ResouceHandlerTest::Run(Utilz::IConsoleBackend * backend)
{
	ResourceHandler::IResourceHandler* r = new ResourceHandler::ResourceHandler();

	r->Initialize();
	Utilz::GUID guid = Utilz::GUID("test.txt");
	result = false;
	r->LoadResource(Utilz::GUID("test.txt"), ResourceHandler::LoadResourceDelegate::Make<&Load>());


	return result;
}
