#include "ResourceHandler.h"
using namespace SE::ResourceHandler;


ResourceHandler::ResourceHandler() : diskLoader(nullptr)
{
}


ResourceHandler::~ResourceHandler()
{
}

void SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid, const std::function<void(void* data, size_t size)>& callback)
{
	ResourceInfo resourceInfo;
	diskLoader->LoadResource(guid, &resourceInfo.data, &resourceInfo.size);
}
