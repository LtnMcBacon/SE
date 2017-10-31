#include "RenderableManager.h"
#include <Profiler.h>

#include <Graphics\VertexStructs.h>
#include <Graphics\FileHeaders.h>

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

using namespace std::chrono_literals;

static const SE::Utilz::GUID solid("Solid");
static const SE::Utilz::GUID wireframe("Wireframe");
static const SE::Utilz::GUID defaultMesh("Placeholder_Block.mesh");
static const SE::Utilz::GUID defaultVertexShader("SimpleVS.hlsl");
static const SE::Utilz::GUID Transparency("RMTransparency");

SE::Core::RenderableManager::RenderableManager(const InitializationInfo& initInfo)
	: initInfo(initInfo)
{
	rmInstancing = new RenderableManagerInstancing(initInfo.renderer);
	Init();

	Allocate(128);

}

SE::Core::RenderableManager::RenderableManager(const IRenderableManager::InitializationInfo & initInfo, 
	size_t allocsize, RenderableManagerInstancing* rmI) : initInfo(initInfo), rmInstancing(rmI)
{
	
	Init();

	Allocate(allocsize);
}

SE::Core::RenderableManager::~RenderableManager()
{
	delete rmInstancing;

	operator delete(renderableObjectInfo.data);
}

void SE::Core::RenderableManager::CreateRenderableObject(const Entity& entity, const CreateInfo& info, bool async, ResourceHandler::Behavior behavior)
{
	StartProfile;
	// See so that the entity does not have a renderable object already.
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find == entityToRenderableObjectInfoIndex.end())
	{
		// Check if the entity is alive
		if (!initInfo.entityManager->Alive(entity))
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
		renderableObjectInfo.wireframe[newEntry] = info.wireframe ? 1u: 0u;
		renderableObjectInfo.transparency[newEntry] = info.transparent ? 1u : 0u;

		initInfo.entityManager->RegisterDestroyCallback(entity, { this, &RenderableManager::Destroy });


		// Load the model
		LoadResource(info.meshGUID, newEntry, async, behavior);




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
		Graphics::RenderJob info;
		CreateRenderObjectInfo(find->second, &info);
		if (visible)
		{
			rmInstancing->AddEntity(entity, info);
			rmInstancing->UpdateTransform(entity, initInfo.transformManager->GetTransform(entity));
		}
		else
		{
			rmInstancing->RemoveEntity(entity);
		}

	}

	ProfileReturnVoid;
}



void SE::Core::RenderableManager::Frame(Utilz::TimeCluster* timer)
{
	_ASSERT(timer);
	StartProfile;
	timer->Start(CREATE_ID_HASH("RenderableManager"));
	GarbageCollection();


	while (!toUpdate.wasEmpty())
	{
		auto& job = toUpdate.top();
		auto& binfo = guidToBufferInfo[job.mesh];
		binfo.state = BufferState::Loaded;
		binfo.size = job.size;
		for (auto& e : binfo.entities)
		{
			const auto findEntity = entityToRenderableObjectInfoIndex.find(e);
			if (findEntity != entityToRenderableObjectInfoIndex.end())
			{
				renderableObjectInfo.mesh[findEntity->second] = job.mesh;
				guidToBufferInfo[job.mesh].size = job.size;
				guidToBufferInfo[job.mesh].vertexCount = job.vertexCount;
				guidToBufferInfo[job.mesh].state = BufferState::Loaded;

				UpdateRenderableObject(e);
			}		
		}

		toUpdate.pop();
	}


	UpdateDirtyTransforms();
	timer->Stop(CREATE_ID_HASH("RenderableManager"));
	ProfileReturnVoid;
}

