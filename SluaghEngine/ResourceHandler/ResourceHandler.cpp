#include <ResourceHandler.h>
#include <Profiler.h>
#include "RawLoader.h"
#include <Utilz\Console.h>

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif


SE::ResourceHandler::ResourceHandler::ResourceHandler() : diskLoader(nullptr)
{
}


SE::ResourceHandler::ResourceHandler::~ResourceHandler()
{
}

int SE::ResourceHandler::ResourceHandler::Initialize()
{
	StartProfile;
	diskLoader = new RawLoader;
	diskLoader->Initialize();
	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::Shutdown()
{
	for (auto& r : resourceMap)
	{
		delete r.second.data;
	}
	delete diskLoader;
}

int SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid, const LoadResourceDelegate& callback, bool wait)
{
	StartProfile;

	auto& find = resourceMap.find(guid);
	if (find == resourceMap.end())
	{
		auto& resourceInfo = resourceMap[guid];
		auto result = diskLoader->LoadResource(guid, &resourceInfo.data, &resourceInfo.size, &resourceInfo.extension);
		if (result)
		{
			Utilz::Console::Print("Could not load resource GUID: %u, Error: %d.\n", guid, result);
			ProfileReturnConst(result);
		}	
		else
		{
		
			auto ret = callback(guid, resourceInfo.data, resourceInfo.size);
			if (ret)
			{
				Utilz::Console::Print("Error in resource callback, GUID: %u, Error: %d.\n", guid, result);
				ProfileReturnConst( ret);
			}
			resourceInfo.refCount++;
		}
			
	}
	else
	{
		auto& resourceInfo = resourceMap[guid];	
		resourceInfo.refCount++;
		ProfileReturn(callback(guid, resourceInfo.data, resourceInfo.size));
	}

	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::UnloadResource(const Utilz::GUID & guid)
{
	StartProfile;
	auto& find = resourceMap.find(guid);
	if (find == resourceMap.end())
	{
		find->second.refCount--;
	}
	StopProfile;
}
