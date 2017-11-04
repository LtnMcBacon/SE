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
			loadJobs.push({ guid, callbacks, loadFlags });
		else
		{

			Data rawData;
			if (!diskLoader->Exist(guid, &rawData.size))
			{
				infoLock.lock();
				auto& ri2 = guidToResourceInfo[guid];
				ri2.state = State::FAIL;
				errors.push_back("Resource does not exist, GUID: " + std::to_string(guid.id));
				infoLock.unlock();
				ProfileReturnConst(-2);
			}

			loadLock.lock();
			auto result = diskLoader->LoadResource(guid, &rawData.data);
			if (result < 0)
			{
				loadLock.unlock();
				infoLock.lock();
				auto& ri2 = guidToResourceInfo[guid];
				ri2.state = State::FAIL;
				errors.push_back("Could not load resource, GUID: " + std::to_string(guid.id) + ", Error: " + std::to_string(result));
				infoLock.unlock();
				ProfileReturnConst(-3);
			}

			Data data;
			if (callbacks.loadCallback)
			{
				auto lresult = callbacks.loadCallback(guid, rawData.data, rawData.size, &data.data, &data.size);
				if (!(lresult & LoadReturn::NO_DELETE))
					operator delete (rawData.data);
				if (lresult & LoadReturn::FAIL)
				{
					loadLock.unlock();
					infoLock.lock();
					auto& ri2 = guidToResourceInfo[guid];

					ri2.state = State::FAIL;
					errors.push_back("Resource failed in LoadCallback, GUID: " + std::to_string(guid.id));
					infoLock.unlock();
					ProfileReturnConst(-4);
				}
			}
			else
			{
				data = rawData;
			}
			loadLock.unlock();
			infoLock.lock();
			auto& ri2 = guidToResourceInfo[guid];

			ri2.state = State::LOADED;
			if (!callbacks.loadCallback)
				ri2.state |= State::RAW;
			if (loadFlags & LoadFlags::LOAD_FOR_VRAM)
			{
				ri2.VRAMdestroyCallback = callbacks.destroyCallback;
				ri2.state |= State::IN_VRAM;
				ri2.VRAMData = data;
			}
			else if (loadFlags & LoadFlags::LOAD_FOR_RAM)
			{
				ri2.RAMdestroyCallback = callbacks.destroyCallback;
				ri2.RAMData = data;
				ri2.state |= State::IN_RAM;
			}

			infoLock.unlock();
			auto iresult = callbacks.invokeCallback(guid, data.data, data.size);
			if (iresult & InvokeReturn::FAIL)
			{
				infoLock.lock();
				auto& ri2 = guidToResourceInfo[guid];
				ri2.state = State::FAIL;
				errors.push_back("Resource failed in InvokeCallback, GUID: " + std::to_string(guid.id));
				infoLock.unlock();
				ProfileReturnConst(-5);
			}
		}
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

using namespace std::chrono_literals;

void SE::ResourceHandler::ResourceHandler::LoadThreadEntry()
{
	while (running)
	{
		while (running && !loadJobs.wasEmpty())
		{
		//	std::this_thread::sleep_for(2s);
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
					ri2.state = State::FAIL;
					errors.push_back("Resource does not exist, GUID: " + std::to_string(job.guid.id));
					infoLock.unlock();
					loadJobs.pop();
					continue;
				}
				loadLock.lock();
				auto result = diskLoader->LoadResource(job.guid, &rawData.data);
				
				if (result < 0)
				{
					loadLock.unlock();
					infoLock.lock();
					auto& ri2 = guidToResourceInfo[job.guid];
					
					ri2.state = State::FAIL;
					errors.push_back("Could not load resource, GUID: " + std::to_string(job.guid.id) + ", Error: " + std::to_string(result));
					infoLock.unlock();
					loadJobs.pop();
					continue;
				}

				Data data;
				if (job.callbacks.loadCallback)
				{
					auto lresult = job.callbacks.loadCallback(job.guid, rawData.data, rawData.size, &data.data, &data.size);
					if (!(lresult & LoadReturn::NO_DELETE))
						operator delete (rawData.data);
					if (lresult & LoadReturn::FAIL)
					{
						loadLock.unlock();
						infoLock.lock();
						auto& ri2 = guidToResourceInfo[job.guid];
						delete rawData.data;
						ri2.state = State::FAIL;
						errors.push_back("Resource failed in LoadCallback, GUID: " + std::to_string(job.guid.id));
						infoLock.unlock();
						loadJobs.pop();
						continue;
					}
				}
				else
				{
					data = rawData;
				}
				loadLock.unlock();
				infoLock.lock();
				auto& ri2 = guidToResourceInfo[job.guid];
				ri2.state = ri2.state ^ State::LOADING | State::LOADED;

				if (job.loadFlags & LoadFlags::LOAD_FOR_VRAM)
				{
					ri2.VRAMdestroyCallback = job.callbacks.destroyCallback;
					ri2.state |= State::IN_VRAM;
					ri2.VRAMData = data;
				}

				if (job.loadFlags & LoadFlags::LOAD_FOR_RAM)
				{
					ri2.RAMdestroyCallback = job.callbacks.destroyCallback;
					ri2.RAMData = data;
					ri2.state |= State::IN_RAM;
				}
				infoLock.unlock();

				invokeJobs.push({ job.guid, job.callbacks.invokeCallback, job.loadFlags });
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
				if (iresult & InvokeReturn::FAIL)
				{
					infoLock.lock();
					auto& ri2 = guidToResourceInfo[job.guid];
					ri2.refVRAM--;
					ri2.refRAM--;
					ri2.state = State::FAIL;					
					errors.push_back("Resource failed in InvokeCallback, GUID: " + std::to_string(job.guid.id));
					infoLock.unlock();
					loadJobs.pop();
					continue;
				}
			}


			invokeJobs.pop();
		}
		std::this_thread::sleep_for(100ms);
	}
}
