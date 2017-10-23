#include <RenderableManager.h>
#include <Profiler.h>
#include <Utilz\Console.h>

#include <Graphics\VertexStructs.h>
#include <Graphics\FileHeaders.h>

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

using namespace std::chrono_literals;

SE::Core::RenderableManager::RenderableManager(const InitializationInfo& initInfo)
	: initInfo(initInfo)
{

	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.transformManager);

	StartProfile;
	switch (initInfo.unloadingStrat)
	{
	case ResourceHandler::UnloadingStrategy::Linear:
		Unload = &RenderableManager::LinearUnload;
		break;
	default:
		break;
	}

	Allocate(128);
	initInfo.transformManager->SetDirty += {this, &RenderableManager::SetDirty};

	auto res = initInfo.resourceHandler->LoadResource(Utilz::GUID("Placeholder_Block.mesh"), [this](auto guid, auto data, auto size) {
		auto handle = LoadModel(data, size);
		if (handle == -1)
			return ResourceHandler::InvokeReturn::Fail;		
		guidToBufferInfoIndex[guid] = bufferInfo.size();
		bufferInfo.push_back({ handle, BufferState::Loaded, size });
		bufferInfo[bufferInfo.size() - 1].entities.push_back(0);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	if (res)
		throw std::exception("Could not load default mesh.");

	res = initInfo.resourceHandler->LoadResource(Utilz::GUID("SimpleVS.hlsl"), { this , &RenderableManager::LoadDefaultShader });
	if (res)
		throw std::exception("Could not load default vertex shader.");


	StopProfile;
}

SE::Core::RenderableManager::~RenderableManager()
{
	operator delete(renderableObjectInfo.data);

}

void SE::Core::RenderableManager::CreateRenderableObject(const Entity& entity, const Utilz::GUID& meshGUID, bool async, ResourceHandler::Behavior behavior)
{
	StartProfile;
	// See so that the entity does not have a renderable object already.
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find == entityToRenderableObjectInfoIndex.end())
	{
		// Check if the entity is alive
		if (!initInfo.entityManager.Alive(entity))
			ProfileReturnVoid;

		// Make sure we have enough memory.
		if (renderableObjectInfo.used + 1 > renderableObjectInfo.allocated)
			Allocate(renderableObjectInfo.allocated * 2);

		// Register the entity
		size_t newEntry = renderableObjectInfo.used;
		entityToRenderableObjectInfoIndex[entity] = newEntry;
		renderableObjectInfo.entity[newEntry] = entity;
		renderableObjectInfo.used++;
		renderableObjectInfo.visible[newEntry] = 0u;
		renderableObjectInfo.fillSolid[newEntry] = 1;
		renderableObjectInfo.transparency[newEntry] = 0;

		initInfo.entityManager.RegisterDestroyCallback(entity, { this, &RenderableManager::DestroyEntity });


		// Transform binding
		renderableObjectInfo.topology[newEntry] = Graphics::RenderObjectInfo::PrimitiveTopology::TRIANGLE_LIST;


		// Load the model
		LoadResource(meshGUID, newEntry, async, behavior);




	}
	StopProfile;
}

void SE::Core::RenderableManager::ToggleRenderableObject(const Entity & entity, bool visible)
{
	StartProfile;
	// See so that the entity exist

	auto find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		//If the visibility state is switched to what it already is we dont do anything.
		if ((bool)renderableObjectInfo.visible[find->second] == visible)
		{
			ProfileReturnVoid;
		}
			
		renderableObjectInfo.visible[find->second] = visible ? 1u : 0u;
		Graphics::RenderObjectInfo info;
		CreateRenderObjectInfo(find->second, &info);

		if (visible)
		{
			const uint32_t jobID = initInfo.renderer->EnableRendering(info);
			renderableObjectInfo.jobID[find->second] = jobID;
			//Dummy-move to make the entity "dirty" so that the transform is sent to the renderer
			initInfo.transformManager->SetAsDirty(entity);
		}
		else
		{
			initInfo.renderer->DisableRendering(renderableObjectInfo.jobID[find->second]);
		}

	}

	ProfileReturnVoid;
}



