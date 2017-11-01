#include "ResourceHandler.h"
#include <Profiler.h>
#include "RawLoader.h"
#include <vector>
#include <Utilz\Memory.h>

using namespace std::chrono_literals;

SE::ResourceHandler::ResourceHandler::ResourceHandler() : diskLoader(nullptr)
{
	
}


SE::ResourceHandler::ResourceHandler::~ResourceHandler()
{
}

int SE::ResourceHandler::ResourceHandler::Initialize(const InitializationInfo& initInfo)
{
	StartProfile;
	this->initInfo = initInfo;

	switch (initInfo.unloadingStrat)
	{
	case UnloadingStrategy::Linear:
		Unload = &ResourceHandler::LinearUnload;
		break;
	default:
		Unload = &ResourceHandler::LinearUnload;
		break;
	}

	diskLoader = new RawLoader;
	auto res = diskLoader->Initialize();
	if (res < 0)
		return res;
	_ASSERT(diskLoader);

	Allocate(128);

	running = true;

	toLoadThread = std::thread(&ResourceHandler::ToLoadThreadEntry, this);
	toInvokeThread = std::thread(&ResourceHandler::ToInvokeThreadEntry, this);

	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::Shutdown()
{

	running = false;
	if(toLoadThread.joinable())
		toLoadThread.join();

	if(toInvokeThread.joinable())
		toInvokeThread.join();


	for (size_t i = 0; i < resourceInfo.used; i++)
	{
		operator delete(resourceInfo.resourceData[i].data);
	}
	operator delete(resourceInfo.data);
	delete diskLoader;
}

void SE::ResourceHandler::ResourceHandler::UpdateInfo(const InitializationInfo & initInfo)
{
}

int SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid, const LoadResourceDelegate& callback, bool async, Behavior behavior)
{
	StartProfile;
	loadResourceLock.lock();
	auto find = guidToResourceInfoIndex.find(guid);
	auto& index = guidToResourceInfoIndex[guid];

	if (find == guidToResourceInfoIndex.end()) //If resource is not registered.
	{

		if (!diskLoader->Exist(guid, nullptr)) // Make sure we can load the resource.
		{
			guidToResourceInfoIndex.erase(guid);
			loadResourceLock.unlock();
			ProfileReturnConst(-1);
		}


		// Make sure we have enough memory.
		if (resourceInfo.used + 1 > resourceInfo.allocated)
			Allocate(resourceInfo.allocated * 2);

		index = resourceInfo.used++;
		resourceInfo.state[index] = State::Loading;
		resourceInfo.refCount[index] = 1;
		resourceInfo.resourceData[index].data = nullptr;

		if (async) { // We create a load job
			toLoad.push({ guid, index, callback, behavior });
			loadResourceLock.unlock();
			ProfileReturnConst(0);
		}
		else {
			auto ret = LoadSync(guid, index, callback);
			loadResourceLock.unlock();
			ProfileReturnConst(ret);
		}
	}
	else // Resource is registered
	{
		resourceInfo.refCount[index]++; // First increase the refcount
		if (resourceInfo.state[index] == State::Loaded) // The resource already loaded.
		{

			if (async) { // Since the load is async we make a load job that will call the callback.			
				toInvoke.push({ guid, index, callback, behavior });
				loadResourceLock.unlock();
				ProfileReturnConst(0);
			}
			else { // Invoke the callback
				auto ret = InvokeCallback(guid, index, callback);
				loadResourceLock.unlock();
				ProfileReturnConst(ret);
			}
		}
		else if (resourceInfo.state[index] == State::Loading) // Someone has already started loading the resource.
		{
			// Update the toLoad struct

			if (async) { // Update the load job
				toInvoke.push({ guid, index, callback, behavior });
				loadResourceLock.unlock();
				ProfileReturnConst(0);
			}
			else // Since the load is sync we need to wait until the resource has been loaded.
			{
				while (resourceInfo.state[index] != State::Loaded) std::this_thread::sleep_for(10ms); // TODO: Maybe change to a condition variable

				auto ret = InvokeCallback(guid, index, callback);
				loadResourceLock.unlock();
				ProfileReturnConst(ret);
			}
		}
		else // The resource is dead, the refCount reached 0 before the load was completed.
		{
			resourceInfo.state[index] = State::Loading;
			resourceInfo.refCount[index] = 1;
			if (async) { // We create a load job
				toLoad.push({ guid, index, callback, behavior });
				loadResourceLock.unlock();
				ProfileReturnConst(0);
			}
			else {
				auto ret = LoadSync(guid, index, callback);
				loadResourceLock.unlock();
				ProfileReturnConst(ret);
			}
		}
	}
	loadResourceLock.unlock();
	StopProfile;
}

void SE::ResourceHandler::ResourceHandler::UnloadResource(const Utilz::GUID & guid)
{
	StartProfile;

	auto& find = guidToResourceInfoIndex.find(guid);
	if (find == guidToResourceInfoIndex.end())
	{
		infoLock.lock();
		resourceInfo.refCount[find->second]--;
		if (resourceInfo.refCount[find->second] == 0 && resourceInfo.state[find->second] == State::Loading) // If no one want the resource any more and isn't loaded yet.
			resourceInfo.state[find->second] = State::Dead;
		infoLock.unlock();
	}
	
	StopProfile;
}

