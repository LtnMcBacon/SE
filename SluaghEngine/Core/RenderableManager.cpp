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



SE::Core::RenderableManager::RenderableManager(ResourceHandler::IResourceHandler * resourceHandler, Graphics::IRenderer * renderer, const EntityManager & entityManager, TransformManager * transformManager, AnimationManager* animationManager)
	:resourceHandler(resourceHandler), renderer(renderer), entityManager(entityManager), transformManager(transformManager),  animationManager(animationManager)
{
	_ASSERT(resourceHandler);
	_ASSERT(renderer);
	_ASSERT(transformManager);
	_ASSERT(animationManager);

	Allocate(128);
	transformManager->SetDirty.Add<RenderableManager, &RenderableManager::SetDirty>(this);
	defaultMeshHandle = 0;
	defaultShader = 0;

	auto res = resourceHandler->LoadResource(Utilz::GUID("Placeholder_Block.mesh"), ResourceHandler::LoadResourceDelegate::Make<RenderableManager, &RenderableManager::LoadDefaultModel>(this));
	if (res)
		throw std::exception("Could not load default mesh.");

	res = resourceHandler->LoadResource(Utilz::GUID("SimpleVS.hlsl"), ResourceHandler::LoadResourceDelegate::Make<RenderableManager, &RenderableManager::LoadDefaultShader>(this));
	if (res)
		throw std::exception("Could not load default vertex shader.");

	res = resourceHandler->LoadResource(Utilz::GUID("SkinnedVS.hlsl"), ResourceHandler::LoadResourceDelegate::Make<RenderableManager, &RenderableManager::LoadSkinnedShader>(this));
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
		if (!entityManager.Alive(entity))
			ProfileReturnVoid;

		// Make sure we have enough memory.
		if (renderableObjectInfo.used + 1 > renderableObjectInfo.allocated)
			Allocate(renderableObjectInfo.allocated * 2);

		// Register the entity
		size_t newEntry = renderableObjectInfo.used;
		entityToRenderableObjectInfoIndex[entity] = newEntry;
		renderableObjectInfo.entity[newEntry] = entity;
		renderableObjectInfo.used++;

		// Load the model
		LoadResource(meshGUID, newEntry, async, behavior);


		// Transform binding
		renderableObjectInfo.topology[newEntry] = Graphics::RenderObjectInfo::PrimitiveTopology::TRIANGLE_LIST;
		renderableObjectInfo.visible[newEntry] = 0;

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
			ProfileReturnVoid;
		renderableObjectInfo.visible[find->second] = visible ? 1 : 0;
		Graphics::RenderObjectInfo info;
		CreateRenderObjectInfo(find->second, &info);
		infoLock.lock();
		if (visible)
		{
			const uint32_t jobID = renderer->EnableRendering(info);
			renderableObjectInfo.jobID[find->second] = jobID;
			//Dummy-move to make the entity "dirty" so that the transform is sent to the renderer
			transformManager->SetAsDirty(entity);
		}
		else
		{
			renderer->DisableRendering(renderableObjectInfo.jobID[find->second]);
		}
		infoLock.unlock();
	}
	ProfileReturnVoid;
}



void SE::Core::RenderableManager::Frame()
{
	StartProfile;
	GarbageCollection();
	UpdateDirtyTransforms();
	ProfileReturnVoid;
}

void SE::Core::RenderableManager::CreateRenderObjectInfo(size_t index, Graphics::RenderObjectInfo * info)
{
	auto vBufferIndex = renderableObjectInfo.bufferIndex[index];
	info->bufferHandle = bufferInfo[vBufferIndex].bufferHandle;
	info->topology = renderableObjectInfo.topology[index];
	info->vertexShader = defaultShader;

	// Get the entity register from the animationManager
	auto &entityIndex = animationManager->entityToIndex.find(renderableObjectInfo.entity[index]);

	// If the entity index is equal to the end of the undordered map, it means that no animated entity was found
	if (entityIndex == animationManager->entityToIndex.end()) {

		info->type = Graphics::RenderObjectInfo::JobType::STATIC;
		info->vertexShader = defaultShader;
	}

	// Otherwise, there was an animated entity and we should use the skinned vertex shader
	else {

		info->type = Graphics::RenderObjectInfo::JobType::SKINNED;
		auto skelIndex = animationManager->animationData.skeletonIndex[entityIndex->second];
		info->skeletonHandle = animationManager->skeletonHandle[skelIndex];
		info->vertexShader = skinnedShader;
	}

	// Gather Renderobjectinfo from other managers
	SetRenderObjectInfoEvent(renderableObjectInfo.entity[index], info);
	
}