void SE::Core::RenderableManager::Frame()
{
	StartProfile;
	GarbageCollection();

	while (!toUpdate.wasEmpty())
	{
		auto& job = toUpdate.top();
		bufferInfo[job.bufferIndex].bufferHandle = job.newHandle;
		bufferInfo[job.bufferIndex].size = job.size;
		bufferInfo[job.bufferIndex].state = BufferState::Loaded;
		for (auto& e : bufferInfo[job.bufferIndex].entities)
			UpdateRenderableObject(e);
		toUpdate.pop();
	}
	UpdateDirtyTransforms();
	ProfileReturnVoid;
}

void SE::Core::RenderableManager::CreateRenderObjectInfo(size_t index, Graphics::RenderObjectInfo * info)
{
	auto vBufferIndex = renderableObjectInfo.bufferIndex[index];
	info->bufferHandle = bufferInfo[vBufferIndex].bufferHandle;
	info->topology = renderableObjectInfo.topology[index];
	info->vertexShader = defaultShader;
	info->fillSolid = renderableObjectInfo.fillSolid[index];
	info->transparency = renderableObjectInfo.transparency[index];

	// Gather Renderobjectinfo from other managers
	SetRenderObjectInfoEvent(renderableObjectInfo.entity[index], info);
}

void SE::Core::RenderableManager::LinearUnload(size_t sizeToAdd)
{
	size_t freed = 0;
	std::vector<size_t> toFree;
	if (!initInfo.renderer->IsUnderLimit(sizeToAdd))
	{
		for (size_t i = 0; i < bufferInfo.size(); i++)
		{
			if (bufferInfo[i].state == BufferState::Loaded && bufferInfo[i].entities.size() == 0)
			{
				freed += bufferInfo[i].size;
				toFree.push_back(i);

				if (initInfo.renderer->IsUnderLimit(freed, sizeToAdd))
					break;
			}
		}
	}
	if (initInfo.renderer->IsUnderLimit(freed, sizeToAdd))
	{
		bufferLock.lock();
		for (auto& r : toFree)
		{
			if (bufferInfo[r].state == BufferState::Loaded && bufferInfo[r].entities.size() == 0)
			{
				initInfo.renderer->DestroyVertexBuffer(bufferInfo[r].bufferHandle);
				bufferInfo[r].state = BufferState::Dead;
			}
		}
		bufferLock.unlock();
		std::this_thread::sleep_for(200ms);
	}
}

void SE::Core::RenderableManager::UpdateRenderableObject(const Entity & entity)
{
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		if (renderableObjectInfo.visible[find->second])
		{
			Graphics::RenderObjectInfo info;
			CreateRenderObjectInfo(find->second, &info);
			initInfo.renderer->UpdateRenderingBuffer(renderableObjectInfo.jobID[find->second], info);
		}
	}
}

void SE::Core::RenderableManager::SetFillSolid(const Entity & entity, uint8_t fillSolid)
{
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		if (renderableObjectInfo.visible[find->second] == 1u)
		{
			renderableObjectInfo.fillSolid[find->second] = fillSolid;
			Graphics::RenderObjectInfo info;
			CreateRenderObjectInfo(find->second, &info);
			initInfo.renderer->UpdateRenderingBuffer(renderableObjectInfo.jobID[find->second], info);
		}
		else
		{
			renderableObjectInfo.fillSolid[find->second] = fillSolid;
		}
	}
}

void SE::Core::RenderableManager::SetTransparency(const Entity & entity, uint8_t transparency)
{
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		if (renderableObjectInfo.visible[find->second] == 1u)
		{
			renderableObjectInfo.transparency[find->second] = transparency;
			Graphics::RenderObjectInfo info;
			CreateRenderObjectInfo(find->second, &info);
			initInfo.renderer->UpdateRenderingBuffer(renderableObjectInfo.jobID[find->second], info);
		}
		else
		{
			renderableObjectInfo.transparency[find->second] = transparency;
		}
	}
}

