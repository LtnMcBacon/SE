#include "ResourceHandler.h"
#include <Profiler.h>
#include "RawLoader.h"
#include <vector>

using namespace std::chrono_literals;

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
	_ASSERT(diskLoader);

	Allocate(128);

	running = true;
	myThread = std::thread(&ResourceHandler::Run, this);

	ProfileReturnConst(0);
}

void SE::ResourceHandler::ResourceHandler::Shutdown()
{

	running = false;
	if(myThread.joinable())
		myThread.join();


	for (size_t i = 0; i < resourceInfo.used; i++)
	{
		if(resourceInfo.resourceData[i].data)
			operator delete(resourceInfo.resourceData[i].data);
	}
	operator delete(resourceInfo.data);
	delete diskLoader;
}

int SE::ResourceHandler::ResourceHandler::LoadResource(const Utilz::GUID & guid, const LoadResourceDelegate& callback, bool async, Behavior behavior)
{
	StartProfile;

	auto& find = guidToResourceInfoIndex.find(guid);
	auto& index = guidToResourceInfoIndex[guid];

	if (find == guidToResourceInfoIndex.end()) //If resource is not registered.
	{

		if (!diskLoader->Exist(guid)) // Make sure we can load the resource.
		{
			//Utilz::Console::Print("Resource %u could not be found!\n", guid);
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
			CreateLoadJob(guid, index, callback, behavior);
			ProfileReturnConst(0);
		}
		else {
			ProfileReturn(LoadSync(guid, index, callback));
		}
	}
	else // Resource is registered
	{
		resourceInfo.refCount[index]++; // First increase the refcount
		if (resourceInfo.state[index] == State::Loaded) // The resource already loaded.
		{

			if (async) { // Since the load is async we make a load job that will call the callback.			
				CreateLoadJob(guid, index, callback, behavior);
				ProfileReturnConst(0);
			}
			else { // Invoke the callback
				ProfileReturn(InvokeCallback(guid, index, callback));
			}
		}
		else if (resourceInfo.state[index] == State::Loading) // Someone has already started loading the resource.
		{
			// Update the toLoad struct

			if (async) { // Update the load job
				CreateLoadJob(guid, index, callback, behavior);
				ProfileReturnConst(0);
			}
			else // Since the load is sync we need to wait until the resource has been loaded.
			{
				while (resourceInfo.state[index] != State::Loaded) std::this_thread::sleep_for(10ms); // TODO: Maybe change to a condition variable
				ProfileReturn(InvokeCallback(guid, index, callback));
			}
		}
		else // The resource is dead, the refCount reached 0 before the load was completed.
		{
			if (async) { // We create a load job
				CreateLoadJob(guid, index, callback, behavior);
				ProfileReturnConst(0);
			}
			else {
				ProfileReturn(LoadSync(guid, index, callback));
			}
		}
	}
	StopProfile;
}

