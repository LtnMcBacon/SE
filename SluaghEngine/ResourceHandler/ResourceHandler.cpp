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

void SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid, const LoadResourceDelegate& callback)
{
	StartProfile;

	auto& find = resourceMap.find(guid);
	if (find == resourceMap.end())
	{
		auto& resourceInfo = resourceMap[guid];
		void* rawData;
		size_t rawSize;
		auto result = diskLoader->LoadResource(guid, &rawData, &rawSize, &resourceInfo.extension);
		if (result)
			Utilz::Console::Print("Could not load resource GUID: %u, Error: %d.\n", guid, result);
		else
		{
			auto& findParser = parsers.find(resourceInfo.extension);
			if (findParser == parsers.end())
			{
				resourceInfo.data = rawData;
				resourceInfo.size = rawSize;
				Utilz::Console::Print("Resource GUID: %u unknown file format Ext: %u. Skipping parsing.\n", guid, resourceInfo.extension);
			}
			else
			{
				result = findParser->second(rawData, rawSize, &resourceInfo.data, &resourceInfo.size);
				if (result)
				{
					resourceInfo.data = rawData;
					resourceInfo.size = rawSize;
					Utilz::Console::Print("Resource GUID: %u, failed to parse, Ext: %u, Error: %d. Skipping parsing.\n", guid, resourceInfo.extension, result);
				}
				else
					delete rawData;



			}
			resourceInfo.refCount++;
			callback(guid, resourceInfo.data, resourceInfo.size);

		}
			
	}
	else
	{
		auto& resourceInfo = resourceMap[guid];	
		resourceInfo.refCount++;
		callback(guid, resourceInfo.data, resourceInfo.size);
	}
	
	StopProfile;
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

void SE::ResourceHandler::ResourceHandler::AddParser(const Utilz::GUID & extGUID, const std::function<int(void*rawData, size_t rawSize, void**parsedData, size_t*parsedSize)>& parserFunction)
{
	parsers[extGUID] = std::move(parserFunction);
}