void SE::Core::RenderableManager::CreateRenderObjectInfo(size_t index, Graphics::RenderJob * info)
{
	info->pipeline.OMStage.renderTargets[0] = "backbuffer";
	info->pipeline.OMStage.renderTargetCount = 1;
	info->pipeline.OMStage.depthStencilView = "backbuffer";

	info->pipeline.VSStage.shader = defaultVertexShader;

	info->pipeline.IAStage.vertexBuffer = renderableObjectInfo.mesh[index];
	info->pipeline.IAStage.inputLayout = defaultVertexShader;
	info->pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;

	info->pipeline.RStage.rasterizerState = renderableObjectInfo.wireframe[index] ? wireframe : solid;
	info->pipeline.OMStage.blendState = renderableObjectInfo.transparency[index] ? Transparency : Utilz::GUID();

	info->vertexCount = guidToBufferInfo[renderableObjectInfo.mesh[index]].vertexCount;
	info->maxInstances = 256;
	info->specialHaxxor = "OncePerObject";

	initInfo.eventManager->TriggerSetRenderObjectInfo(renderableObjectInfo.entity[index], info);
}

void SE::Core::RenderableManager::LinearUnload(size_t sizeToAdd)
{
	size_t freed = 0;
	std::vector<Utilz::GUID> toFree;
	if (!initInfo.renderer->IsUnderLimit(sizeToAdd))
	{
		for(auto& binfo : guidToBufferInfo)
		{
			if (binfo.second.state == BufferState::Loaded && binfo.second.entities.size() == 0)
			{
				freed += binfo.second.size;
				toFree.push_back(binfo.first);

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
			auto& binfo = guidToBufferInfo[r];
			if (binfo.state == BufferState::Loaded && binfo.entities.size() == 0)
			{
				initInfo.renderer->GetPipelineHandler()->DestroyVertexBuffer(r);
				binfo.state = BufferState::Dead;
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
			Graphics::RenderJob info;
			CreateRenderObjectInfo(find->second, &info);
			rmInstancing->AddEntity(entity, info);
			rmInstancing->UpdateTransform(entity, initInfo.transformManager->GetTransform(entity));
		}
	}
}

void SE::Core::RenderableManager::ToggleWireframe(const Entity & entity, bool wireFrame)
{
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		bool p = static_cast<bool>(renderableObjectInfo.wireframe[find->second]);
		renderableObjectInfo.wireframe[find->second] = wireFrame ? 1u : 0u;
		if (renderableObjectInfo.visible[find->second] == 1u &&  p != wireFrame)
		{
			Graphics::RenderJob info;
			CreateRenderObjectInfo(find->second, &info);
			rmInstancing->AddEntity(entity, info);
		}
		
	}
}

void SE::Core::RenderableManager::ToggleTransparency(const Entity & entity, bool transparency)
{
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		if (renderableObjectInfo.visible[find->second] == 1u && static_cast<bool>(renderableObjectInfo.transparency[find->second]) != transparency)
		{
			renderableObjectInfo.transparency[find->second] = transparency ? 1u : 0u;
			Graphics::RenderJob info;
			CreateRenderObjectInfo(find->second, &info); 
			rmInstancing->AddEntity(entity, info);
		}
		
	}
}