void SE::ResourceHandler::ResourceHandler::LinearUnload(size_t addedSize)
{
	StartProfile;
	if (Utilz::Memory::IsUnderLimit(initInfo.maxMemory, addedSize))
		ProfileReturnVoid;

	size_t freed = 0;
	std::vector<size_t> toFree;
	for (size_t i = 0; i < resourceInfo.used; i++)
	{
	
		if (resourceInfo.state[i] == State::Loaded && resourceInfo.refCount[i] == 0)
		{
			freed += resourceInfo.resourceData[i].size;
			toFree.push_back(i);
			if (freed >= addedSize)
				break;
		}
	
	}
	if (Utilz::Memory::IsUnderLimit(initInfo.maxMemory + freed, addedSize))
	{
		infoLock.lock();
		for (auto& i : toFree)
		{
			if (resourceInfo.state[i] == State::Loaded && resourceInfo.refCount[i] == 0)
			{
				operator delete(resourceInfo.resourceData[i].data);
				resourceInfo.resourceData[i].data = nullptr;
				resourceInfo.state[i] = State::Dead;

			}

		}
		infoLock.unlock();
	}
	
	ProfileReturnVoid;
}


void SE::ResourceHandler::ResourceHandler::Allocate(size_t size)
{
	StartProfile;
	_ASSERT(size > resourceInfo.allocated);

	// Allocate new memory
	ResourceInfo newData;
	newData.allocated = size;
	newData.data = operator new(size * ResourceInfo::size);
	newData.used = resourceInfo.used;

	// Setup the new pointers
	newData.resourceData = static_cast<Data*>(newData.data);
	newData.refCount = reinterpret_cast<uint16_t*>(newData.resourceData + newData.allocated);
	newData.state = reinterpret_cast<State*>(newData.refCount + newData.allocated);


	infoLock.lock();
	// Copy data
	memcpy(newData.resourceData, resourceInfo.resourceData, resourceInfo.used * sizeof(Data));
	memcpy(newData.refCount, resourceInfo.refCount, resourceInfo.used * sizeof(uint16_t));
	memcpy(newData.state, resourceInfo.state, resourceInfo.used * sizeof(State));

	// Delete old data;
	operator delete(resourceInfo.data);
	resourceInfo = newData;
	infoLock.unlock();
	StopProfile;
}

void SE::ResourceHandler::ResourceHandler::Destroy(size_t index)
{
}

int SE::ResourceHandler::ResourceHandler::LoadSync(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback)
{
	StartProfile;
	Data data;

	diskLoader->Exist(guid, &data.size);

	(*this.*Unload)(data.size);

	auto result = diskLoader->LoadResource(guid, &data.data);
	if (result)
		ProfileReturnConst(result);
		
	infoLock.lock();
	resourceInfo.resourceData[index] = data;
	resourceInfo.state[index] = State::Loaded;
	infoLock.unlock();
	ProfileReturn(InvokeCallback(guid, index, callback));
}

int SE::ResourceHandler::ResourceHandler::InvokeCallback(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback)
{
	StartProfile;

	auto result = callback(guid, resourceInfo.resourceData[index].data, resourceInfo.resourceData[index].size);

	switch (result)
	{
	case SE::ResourceHandler::InvokeReturn::Fail:
		ProfileReturnConst(-1);
		break;
	case SE::ResourceHandler::InvokeReturn::DecreaseRefcount:
		infoLock.lock();
		resourceInfo.refCount[index]--;
		infoLock.unlock();
		break;
	}
	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::ToLoadThreadEntry()
{
	while (running)
	{
		while (running && !toLoad.wasEmpty())
		{
			const auto& job = toLoad.top();

			infoLock.lock();
			auto state = resourceInfo.state[job.resourceInfoIndex];
			infoLock.unlock();

			if (state == State::Loading) // Resource is not loaded yet
			{
				Data data;

				if (diskLoader->Exist(job.guid, &data.size))
				{
					(*this.*Unload)(data.size);
					auto result = diskLoader->LoadResource(job.guid, &data.data);
					if (!result)
					{
						infoLock.lock();
						resourceInfo.resourceData[job.resourceInfoIndex] = data;
						resourceInfo.state[job.resourceInfoIndex] = State::Loaded;
						infoLock.unlock();
						job.callback;
						toInvoke.push(job);
					}
				}					
			}

			toLoad.pop();
		}
		std::this_thread::sleep_for(100ms);
	}
}

void SE::ResourceHandler::ResourceHandler::ToInvokeThreadEntry()
{
	while (running)
	{
		while (running && !toInvoke.wasEmpty())
		{
			const auto& job = toInvoke.top();

			infoLock.lock();
			auto state = resourceInfo.state[job.resourceInfoIndex];
			infoLock.unlock();


			switch (state)
			{
			case SE::ResourceHandler::ResourceHandler::State::Loaded:
				InvokeCallback(job.guid, job.resourceInfoIndex, job.callback);
				toInvoke.pop();
				break;
			case SE::ResourceHandler::ResourceHandler::State::Loading:
				std::this_thread::sleep_for(10ms);
				break;
			case SE::ResourceHandler::ResourceHandler::State::Dead:
				toInvoke.pop();
				break;
			default:
				break;
			}
		}
		std::this_thread::sleep_for(100ms);
	}
}
