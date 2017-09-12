#include <ResourceHandler.h>
#include <Profiler.h>
#include "RawLoader.h"

using namespace SE::ResourceHandler;
ResourceHandler::ResourceHandler() : diskLoader(nullptr)
{
}


ResourceHandler::~ResourceHandler()
{
}

int SE::ResourceHandler::ResourceHandler::Initialize()
{
	StartProfile;
	diskLoader = new RawLoader;
	diskLoader->Initialize();
	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid, const std::function<void(void* data, size_t size)>& callback)
{
	StartProfile;

	auto find = resourceMap.find(guid);
	if (find == resourceMap.end())
	{
		auto& resourceInfo = resourceMap[guid];
		diskLoader->LoadResource(guid, &resourceInfo.data, &resourceInfo.size);
		callback(resourceInfo.data, resourceInfo.size);
	}
	else
	{
		auto& resourceInfo = resourceMap[guid];	
		callback(resourceInfo.data, resourceInfo.size);
	}
	
	StopProfile;
}

void SE::ResourceHandler::ResourceHandler::UnloadResource(const Utilz::GUID & guid)
{
}

void SE::ResourceHandler::ResourceHandler::AddParser(const Utilz::GUID & extGUID, const std::function<int(void*rawData, size_t rawSize, void**parsedData, size_t*parsedSize)>& parserFunction)
{
}
