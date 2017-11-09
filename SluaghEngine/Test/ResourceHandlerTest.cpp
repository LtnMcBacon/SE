#include "ResourceHandlerTest.h"
#include <ResourceHandler\IResourceHandler.h>
#include <Utilz\Memory.h>

#include <string>
#ifdef _DEBUG
#pragma comment(lib, "ResourceHandlerD.lib")
#else
#pragma comment(lib, "ResourceHandler.lib")
#endif

#include <Profiler.h>

using namespace SE::Test;
using namespace SE::Utilz::Memory;

ResourceHandlerTest::ResourceHandlerTest()
{
}


ResourceHandlerTest::~ResourceHandlerTest()
{
}
static bool result = false;
static bool result2 = false;
SE::ResourceHandler::InvokeReturn Load(const SE::Utilz::GUID& guid, void* data, size_t size)
{
	std::string r = (char*)data;
	if (r.substr(0, size) == "1337")
	{
		result = true;
	}

	return SE::ResourceHandler::InvokeReturn::SUCCESS | SE::ResourceHandler::InvokeReturn::DEC_RAM;
}
SE::ResourceHandler::InvokeReturn Load2(const SE::Utilz::GUID& guid, void* data, size_t size)
{
	std::string r = (char*)data;
	if (r.substr(0, size) == "1337")
	{
		result2 = true;
	}

	return SE::ResourceHandler::InvokeReturn::SUCCESS | SE::ResourceHandler::InvokeReturn::DEC_RAM;
}
bool SE::Test::ResourceHandlerTest::Run(DevConsole::IConsole * backend)
{
	StartProfile;
	using namespace std::chrono_literals;

	ResourceHandler::IResourceHandler* r = ResourceHandler::CreateResourceHandler();

	r->Initialize({10_mb});

	r->LoadResource("texture8.sei", [](auto guid, auto data, auto size) {
		std::this_thread::sleep_for(100ms);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});

	if (Utilz::Memory::IsUnderLimit(10_mb))
	{
		r->Shutdown(); delete r;
		return false;
	}
	Utilz::GUID guid = Utilz::GUID("test.txt");
	result = false;
	auto res = r->LoadResource(Utilz::GUID("test.txt"), &Load);
	if (res)
	{
		backend->Print("test.txt could not be loaded.\n");
		r->Shutdown(); delete r;
		return false;
	}
	if (!result)
	{
		backend->Print("String was not 1337\n");
		r->Shutdown(); delete r;
		return false;
	}




	//*******************************
	result = false;
	res = r->LoadResource(Utilz::GUID("test2.txt"),&Load);
	if (res)
	{
		backend->Print("test2.txt could not be loaded.\n");
		r->Shutdown(); delete r;
		return false;
	}
	int timeOut = 0;
	while (!result && timeOut < 3) { timeOut++;  std::this_thread::sleep_for(200ms); }
	if (!result)
	{
		backend->Print("Load timed out for test2.txt\n");
		r->Shutdown(); delete r;
		return false;
	}
	


	//*******************************
	result = false;
	res = r->LoadResource(Utilz::GUID("test.txt"), &Load);
	if (res)
	{
		backend->Print("test.txt could not be loaded again.\n");
		r->Shutdown(); delete r;
		return false;
	}
	if (!result)
	{
		backend->Print("String was not 1337 again\n");
		r->Shutdown(); delete r;
		return false;
	}


	//*******************************
	result = false;
	res = r->LoadResource(Utilz::GUID("test2.txt"), &Load);
	if (res)
	{
		backend->Print("test2.txt could not be loaded again.\n");
		r->Shutdown(); delete r;
		return false;
	}
	timeOut = 0;
	while (!result && timeOut < 3) { timeOut++;  std::this_thread::sleep_for(200ms); }
	if (!result)
	{
		backend->Print("Load timed out for test2 again.txt\n");
		r->Shutdown(); delete r;
		return false;
	}


	//*******************************
	result = false;
	result2 = false;
	res = r->LoadResource(Utilz::GUID("test3.txt"), &Load);
	if (res)
	{
		backend->Print("test3.txt could not be loaded.\n");
		r->Shutdown(); delete r;
		return false;
	}
	res = r->LoadResource(Utilz::GUID("test3.txt"), &Load2);
	if (res)
	{
		backend->Print("test3.txt could not be loaded again.\n");
		r->Shutdown(); delete r;
		return false;
	}
	timeOut = 0;
	while (!result && !result2 && timeOut < 3) { timeOut++;  std::this_thread::sleep_for(200ms); }
	if (!result)
	{
		backend->Print("Load timed out for test3.txt\n");
		r->Shutdown(); delete r;
		return false;
	}


	//*******************************

	bool tt = false;
	bool tt2 = false;
	res = r->LoadResource("test4.txt", [r, &tt, &tt2](auto guid, auto data, auto size)
	{
		auto re = r->LoadResource("test.txt", [&tt](auto guid, auto data, auto size) {
			tt = true;
			return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
		});

		if (re)
			return ResourceHandler::InvokeReturn::FAIL;

		re = r->LoadResource("test5.txt", [&tt2](auto guid, auto data, auto size) {
			tt2 = true;
			return ResourceHandler::InvokeReturn::SUCCESS| ResourceHandler::InvokeReturn::DEC_RAM;
		});
		if (re)
			return ResourceHandler::InvokeReturn::FAIL;
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});


	timeOut = 0;
	while (!tt2 && timeOut < 3) { timeOut++;  std::this_thread::sleep_for(200ms); }
	if (!result)
	{
		backend->Print("Load timed out for test5.txt\n");
		r->Shutdown(); delete r;
		return false;
	}



	if(res || !tt || !tt2)
	{
		backend->Print("Recursive load failed.\n");
		r->Shutdown(); delete r;
		return false;
	}



	if (!Utilz::Memory::IsUnderLimit(10_mb))
	{

		r->Shutdown(); delete r;
		return false;
	}
	r->Shutdown(); delete r;

	ProfileReturnConst(true);
}
