#include "ResourceHandlerTest.h"
#include <ResourceHandler\IResourceHandler.h>

#include <string>
#ifdef _DEBUG
#pragma comment(lib, "ResourceHandlerD.lib")
#else
#pragma comment(lib, "ResourceHandler.lib")
#endif

#include <Profiler.h>

using namespace SE::Test;
ResourceHandlerTest::ResourceHandlerTest()
{
}


ResourceHandlerTest::~ResourceHandlerTest()
{
}
static bool result = false;
static bool result2 = false;
int Load(const SE::Utilz::GUID& guid, void* data, size_t size)
{
	std::string r = (char*)data;
	if (r.substr(0, size) == "1337")
	{
		result = true;
	}

	return 0;
}
int Load2(const SE::Utilz::GUID& guid, void* data, size_t size)
{
	std::string r = (char*)data;
	if (r.substr(0, size) == "1337")
	{
		result2 = true;
	}

	return 0;
}
bool SE::Test::ResourceHandlerTest::Run(Utilz::IConsoleBackend * backend)
{
	StartProfile;
	using namespace std::chrono_literals;

	ResourceHandler::IResourceHandler* r = ResourceHandler::CreateResourceHandler();

	r->Initialize();
	Utilz::GUID guid = Utilz::GUID("test.txt");
	result = false;
	auto res = r->LoadResource(Utilz::GUID("test.txt"), ResourceHandler::LoadResourceDelegate::Make<&Load>());
	if (res)
	{
		backend->Print("test.txt could not be loaded.\n");
		return false;
	}
	if (!result)
	{
		backend->Print("String was not 1337\n");
		return false;
	}




	//*******************************
	result = false;
	res = r->LoadResource(Utilz::GUID("test2.txt"), ResourceHandler::LoadResourceDelegate::Make<&Load>(), true);
	if (res)
	{
		backend->Print("test2.txt could not be loaded.\n");
		return false;
	}
	int timeOut = 0;
	while (!result && timeOut < 3) { timeOut++;  std::this_thread::sleep_for(200ms); }
	if (!result)
	{
		backend->Print("Load timed out for test2.txt\n");
		return false;
	}
	


	//*******************************
	result = false;
	res = r->LoadResource(Utilz::GUID("test.txt"), ResourceHandler::LoadResourceDelegate::Make<&Load>());
	if (res)
	{
		backend->Print("test.txt could not be loaded again.\n");
		return false;
	}
	if (!result)
	{
		backend->Print("String was not 1337 again\n");
		return false;
	}


	//*******************************
	result = false;
	res = r->LoadResource(Utilz::GUID("test2.txt"), ResourceHandler::LoadResourceDelegate::Make<&Load>(), true);
	if (res)
	{
		backend->Print("test2.txt could not be loaded again.\n");
		return false;
	}
	timeOut = 0;
	while (!result && timeOut < 3) { timeOut++;  std::this_thread::sleep_for(200ms); }
	if (!result)
	{
		backend->Print("Load timed out for test2 again.txt\n");
		return false;
	}


	//*******************************
	result = false;
	result2 = false;
	res = r->LoadResource(Utilz::GUID("test3.txt"), ResourceHandler::LoadResourceDelegate::Make<&Load>(), true);
	if (res)
	{
		backend->Print("test3.txt could not be loaded.\n");
		return false;
	}
	res = r->LoadResource(Utilz::GUID("test3.txt"), ResourceHandler::LoadResourceDelegate::Make<&Load2>(), true);
	if (res)
	{
		backend->Print("test3.txt could not be loaded again.\n");
		return false;
	}
	timeOut = 0;
	while (!result && !result2 && timeOut < 3) { timeOut++;  std::this_thread::sleep_for(200ms); }
	if (!result)
	{
		backend->Print("Load timed out for test3.txt\n");
		return false;
	}
	r->Shutdown();
	delete r;

	ProfileReturnConst(true);
}