bool SE::Core::RenderableManager::IsVisible(const Entity & entity) const
{
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		return static_cast<bool>(renderableObjectInfo.visible[find->second]);
	}
	return false;
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
	newData.mesh = (Utilz::GUID*)(newData.entity + newData.allocated);
	newData.visible = (uint8_t*)(newData.mesh + newData.allocated);
	newData.wireframe = (uint8_t*)(newData.visible + newData.allocated);
	newData.transparency = (uint8_t*)(newData.wireframe + newData.allocated);

	// Copy data
	memcpy(newData.entity, renderableObjectInfo.entity, renderableObjectInfo.used * sizeof(Entity));
	memcpy(newData.mesh, renderableObjectInfo.mesh, renderableObjectInfo.used * sizeof(Utilz::GUID));
	memcpy(newData.visible, renderableObjectInfo.visible, renderableObjectInfo.used * sizeof(uint8_t));
	memcpy(newData.wireframe, renderableObjectInfo.wireframe, renderableObjectInfo.used * sizeof(uint8_t));
	memcpy(newData.transparency, renderableObjectInfo.transparency, renderableObjectInfo.used * sizeof(uint8_t));

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

	if (renderableObjectInfo.visible[index])
		rmInstancing->RemoveEntity(entity);


	guidToBufferInfo[renderableObjectInfo.mesh[index]].entities.remove(entity); // Decrease the refcount


	// Copy the data
	renderableObjectInfo.entity[index] = last_entity;
	renderableObjectInfo.mesh[index] = renderableObjectInfo.mesh[last];
	renderableObjectInfo.visible[index] = renderableObjectInfo.visible[last];
	renderableObjectInfo.wireframe[index] = renderableObjectInfo.wireframe[last];
	renderableObjectInfo.transparency[index] = renderableObjectInfo.transparency[last];


	// Replace the index for the last_entity 
	entityToRenderableObjectInfoIndex[last_entity] = index;
	entityToRenderableObjectInfoIndex.erase(entity);

	renderableObjectInfo.used--;

	StopProfile;
}

void SE::Core::RenderableManager::Init()
{
	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.eventManager);
	_ASSERT(initInfo.transformManager);
	_ASSERT(initInfo.console);

	initInfo.eventManager->RegisterToUpdateRenderableObject({ this, &RenderableManager::UpdateRenderableObject });

	
	switch (initInfo.unloadingStrat)
	{
	case ResourceHandler::UnloadingStrategy::Linear:
		Unload = &RenderableManager::LinearUnload;
		break;
	default:
		break;
	}

	
	initInfo.transformManager->RegisterSetDirty({ this, &RenderableManager::SetDirty });

	auto res = initInfo.resourceHandler->LoadResource(defaultMesh, [this](auto guid, auto data, auto size) {
		auto& binfo = guidToBufferInfo[guid];
		auto res = LoadModel(guid, data, size, binfo.vertexCount);
		if (res < 0)
			return ResourceHandler::InvokeReturn::Fail;
		binfo.state = BufferState::Loaded;
		binfo.size = size;
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	if (res)
		throw std::exception("Could not load default mesh");

	res = initInfo.resourceHandler->LoadResource(defaultVertexShader, { this , &RenderableManager::LoadDefaultShader });
	if (res)
		throw std::exception("Could not load default shader");


	Graphics::RasterizerState info;
	info.cullMode = Graphics::CullMode::CULL_BACK;
	info.fillMode = Graphics::FillMode::FILL_SOLID;
	info.windingOrder = Graphics::WindingOrder::CLOCKWISE;

	auto result = initInfo.renderer->GetPipelineHandler()->CreateRasterizerState(solid, info);
	if (result < 0)
		throw std::exception("Could not create Solid Rasterizer.");

	info.fillMode = Graphics::FillMode::FILL_WIREFRAME;
	info.cullMode = Graphics::CullMode::CULL_BACK;
	result = initInfo.renderer->GetPipelineHandler()->CreateRasterizerState(wireframe, info);
	if (result < 0)
		throw std::exception("Could not create wireframe Rasterizer.");


	Graphics::BlendState bs;
	bs.enable = true;
	bs.blendOperation = Graphics::BlendOperation::ADD;
	bs.blendOperationAlpha = Graphics::BlendOperation::MAX;
	bs.srcBlend = Graphics::Blend::INV_SRC_ALPHA;
	bs.srcBlendAlpha = Graphics::Blend::ONE;
	bs.dstBlend = Graphics::Blend::INV_SRC_ALPHA;
	bs.dstBlendAlpha = Graphics::Blend::ONE;

	result = this->initInfo.renderer->GetPipelineHandler()->CreateBlendState(Transparency, bs);
	if (result < 0)
		throw std::exception("Could not create Transparency Blendstate.");
}

void SE::Core::RenderableManager::Destroy(const Entity & entity)
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
		if (initInfo.entityManager->Alive(renderableObjectInfo.entity[i]))
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
	auto arr = initInfo.transformManager->GetCleanedTransforms();
	for (auto& dirty : dirtyEntites)
	{
		auto& find = entityToRenderableObjectInfoIndex.find(dirty.entity);
		if (find != entityToRenderableObjectInfoIndex.end())
		{			
			if (renderableObjectInfo.visible[find->second])
			{
				rmInstancing->UpdateTransform(dirty.entity, arr[dirty.transformIndex]);
			}				
		}
	}

	dirtyEntites.clear();
	StopProfile;
}