void SE::Core::RenderableManager::Allocate(size_t size)
{
	StartProfile;
	_ASSERT(size > renderableObjectInfo.allocated);

	// Allocate new memory
	RenderableObjectData newData;
	newData.allocated = size;
	newData.data = operator new(size * RenderableObjectData::size);
	newData.used = renderableObjectInfo.used;

	// Setup the new pointers
	newData.entity = (Entity*)newData.data;
	newData.bufferIndex = (size_t*)(newData.entity + newData.allocated);
	newData.topology = (Graphics::RenderObjectInfo::PrimitiveTopology*)(newData.bufferIndex + newData.allocated);
	newData.visible = (uint8_t*)(newData.topology + newData.allocated);
	newData.jobID = (uint32_t*)(newData.visible + newData.allocated);
	newData.fillSolid = (uint8_t*)(newData.jobID + newData.allocated);
	newData.transparency = (uint8_t*)(newData.fillSolid + newData.allocated);

	// Copy data
	memcpy(newData.entity, renderableObjectInfo.entity, renderableObjectInfo.used * sizeof(Entity));
	memcpy(newData.bufferIndex, renderableObjectInfo.bufferIndex, renderableObjectInfo.used * sizeof(size_t));
	memcpy(newData.topology, renderableObjectInfo.topology, renderableObjectInfo.used * sizeof(Graphics::RenderObjectInfo::PrimitiveTopology));
	memcpy(newData.visible, renderableObjectInfo.visible, renderableObjectInfo.used * sizeof(uint8_t));
	memcpy(newData.jobID, renderableObjectInfo.jobID, renderableObjectInfo.used * sizeof(uint32_t));
	memcpy(newData.fillSolid, renderableObjectInfo.fillSolid, renderableObjectInfo.used * sizeof(bool));
	memcpy(newData.transparency, renderableObjectInfo.transparency, renderableObjectInfo.used * sizeof(bool));

	// Delete old data;
	operator delete(renderableObjectInfo.data);
	renderableObjectInfo = newData;
	StopProfile;
}

void SE::Core::RenderableManager::Destroy(size_t index)
{
	StartProfile;
	// Temp variables
	size_t last = renderableObjectInfo.used - 1;
	const Entity entity = renderableObjectInfo.entity[index];
	const Entity last_entity = renderableObjectInfo.entity[last];

	if(renderableObjectInfo.visible[index])
		initInfo.renderer->DisableRendering(renderableObjectInfo.jobID[index]);


	bufferInfo[renderableObjectInfo.bufferIndex[index]].entities.remove(entity); // Decrease the refcount


	// Copy the data
	renderableObjectInfo.entity[index] = last_entity;
	renderableObjectInfo.bufferIndex[index] = renderableObjectInfo.bufferIndex[last];
	renderableObjectInfo.topology[index] = renderableObjectInfo.topology[last];
	renderableObjectInfo.visible[index] = renderableObjectInfo.visible[last];
	renderableObjectInfo.jobID[index] = renderableObjectInfo.jobID[last];
	renderableObjectInfo.fillSolid[index] = renderableObjectInfo.fillSolid[last];
	renderableObjectInfo.transparency[index] = renderableObjectInfo.transparency[last];


	// Replace the index for the last_entity 
	entityToRenderableObjectInfoIndex[last_entity] = index;
	entityToRenderableObjectInfoIndex.erase(entity);

	renderableObjectInfo.used--;

	StopProfile;
}

void SE::Core::RenderableManager::DestroyEntity(const Entity & entity)
{
	ToggleRenderableObject(entity, false);
	/*auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		Destroy(find->second);
	}*/
}