void SE::Core::RenderableManager::UpdateRenderableObject(const Entity & entity)
{
	infoLock.lock();
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		if (renderableObjectInfo.visible[find->second])
		{
			Graphics::RenderObjectInfo info;
			CreateRenderObjectInfo(find->second, &info);
			renderer->UpdateRenderingBuffer(renderableObjectInfo.jobID[find->second], info);
			//transformManager->SetAsDirty(entity);
		}
	}
	infoLock.unlock();
}

void SE::Core::RenderableManager::Allocate(size_t size)
{
	StartProfile;
	infoLock.lock();
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

	// Copy data
	memcpy(newData.entity, renderableObjectInfo.entity, renderableObjectInfo.used * sizeof(Entity));
	memcpy(newData.bufferIndex, renderableObjectInfo.bufferIndex, renderableObjectInfo.used * sizeof(size_t));
	memcpy(newData.topology, renderableObjectInfo.topology, renderableObjectInfo.used * sizeof(Graphics::RenderObjectInfo::PrimitiveTopology));
	memcpy(newData.visible, renderableObjectInfo.visible, renderableObjectInfo.used * sizeof(uint8_t));
	memcpy(newData.jobID, renderableObjectInfo.jobID, renderableObjectInfo.used * sizeof(uint32_t));

	// Delete old data;
	operator delete(renderableObjectInfo.data);
	renderableObjectInfo = newData;
	infoLock.unlock();
	StopProfile;
}

void SE::Core::RenderableManager::Destroy(size_t index)
{
	StartProfile;
	infoLock.lock();
	// Temp variables
	size_t last = renderableObjectInfo.used - 1;
	const Entity& entity = renderableObjectInfo.entity[index];
	const Entity& last_entity = renderableObjectInfo.entity[last];

	if(renderableObjectInfo.visible[index])
		renderer->DisableRendering(renderableObjectInfo.jobID[index]);

	entityToChangeLock.lock();
	bufferInfo[renderableObjectInfo.bufferIndex[index]].entities.remove(entity); // Decrease the refcount
	entityToChangeLock.unlock();

	// Copy the data
	renderableObjectInfo.entity[index] = last_entity;
	renderableObjectInfo.bufferIndex[index] = renderableObjectInfo.bufferIndex[last];
	renderableObjectInfo.topology[index] = renderableObjectInfo.topology[last];
	renderableObjectInfo.visible[index] = renderableObjectInfo.visible[last];	
	renderableObjectInfo.jobID[index] = renderableObjectInfo.jobID[last];


	// Replace the index for the last_entity 
	entityToRenderableObjectInfoIndex[last_entity] = index;
	entityToRenderableObjectInfoIndex.erase(entity);

	renderableObjectInfo.used--;
	infoLock.unlock();
	StopProfile;
}

void SE::Core::RenderableManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (renderableObjectInfo.used > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<uint32_t> distribution(0U, renderableObjectInfo.used - 1U);
		uint32_t i = distribution(generator);
		if (entityManager.Alive(renderableObjectInfo.entity[i]))
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
	infoLock.lock();
	for (auto& dirty : dirtyEntites)
	{
		auto& transform = transformManager->dirtyTransforms[dirty.transformIndex];
		if(renderableObjectInfo.visible[dirty.renderableIndex])
			renderer->UpdateTransform(renderableObjectInfo.jobID[dirty.renderableIndex], (float*)&transform);
	}
	infoLock.unlock();
	dirtyEntites.clear();
	StopProfile;
}

