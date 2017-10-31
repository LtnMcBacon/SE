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
	diskLoader->Initialize();
	_ASSERT(diskLoader);

	running = true;

	loadThread = std::thread(&ResourceHandler::LoadThreadEntry, this);
	invokeThread = std::thread(&ResourceHandler::InvokeThreadEntry, this);

	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::Shutdown()
{

	running = false;
	if(loadThread.joinable())
		loadThread.join();

	if(invokeThread.joinable())
		invokeThread.join();


	for (auto& r : guidToResourceInfo)
	{
		
	}
		
	delete diskLoader;
}

void SE::ResourceHandler::ResourceHandler::UpdateInfo(const InitializationInfo & initInfo)
{
}

int SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid, const LoadResourceDelegate& callback, bool async, Behavior behavior)
{
	StartProfile;
	//loadResourceLock.lock();
	//auto find = guidToResourceInfo.find(guid);
	//auto& index = guidToResourceInfo[guid];

	//if (find == guidToResourceInfo.end()) //If resource is not registered.
	//{

	//	if (!diskLoader->Exist(guid, nullptr)) // Make sure we can load the resource.
	//	{
	//		guidToResourceInfo.erase(guid);
	//		loadResourceLock.unlock();
	//		ProfileReturnConst(-1);
	//	}


	//	// Make sure we have enough memory.
	//	if (resourceInfo.used + 1 > resourceInfo.allocated)
	//		Allocate(resourceInfo.allocated * 2);

	//	index = resourceInfo.used++;
	//	resourceInfo.state[index] = State::Loading;
	//	resourceInfo.refCount[index] = 1;
	//	resourceInfo.resourceData[index].data = nullptr;

	//	if (async) { // We create a load job
	//		toLoad.push({ guid, index, callback, behavior });
	//		loadResourceLock.unlock();
	//		ProfileReturnConst(0);
	//	}
	//	else {
	//		auto ret = LoadSync(guid, index, callback);
	//		loadResourceLock.unlock();
	//		ProfileReturnConst(ret);
	//	}
	//}
	//else // Resource is registered
	//{
	//	resourceInfo.refCount[index]++; // First increase the refcount
	//	if (resourceInfo.state[index] == State::Loaded) // The resource already loaded.
	//	{

	//		if (async) { // Since the load is async we make a load job that will call the callback.			
	//			toInvoke.push({ guid, index, callback, behavior });
	//			loadResourceLock.unlock();
	//			ProfileReturnConst(0);
	//		}
	//		else { // Invoke the callback
	//			auto ret = InvokeCallback(guid, index, callback);
	//			loadResourceLock.unlock();
	//			ProfileReturnConst(ret);
	//		}
	//	}
	//	else if (resourceInfo.state[index] == State::Loading) // Someone has already started loading the resource.
	//	{
	//		// Update the toLoad struct

	//		if (async) { // Update the load job
	//			toInvoke.push({ guid, index, callback, behavior });
	//			loadResourceLock.unlock();
	//			ProfileReturnConst(0);
	//		}
	//		else // Since the load is sync we need to wait until the resource has been loaded.
	//		{
	//			while (resourceInfo.state[index] != State::Loaded) std::this_thread::sleep_for(10ms); // TODO: Maybe change to a condition variable

	//			auto ret = InvokeCallback(guid, index, callback);
	//			loadResourceLock.unlock();
	//			ProfileReturnConst(ret);
	//		}
	//	}
	//	else // The resource is dead, the refCount reached 0 before the load was completed.
	//	{
	//		resourceInfo.state[index] = State::Loading;
	//		resourceInfo.refCount[index] = 1;
	//		if (async) { // We create a load job
	//			toLoad.push({ guid, index, callback, behavior });
	//			loadResourceLock.unlock();
	//			ProfileReturnConst(0);
	//		}
	//		else {
	//			auto ret = LoadSync(guid, index, callback);
	//			loadResourceLock.unlock();
	//			ProfileReturnConst(ret);
	//		}
	//	}
	//}
	//loadResourceLock.unlock();
	return 0;
}

int SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid, 
	const Utilz::Delegate<LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)>& loadCallback,
	const Utilz::Delegate<InvokeReturn1(const Utilz::GUID&, void*, size_t)>& invokeCallback, 
	LoadFlags loadFlags)
{

	if (loadFlags & LoadFlags::ASYNC)
	{
		infoLock.lock();
		auto& ri = guidToResourceInfo[guid];
		ri.refRAM++;
		ri.refVRAM++;
		// If the resource is dead or loading, or if the resource is needed in RAM and it is not in RAM
		if ((ri.state & (State::DEAD | State::LOADING)) || ((loadFlags & LoadFlags::LOAD_FOR_RAM) && !(ri.state & State::IN_RAM)))
		{
			auto temp = State::LOADING;
			if (ri.state & State::IN_VRAM)
				temp |= State::IN_VRAM;
			ri.state = temp;
			infoLock.unlock();
			loadJobs.push({ guid, loadCallback, invokeCallback, loadFlags });
		}
		else
		{
			infoLock.unlock();
			invokeJobs.push({ guid, invokeCallback, loadFlags });
		}
	}
	else
	{
		infoLock.lock();
		auto& ri = guidToResourceInfo[guid];
		ri.refRAM++;
		ri.refVRAM++;
		if ((ri.state & (State::DEAD | State::LOADING)) || ((loadFlags & LoadFlags::LOAD_FOR_RAM) && !(ri.state & State::IN_RAM)))
		{
			auto temp = State::LOADING;
			if (ri.state & State::IN_VRAM)
				temp |= State::IN_VRAM;
			ri.state = temp;
			infoLock.unlock();
		
		}
		else
		{
			infoLock.unlock();
			
		}

	}

	return 0;
}

