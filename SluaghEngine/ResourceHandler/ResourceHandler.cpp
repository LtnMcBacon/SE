#include "ResourceHandler.h"
using namespace SE::ResourceHandler;
#include <Profiler.h>

ResourceHandler::ResourceHandler() : diskLoader(nullptr)
{
}


ResourceHandler::~ResourceHandler()
{
}

int SE::ResourceHandler::ResourceHandler::Initialize()
{
	StartProfile;

	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid, const std::function<void(void* data, size_t size)>& callback)
{
	StartProfile;

	auto find = resourceMap.find(guid);
	if (find == resourceMap.end())
	{
		auto& resourceInfo = resourceMap[guid];
		callback(resourceInfo.data, resourceInfo.size);
	}
	else
	{
		auto& resourceInfo = resourceMap[guid];
		diskLoader->LoadResource(guid, &resourceInfo.data, &resourceInfo.size);
		callback(resourceInfo.data, resourceInfo.size);
	}
	
	StopProfile;
}