int SE::Core::RenderableManager::LoadDefaultModel(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;

	auto meshHeader = (Graphics::Mesh_Header*)data;

	if (meshHeader->vertexLayout == 0) {

		guidToMeshType[guid] = Graphics::RenderObjectInfo::JobType::STATIC;

		Vertex* v = (Vertex*)(meshHeader + 1);
		defaultMeshHandle = renderer->CreateVertexBuffer(v, meshHeader->nrOfVertices, sizeof(Vertex));

	}

	else {

		guidToMeshType[guid] = Graphics::RenderObjectInfo::JobType::SKINNED;

		VertexDeformer* v = (VertexDeformer*)(meshHeader + 1);
		defaultMeshHandle = renderer->CreateVertexBuffer(v, meshHeader->nrOfVertices, sizeof(VertexDeformer));
	}

	if (defaultMeshHandle == -1)
		ProfileReturnConst(-1);

	ProfileReturnConst(0);
}

int SE::Core::RenderableManager::LoadSkinnedShader(const Utilz::GUID& guid, void* data, size_t size) {

	StartProfile;
	skinnedShader = renderer->CreateVertexShader(data, size);
	if (skinnedShader == -1)
		ProfileReturnConst(-1);
	ProfileReturnConst(0);
}

int SE::Core::RenderableManager::LoadDefaultShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	defaultShader = renderer->CreateVertexShader(data, size);
	if (defaultShader == -1)
		ProfileReturnConst( -1);
	ProfileReturnConst( 0);
}

void SE::Core::RenderableManager::LoadResource(const Utilz::GUID& meshGUID, size_t newEntry, bool async, ResourceHandler::Behavior behavior)
{
	StartProfile;
	// Load model
	auto& findBuffer = guidToBufferInfoIndex.find(meshGUID); // See if it the mesh is loaded.
	entityToChangeLock.lock();
	auto& bufferIndex = guidToBufferInfoIndex[meshGUID]; // Get a reference to the buffer index
	if (findBuffer == guidToBufferInfoIndex.end())	// If it wasn't loaded, load it.	
	{
		bufferInfo.push_back({ defaultMeshHandle }); // Init the texture to default texture.
		bufferIndex = bufferInfo.size() - 1;
		entityToChangeLock.unlock();

		auto res = resourceHandler->LoadResource(meshGUID, ResourceHandler::LoadResourceDelegate::Make<RenderableManager, &RenderableManager::LoadModel>(this), async, behavior);
		if (res)
			Utilz::Console::Print("Model %u could not be loaded. Using default instead.\n", meshGUID);

	}
	else
		entityToChangeLock.unlock();
	// Increase ref Count and save the index to the material info.
	//bufferInfo[bufferIndex].refCount++;
	entityToChangeLock.lock();
	bufferInfo[bufferIndex].entities.push_back(renderableObjectInfo.entity[newEntry]);
	entityToChangeLock.unlock();
	renderableObjectInfo.bufferIndex[newEntry] = bufferIndex;
	StopProfile;
}

int SE::Core::RenderableManager::LoadModel(const Utilz::GUID& guid, void* data, size_t size)
{
	StartProfile;
	//using namespace std::chrono_literals;

	//std::this_thread::sleep_for(1s);

	auto bufferHandle = -1;

	auto meshHeader = (Graphics::Mesh_Header*)data;
	if (meshHeader->vertexLayout == 0) {

		Vertex* v = (Vertex*)(meshHeader + 1);
		bufferHandle = renderer->CreateVertexBuffer(v, meshHeader->nrOfVertices, sizeof(Vertex));

	}

	else {

		VertexDeformer* v = (VertexDeformer*)(meshHeader + 1);
		bufferHandle = renderer->CreateVertexBuffer(v, meshHeader->nrOfVertices, sizeof(VertexDeformer));
	}

	if (bufferHandle == -1)
		ProfileReturnConst(-1);

	entityToChangeLock.lock();
	auto index = guidToBufferInfoIndex[guid];
	bufferInfo[index].bufferHandle = bufferHandle;
	for (auto& e : bufferInfo[index].entities)
	{
		UpdateRenderableObject(e);

		
	}
	entityToChangeLock.unlock();
	ProfileReturnConst(0);
}

void SE::Core::RenderableManager::SetDirty(const Entity & entity, size_t index)
{
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		dirtyEntites.push_back({ index, find->second, entity });
	}
}