void SE::ResourceHandler::ResourceHandler::UnloadResource(const Utilz::GUID & guid)
{
	StartProfile;

	//auto& find = guidToResourceInfo.find(guid);
	//if (find == guidToResourceInfo.end())
	//{
	//	infoLock.lock();
	//	resourceInfo.refCount[find->second]--;
	//	if (resourceInfo.refCount[find->second] == 0 && resourceInfo.state[find->second] == State::Loading) // If no one want the resource any more and isn't loaded yet.
	//		resourceInfo.state[find->second] = State::Dead;
	//	infoLock.unlock();
	//}
	//
	StopProfile;
}

void SE::ResourceHandler::ResourceHandler::LinearUnload(size_t addedSize)
{
	StartProfile;
	/*if (Utilz::Memory::IsUnderLimit(initInfo.maxMemory, addedSize))
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
	}*/
	
	ProfileReturnVoid;
}

int SE::ResourceHandler::ResourceHandler::LoadSync(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback)
{

	//StartProfile;
	//Data data;

	//diskLoader->Exist(guid, &data.size);

	//(*this.*Unload)(data.size);

	//auto result = diskLoader->LoadResource(guid, &data.data);
	//if (result)
	//	ProfileReturnConst(result);
	//	
	//infoLock.lock();
	//resourceInfo.resourceData[index] = data;
	////resourceInfo.state[index] = State::Loaded;
	//infoLock.unlock();
	//ProfileReturn(InvokeCallback(guid, index, callback));
	return 0;
}

int SE::ResourceHandler::ResourceHandler::InvokeCallback(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback)
{
	StartProfile;

	/*auto result = callback(guid, resourceInfo.resourceData[index].data, resourceInfo.resourceData[index].size);

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
	}*/
	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::LoadThreadEntry()
{
	while (running)
	{
		while (running && !loadJobs.wasEmpty())
		{
			const auto& job = loadJobs.top();

			infoLock.lock();
			auto ri = guidToResourceInfo[job.guid];
			infoLock.unlock();
			if (ri.state & State::DEAD)
			{
			
				loadJobs.pop();				
				continue;
			}

			if (ri.state & State::LOADING)
			{
				Data rawData;
				if (!diskLoader->Exist(job.guid, &rawData.size))
				{
					infoLock.lock();
					auto& ri2 = guidToResourceInfo[job.guid];
					ri2.refVRAM--;
					ri2.refRAM--;
					ri2.state = State::FAIL;
					infoLock.unlock();
					errors.push_back("Resource does not exist, GUID: " + std::to_string(job.guid.id));
					loadJobs.pop();
					continue;
				}

				auto result = diskLoader->LoadResource(job.guid, &rawData.data);
				if (result < 0)
				{
					infoLock.lock();
					auto& ri2 = guidToResourceInfo[job.guid];
					ri2.refVRAM--;
					ri2.refRAM--;
					ri2.state = State::FAIL;
					infoLock.unlock();
					errors.push_back("Could not load resource, GUID: " + std::to_string(job.guid.id) + ", Error: " + std::to_string(result));
					loadJobs.pop();
					continue;
				}

				Data data;
				auto lresult = job.loadCallback(job.guid, rawData.data, rawData.size, &data.data, &data.size);

				if (lresult & LoadReturn::FAIL)
				{
					infoLock.lock();
					auto& ri2 = guidToResourceInfo[job.guid];
					ri2.refVRAM--;
					ri2.refRAM--;
					ri2.state = State::FAIL;
					infoLock.unlock();
					errors.push_back("Resource failed in LoadCallback, GUID: " + std::to_string(job.guid.id));
					loadJobs.pop();
					continue;
				}
				
			
				infoLock.lock();
				auto& ri2 = guidToResourceInfo[job.guid];
				ri2.RAMData = ri.RAMData;
				ri2.VRAMData = ri.VRAMData;

				ri2.state = State::LOADED;
				if (lresult & LoadReturn::VRAM)
				{
					ri2.state |= State::IN_VRAM;
					ri2.VRAMData = data;
				}
				else
					ri2.refVRAM--;
				if (lresult & LoadReturn::RAM)
				{
					ri2.RAMData = data;
					ri2.state |= State::IN_RAM;
				}
				else
					ri2.refRAM--;
				infoLock.unlock();

				invokeJobs.push({ job.guid, job.invokeCallback, job.loadFlags });
			}

			
			loadJobs.pop();
		}
		std::this_thread::sleep_for(100ms);
	}
}

void SE::ResourceHandler::ResourceHandler::InvokeThreadEntry()
{
	while (running)
	{
		while (running && !invokeJobs.wasEmpty())
		{
			const auto& job = invokeJobs.top();
			infoLock.lock();
			auto ri = guidToResourceInfo[job.guid];
			infoLock.unlock();
			if (ri.state & State::LOADED)
			{
				Data data;
				if (ri.state & State::IN_RAM)
					data = ri.RAMData;
				else if (ri.state & State::IN_VRAM)
					data = ri.VRAMData;
				auto iresult = job.invokeCallback(job.guid, data.data, data.size);
				if (iresult & InvokeReturn1::FAIL)
				{
					infoLock.lock();
					auto& ri2 = guidToResourceInfo[job.guid];
					ri2.refVRAM--;
					ri2.refRAM--;
					ri2.state = State::FAIL;
					infoLock.unlock();
					errors.push_back("Resource failed in LoadCallback, GUID: " + std::to_string(job.guid.id));
					loadJobs.pop();
					continue;
				}
			}


			invokeJobs.pop();
		}
		std::this_thread::sleep_for(100ms);
	}
}
