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

	load_threadPool = new Utilz::ThreadPool(1);
	invoke_threadPool = new Utilz::ThreadPool(2);

	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::Shutdown()
{
	delete load_threadPool;
	delete invoke_threadPool;

	for (auto& r : guidToResourceInfo)
	{
		if (r.second.state & State::RAW)
			operator delete(r.second.RAMData.data);
		if (r.second.state & State::IN_RAM)
			if(r.second.RAMdestroyCallback)
				r.second.RAMdestroyCallback(r.first, r.second.RAMData.data, r.second.RAMData.size);
		if (r.second.state & State::IN_VRAM)
			if (r.second.VRAMdestroyCallback)
				r.second.VRAMdestroyCallback(r.first, r.second.VRAMData.data, r.second.VRAMData.size);
	}
		
	delete diskLoader;
}

const SE::ResourceHandler::InitializationInfo & SE::ResourceHandler::ResourceHandler::GetInfo() const
{
	return initInfo;
}

void SE::ResourceHandler::ResourceHandler::UpdateInfo(const InitializationInfo & initInfo)
{
	this->initInfo = initInfo;
}

int SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid,
	const std::function<InvokeReturn(const Utilz::GUID&, void*, size_t)>& invokeCallback, 
	LoadFlags loadFlags)
{
	Callbacks callbacks;
	callbacks.invokeCallback = invokeCallback;
	return LoadResource(guid, callbacks, loadFlags);
}


int SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid,
	const Callbacks& callbacks,
	LoadFlags loadFlags)
{
	StartProfile;
	_ASSERT_EXPR(callbacks.invokeCallback, "An invokeCallback must be provided");

	infoLock.lock();
	auto& ri = guidToResourceInfo[guid];

	if (ri.state & State::FAIL)
	{
		infoLock.unlock();
		ProfileReturnConst( -1);
	}
		
	if(loadFlags & LoadFlags::LOAD_FOR_VRAM)
		ri.refVRAM++;
	if (loadFlags & LoadFlags::LOAD_FOR_RAM)
		ri.refRAM++;

	bool load = false;
	if (ri.state & State::LOADING)
		load = true;
	else if ((loadFlags & LoadFlags::LOAD_FOR_RAM) && !(ri.state & State::IN_RAM))
		load = true;
	else if ((loadFlags & LoadFlags::LOAD_FOR_VRAM) && !(ri.state & State::IN_VRAM))
		load = true;
	if (load)
	{
		ri.state = ri.state ^ State::DEAD | State::LOADING;
		if (loadFlags & LoadFlags::IMMUTABLE)
			ri.state |= State::IMMUTABLE;
		infoLock.unlock();
		if (loadFlags & LoadFlags::ASYNC)
			load_threadPool->Enqueue(this, &ResourceHandler::Load, { guid, callbacks, loadFlags });
		else
			Load({ guid, callbacks, loadFlags });
	}
	else // If the resource is loaded we just skip async.
	{

		Data data;
		if (ri.state & State::IN_RAM)
			data = ri.RAMData;
		else if (ri.state & State::IN_VRAM)
			data = ri.VRAMData;
		infoLock.unlock();

		auto iresult = callbacks.invokeCallback(guid, data.data, data.size);
		if (iresult & InvokeReturn::FAIL)
		{
			infoLock.lock();
			auto& ri2 = guidToResourceInfo[guid];
			ri2.state = State::FAIL;
			errors.push_back("Resource failed in InvokeCallback, GUID: " + std::to_string(guid.id));
			infoLock.unlock();
			ProfileReturnConst(-6);
		}

	}
	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::UnloadResource(const Utilz::GUID & guid, UnloadFlags unloadFlags)
{
	StartProfile;
	infoLock.lock();
	auto& ri = guidToResourceInfo[guid];
	if (unloadFlags & UnloadFlags::RAM)
		if(ri.refRAM > 0)
			ri.refRAM--;
	if (unloadFlags & UnloadFlags::VRAM)
		if(ri.refVRAM > 0)
			ri.refVRAM--;
	if (ri.refRAM == 0 && ri.refVRAM == 0)
		ri.state = ri.state ^ State::LOADED | State::DEAD;
	infoLock.unlock();
	StopProfile;
}

bool SE::ResourceHandler::ResourceHandler::Load(LoadJob job)
{
	// Do some checks
	{
		infoLock.lock();
		auto ri = guidToResourceInfo[job.guid];
		infoLock.unlock();
		if (ri.state & State::DEAD)
		{
			return false;
		}

		if (!(ri.state & State::LOADING))
			return false;
	}


	// Load the resource
	Data data;
	{
		Data rawData;
		if (!diskLoader->Exist(job.guid, &rawData.size))
		{
			infoLock.lock();
			auto& ri = guidToResourceInfo[job.guid];
			ri.state = State::FAIL;
			errors.push_back("Resource does not exist, GUID: " + std::to_string(job.guid.id));
			infoLock.unlock();
			return false;
		}
		loadLock.lock();
		auto result = diskLoader->LoadResource(job.guid, &rawData.data);

		if (result < 0)
		{
			loadLock.unlock();
			infoLock.lock();
			auto& ri = guidToResourceInfo[job.guid];

			ri.state = State::FAIL;
			errors.push_back("Could not load resource, GUID: " + std::to_string(job.guid.id) + ", Error: " + std::to_string(result));
			infoLock.unlock();
			return false;
		}


		if (job.callbacks.loadCallback)
		{
			auto lresult = job.callbacks.loadCallback(job.guid, rawData.data, rawData.size, &data.data, &data.size);
			loadLock.unlock();
			infoLock.lock();
			auto& ri = guidToResourceInfo[job.guid];
			if (lresult & LoadReturn::NO_DELETE)
				ri.state |= State::RAW;
			else
				operator delete (rawData.data);
			if (lresult & LoadReturn::FAIL)
			{

				ri.state = State::FAIL;
				errors.push_back("Resource failed in LoadCallback, GUID: " + std::to_string(job.guid.id));
				infoLock.unlock();
				return false;
			}
			infoLock.unlock();
		}
		else
		{
			loadLock.unlock();
			data = rawData;
		}
	}
	{
		infoLock.lock();
		auto& ri = guidToResourceInfo[job.guid];
		ri.state = ri.state ^ State::LOADING | State::LOADED;
		if (!job.callbacks.destroyCallback)
			ri.state |= State::RAW;
		if (job.loadFlags & LoadFlags::LOAD_FOR_VRAM)
		{
			ri.VRAMdestroyCallback = job.callbacks.destroyCallback;
			ri.state |= State::IN_VRAM;
			ri.VRAMData = data;
		}

		if (job.loadFlags & LoadFlags::LOAD_FOR_RAM)
		{
			ri.RAMdestroyCallback = job.callbacks.destroyCallback;
			ri.RAMData = data;
			ri.state |= State::IN_RAM;
		}
		infoLock.unlock();
	}

	// Invoke the invoke callback
	{

		auto iresult = job.callbacks.invokeCallback(job.guid, data.data, data.size);
		infoLock.lock();
		auto& ri= guidToResourceInfo[job.guid];

		if (iresult & InvokeReturn::FAIL)
		{
			if(job.loadFlags & LoadFlags::LOAD_FOR_RAM)
				ri.refRAM--;
			if (job.loadFlags & LoadFlags::LOAD_FOR_VRAM)
				ri.refVRAM--;
			ri.state = State::FAIL;
			errors.push_back("Resource failed in InvokeCallback, GUID: " + std::to_string(job.guid.id));
			infoLock.unlock();
			return false;
		}
		if (iresult & InvokeReturn::DEC_RAM)
			ri.refRAM--;
		if (iresult & InvokeReturn::DEC_VRAM)
			ri.refVRAM--;
		infoLock.unlock();
	}
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