void SE::ResourceHandler::ResourceHandler::UnloadResource(const Utilz::GUID & guid)
{
	StartProfile;
	infoLock.lock();
	auto& find = guidToResourceInfoIndex.find(guid);
	if (find == guidToResourceInfoIndex.end())
	{
		resourceInfo.refCount[find->second]--;
		if (resourceInfo.refCount[find->second] == 0 && resourceInfo.state[find->second] == State::Loading) // If no one want the resource any more and isn't loaded yet.
			RemoveLoadJob(guid); // Remove the load job.

	}
	infoLock.unlock();
	StopProfile;
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
	newData.refCount = (uint16_t*)(newData.resourceData + newData.allocated);
	newData.state = (State*)(newData.refCount + newData.allocated);


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

void SE::ResourceHandler::ResourceHandler::Run()
{
	StartProfile;

	while (running)
	{		
			LoadAsync();
			std::this_thread::sleep_for(66ms);
	}
	StopProfile;
}

void SE::ResourceHandler::ResourceHandler::LoadAsync()
{

	while (toLoad.size()) // Do we need to load anything.
	{
		StartProfile;
		toLoadLock.lock();
		auto job = toLoad.top();
		toLoad.pop();
		toLoadLock.unlock();

		infoLock.lock();
		auto state = resourceInfo.state[job.resourceInfoIndex];		
		infoLock.unlock();

		if (state == State::Loading) // Resource is not loaded yet
		{
			Data data;
			auto result = diskLoader->LoadResource(job.guid, &data.data, &data.size); // TODO: Fail check
			if (result != 0)
				throw std::exception("EnumAdapters failed in MemoryMeasuring.");


			infoLock.lock();
			resourceInfo.resourceData[job.resourceInfoIndex] = data;
			resourceInfo.state[job.resourceInfoIndex] = State::Loaded;
			infoLock.unlock();


			auto ret = job.callback(job.guid, data.data, data.size);
			if (ret == 1)
			{
				infoLock.lock();
				resourceInfo.refCount[job.resourceInfoIndex]--;
				infoLock.unlock();
			}

		}
		else if (state == State::Loaded) // Resource is loaded.
		{
			infoLock.lock();
			Data data = resourceInfo.resourceData[job.resourceInfoIndex];
			infoLock.unlock();

			auto ret = job.callback(job.guid, data.data, data.size);
			if (ret == 1)
			{
				infoLock.lock();
				resourceInfo.refCount[job.resourceInfoIndex]--;
				infoLock.unlock();
			}

		}
		StopProfile;
	}
	
}

void SE::ResourceHandler::ResourceHandler::CreateLoadJob(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback, Behavior behavior)
{
	StartProfile;

	ToLoadInfo tl;
	tl.behavior = behavior;
	tl.callback = callback;
	tl.resourceInfoIndex = index;
	tl.guid = guid;
	toLoadLock.lock();
	toLoad.push(tl);
	toLoadLock.unlock();
	StopProfile;
}

void SE::ResourceHandler::ResourceHandler::UpdateLoadJob(ToLoadInfo & loadInfo, const LoadResourceDelegate & callback, Behavior behavior)
{
	StartProfile;
	//toLoadLock.lock();
	//loadInfo.behavior = loadInfo.behavior == Behavior::QUICK ? Behavior::QUICK : behavior; // If someone already needs the resource quickly, make sure that we don't change it to lazy.
	//loadInfo.callbacks += callback;
	//toLoadLock.unlock();
	StopProfile;
}

void SE::ResourceHandler::ResourceHandler::RemoveLoadJob(const Utilz::GUID & guid)
{
	StartProfile;
	//toLoadLock.lock();
	//toLoad.erase(guid);
	//auto index = guidToResourceInfoIndex[guid];
	//resourceInfo.state[index] = State::Dead;
	//toLoadLock.unlock();
	StopProfile;
}

int SE::ResourceHandler::ResourceHandler::LoadSync(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback)
{
	StartProfile;
	auto result = diskLoader->LoadResource(guid, &resourceInfo.resourceData[index].data, &resourceInfo.resourceData[index].size);
	if (result)
	{
	//	Utilz::Console::Print("Could not load resource GUID: %u, Error: %d.\n", guid, result);
		ProfileReturnConst(result);
	}
		
	resourceInfo.state[index] = State::Loaded;
	auto cb = callback;
	ProfileReturn(InvokeCallback(guid, index, callback));
}

int SE::ResourceHandler::ResourceHandler::InvokeCallback(const Utilz::GUID& guid, size_t index, const LoadResourceDelegate& callback)
{
	StartProfile;
	auto result = callback(guid, resourceInfo.resourceData[index].data, resourceInfo.resourceData[index].size);
	if (result == 1) // Tells the resourceHandler to decrease the refcount.
	{
		infoLock.lock();
		resourceInfo.refCount[index]--;
		infoLock.unlock();
		ProfileReturnConst(0);
	}

	ProfileReturnConst(result);
}