void SE::Core::RenderableManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (renderableObjectInfo.used > 0 && alive_in_row < 50U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, renderableObjectInfo.used - 1U);
		size_t i = distribution(generator);
		if (initInfo.entityManager.Alive(renderableObjectInfo.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}

void SE::Core::RenderableManager::UpdateDirtyTransforms()
{
	StartProfile;

	for (auto& dirty : dirtyEntites)
	{
		auto& find = entityToRenderableObjectInfoIndex.find(dirty.entity);
		if (find != entityToRenderableObjectInfoIndex.end())
		{			
			if (renderableObjectInfo.visible[find->second])
			{
				auto& transform = initInfo.transformManager->dirtyTransforms[dirty.transformIndex];
				initInfo.renderer->UpdateTransform(renderableObjectInfo.jobID[find->second], (float*)&transform);
			}				
		}
	}

	dirtyEntites.clear();
	StopProfile;
}


SE::ResourceHandler::InvokeReturn SE::Core::RenderableManager::LoadDefaultShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	defaultShader = initInfo.renderer->CreateVertexShader(data, size);
	if (defaultShader == -1)
		ProfileReturnConst(ResourceHandler::InvokeReturn::Fail);
	ProfileReturnConst(ResourceHandler::InvokeReturn::DecreaseRefcount);
}

void SE::Core::RenderableManager::LoadResource(const Utilz::GUID& meshGUID, size_t newEntry, bool async, ResourceHandler::Behavior behavior)
{
	StartProfile;
	// Load model
	auto& findBuffer = guidToBufferInfoIndex.find(meshGUID); // See if it the mesh is loaded.
	auto& bufferIndex = guidToBufferInfoIndex[meshGUID]; // Get a reference to the buffer index
	bufferLock.lock();
	if (findBuffer == guidToBufferInfoIndex.end() || bufferInfo[bufferIndex].state == BufferState::Dead)	// If it wasn't loaded, load it.	
	{
		if (findBuffer == guidToBufferInfoIndex.end())
		{
			bufferIndex = bufferInfo.size();
			bufferInfo.push_back({ bufferInfo[0].bufferHandle, BufferState::Loading });
		}
		else
		{
			bufferInfo[bufferIndex].bufferHandle = bufferInfo[0].bufferHandle;
			bufferInfo[bufferIndex].state = BufferState::Loading;
		}
	
		bufferLock.unlock();

		auto res = initInfo.resourceHandler->LoadResource(meshGUID, [this, bufferIndex,async](auto guid, auto data, auto size)->ResourceHandler::InvokeReturn {		
			auto bufferHandle = LoadModel(data, size);
			if (bufferHandle == -1)
				return ResourceHandler::InvokeReturn::Fail;

			(*this.*Unload)(size);
		
			if (async) 
			{
				toUpdate.push({ bufferIndex, bufferHandle, size });
			}
			
			else
			{
				bufferInfo[bufferIndex].bufferHandle = bufferHandle;
				bufferInfo[bufferIndex].size = size;
				bufferInfo[bufferIndex].state = BufferState::Loaded;
			}
				
			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		}, async, behavior);
		
		
		//{ this , &RenderableManager::LoadModel }, async, behavior);
		if (res)
			Utilz::Console::Print("Model %u could not be loaded. Using default instead.\n", meshGUID);

	}
	else bufferLock.unlock();
	bufferInfo[bufferIndex].entities.push_back(renderableObjectInfo.entity[newEntry]);
	renderableObjectInfo.bufferIndex[newEntry] = bufferIndex;
	StopProfile;
}

int SE::Core::RenderableManager::LoadModel(void* data, size_t size)
{
	StartProfile;

	auto bufferHandle = -1;

	auto meshHeader = (Graphics::Mesh_Header*)data;
	if (meshHeader->vertexLayout == 0) {

		Vertex* v = (Vertex*)(meshHeader + 1);
		bufferHandle = initInfo.renderer->CreateVertexBuffer(v, meshHeader->nrOfVertices, sizeof(Vertex));

	}

	else {
		VertexDeformer* v = (VertexDeformer*)(meshHeader + 1);

		bufferHandle = initInfo.renderer->CreateVertexBuffer(v, meshHeader->nrOfVertices, sizeof(VertexDeformer));
	}

	ProfileReturnConst( bufferHandle);
}

void SE::Core::RenderableManager::SetDirty(const Entity & entity, size_t index)
{

	dirtyEntites.push_back({ index, entity });

}