SE::ResourceHandler::InvokeReturn SE::Core::RenderableManager::LoadDefaultShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	int result = 0;
	/*result = */initInfo.renderer->GetPipelineHandler()->CreateVertexShader(guid, data, size);
	if (result < 0)
		ProfileReturnConst(ResourceHandler::InvokeReturn::Fail);
	ProfileReturnConst(ResourceHandler::InvokeReturn::DecreaseRefcount);
}

void SE::Core::RenderableManager::LoadResource(const Utilz::GUID& meshGUID, size_t newEntry, bool async, ResourceHandler::Behavior behavior)
{
	StartProfile;
	// Load model
	auto& findBuffer = guidToBufferInfo.find(meshGUID); // See if it the mesh is loaded.
	auto& bufferInfo = guidToBufferInfo[meshGUID]; // Get a reference to the buffer index
	bufferLock.lock();
	renderableObjectInfo.mesh[newEntry] = defaultMesh;
	if (findBuffer == guidToBufferInfo.end() || bufferInfo.state == BufferState::Dead)	// If it wasn't loaded, load it.	
	{
		bufferInfo.state = BufferState::Loading;
	
		bufferLock.unlock();

		auto res = initInfo.resourceHandler->LoadResource(meshGUID, [this,async](auto guid, auto data, auto size)->ResourceHandler::InvokeReturn {	
			int vertexCount = 0;
			auto result = LoadModel(guid, data, size, vertexCount);
			if (result < 0)
				return ResourceHandler::InvokeReturn::Fail;

			//(*this.*Unload)(size);
		
			if (async) 
			{
				toUpdate.push({ guid, size, vertexCount });
			}	
			else
			{
				auto& binfo = guidToBufferInfo[guid];
				binfo.size = size;
				binfo.vertexCount = vertexCount;
				binfo.state = BufferState::Loaded;
			}
				
			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		}, async, behavior);
		
		
		if (res)
			initInfo.console->PrintChannel("Resources", "Model %u could not be loaded, Error: %d. Using default instead.\n",  meshGUID, res);
		else if(!async)
			renderableObjectInfo.mesh[newEntry] = meshGUID;
	}
	else 
	{
		bufferLock.unlock();
		renderableObjectInfo.mesh[newEntry] = meshGUID;
	}
	

	guidToBufferInfo[meshGUID].entities.push_back(renderableObjectInfo.entity[newEntry]);
	
	StopProfile;
}

int SE::Core::RenderableManager::LoadModel(const Utilz::GUID& meshGUID, void* data, size_t size, int& vertexCount)
{
	int result = 0;
	auto meshHeader = (Graphics::Mesh_Header*)data;
	vertexCount = meshHeader->nrOfVertices;
	if (meshHeader->vertexLayout == 0) {
		Vertex* v = (Vertex*)(meshHeader + 1);
		
		result = initInfo.renderer->GetPipelineHandler()->CreateVertexBuffer(meshGUID, v, meshHeader->nrOfVertices, sizeof(Vertex));
	}
	else {
		VertexDeformer* v = (VertexDeformer*)(meshHeader + 1);
		result = initInfo.renderer->GetPipelineHandler()->CreateVertexBuffer(meshGUID, v, meshHeader->nrOfVertices, sizeof(VertexDeformer));
	}
	return result;
}

void SE::Core::RenderableManager::SetDirty(const Entity & entity, size_t index)
{

	dirtyEntites.push_back({ index, entity });

}
