#include <RenderableManager.h>
#include <Profiler.h>
#include <Utilz\Console.h>
#include <OBJParser\Parsers.h>

#include <VertexStructs.h>
#include <FileHeaders.h>

#ifdef _DEBUG
#pragma comment(lib, "OBJParser.lib")
#else
#pragma comment(lib, "OBJParser.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif



SE::Core::RenderableManager::RenderableManager(ResourceHandler::IResourceHandler * resourceHandler, Graphics::IRenderer * renderer, const EntityManager & entityManager, TransformManager * transformManager, MaterialManager* materialManager, AnimationManager* animationManager)
	:resourceHandler(resourceHandler), renderer(renderer), entityManager(entityManager), transformManager(transformManager), materialManager(materialManager), animationManager(animationManager)
{

	_ASSERT(resourceHandler);
	_ASSERT(renderer);
	_ASSERT(transformManager);
	_ASSERT(materialManager);
	_ASSERT(animationManager);

	Allocate(128);
	transformManager->SetDirty.Add<RenderableManager, &RenderableManager::SetDirty>(this);
	defaultMeshHandle = 0;
	defaultShader = 0;

	auto res = resourceHandler->LoadResource(Utilz::GUID("TestMesh_bakedTest.mesh"), ResourceHandler::LoadResourceDelegate::Make<RenderableManager, &RenderableManager::LoadDefaultModel>(this));
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

void SE::Core::RenderableManager::CreateRenderableObject(const Entity& entity, const Utilz::GUID& meshGUID)
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
		LoadResource(meshGUID, newEntry);


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
		auto vBufferIndex = renderableObjectInfo.bufferIndex[find->second];
		info.bufferHandle = bufferInfo[vBufferIndex].bufferHandle;
		info.topology = renderableObjectInfo.topology[find->second];
		info.vertexShader = defaultShader;

		// Get the entity register from the animationManager
		auto &entityIndex = animationManager->entityToIndex.find(entity);

		// If the entity index is equal to the end of the undordered map, it means that no animated entity was found
		if (entityIndex == animationManager->entityToIndex.end()) {

			info.type = Graphics::RenderObjectInfo::JobType::STATIC;
			info.vertexShader = defaultShader;
		}

		// Otherwise, there was an animated entity and we should use the skinned vertex shader
		else {

			info.type = Graphics::RenderObjectInfo::JobType::SKINNED;
			auto skelIndex = animationManager->animationData.skeletonIndex[entityIndex->second];
			info.skeletonHandle = animationManager->skeletonHandle[skelIndex];
			info.vertexShader = skinnedShader;
		}
		

		auto& find = materialManager->entityToMaterialInfo.find(entity);
		if (find != materialManager->entityToMaterialInfo.end())
		{
			info.pixelShader = materialManager->shaders[materialManager->materialInfo.shaderIndex[find->second]].shaderHandle;
			auto& reflection = materialManager->shaders[materialManager->materialInfo.shaderIndex[find->second]].shaderReflection;
			const int textureCount = reflection.textureNameToBindSlot.size();
			info.textureCount = textureCount;
			for(int i = 0; i < textureCount; ++i)
			{
				info.textureBindings[i] = materialManager->materialInfo.textureBindings[find->second].bindings[i];
				info.textureHandles[i] = materialManager->textures[materialManager->materialInfo.textureIndices[find->second].indices[i]].textureHandle;
			}
		}
		else
		{
			info.pixelShader = materialManager->defaultShaderHandle;
			auto& reflection = materialManager->defaultShaderReflection;
			const int textureCount = reflection.textureNameToBindSlot.size();
			info.textureCount = textureCount;
			int i = 0;
			for(auto& b : reflection.textureNameToBindSlot)
			{
				info.textureBindings[i] = b.second;
				info.textureHandles[i] = materialManager->defaultShaderHandle;
				++i;
			}
		}
			
		if (visible)
		{
			const uint32_t jobID = renderer->EnableRendering(info);
			entityToJobID[entity] = jobID;
			//Dummy-move to make the entity "dirty" so that the transform is sent to the renderer
			transformManager->Move(entity, DirectX::XMFLOAT3{ 0.0f,0.0f,0.0f });
		}
		else
		{
			renderer->DisableRendering(entityToJobID[entity]);
		}
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

	// Copy data
	memcpy(newData.entity, renderableObjectInfo.entity, renderableObjectInfo.used * sizeof(Entity));
	memcpy(newData.bufferIndex, renderableObjectInfo.bufferIndex, renderableObjectInfo.used * sizeof(size_t));
	memcpy(newData.topology, renderableObjectInfo.topology, renderableObjectInfo.used * sizeof(Graphics::RenderObjectInfo::PrimitiveTopology));
	memcpy(newData.visible, renderableObjectInfo.visible, renderableObjectInfo.used * sizeof(uint8_t));

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
	const Entity& entity = renderableObjectInfo.entity[index];
	const Entity& last_entity = renderableObjectInfo.entity[last];
	renderer->DisableRendering(entityToJobID[entity]);
	// Copy the data
	renderableObjectInfo.entity[index] = last_entity;
	renderableObjectInfo.bufferIndex[index] = renderableObjectInfo.bufferIndex[last];
	renderableObjectInfo.topology[index] = renderableObjectInfo.topology[last];
	renderableObjectInfo.visible[index] = renderableObjectInfo.visible[last];

	// Replace the index for the last_entity 
	entityToRenderableObjectInfoIndex[last_entity] = index;
	entityToRenderableObjectInfoIndex.erase(entity);
	entityToJobID[last_entity] = entityToJobID[entity];

	renderableObjectInfo.used--;
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
	
	for (auto& dirty : dirtyEntites)
	{
		auto& transform = transformManager->dirtyTransforms[dirty.transformIndex];
		renderer->UpdateTransform(entityToJobID[dirty.entity], (float*)&transform);
	}

	dirtyEntites.clear();
	StopProfile;
}

int SE::Core::RenderableManager::LoadDefaultModel(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;

	Mesh_Header* meshHeader = (Mesh_Header*)data;

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

void SE::Core::RenderableManager::LoadResource(const Utilz::GUID& meshGUID, size_t newEntry)
{
	StartProfile;
	// Load model
	auto& findBuffer = guidToBufferInfoIndex.find(meshGUID); // See if it the mesh is loaded.
	auto& bufferIndex = guidToBufferInfoIndex[meshGUID]; // Get a reference to the buffer index
	if (findBuffer == guidToBufferInfoIndex.end())	// If it wasn't loaded, load it.	
	{
		bufferInfo.push_back({ defaultMeshHandle , 0 }); // Init the texture to default texture.
		bufferIndex = bufferInfo.size() - 1;
		auto res = resourceHandler->LoadResource(meshGUID, ResourceHandler::LoadResourceDelegate::Make<RenderableManager, &RenderableManager::LoadModel>(this));
		if (res)
			Utilz::Console::Print("Model %u could not be loaded. Using default instead.\n", meshGUID);
	}
	// Increase ref Count and save the index to the material info.
	bufferInfo[bufferIndex].refCount++;
	renderableObjectInfo.bufferIndex[newEntry] = bufferIndex;
	StopProfile;
}

int SE::Core::RenderableManager::LoadModel(const Utilz::GUID& guid, void* data, size_t size)
{
	StartProfile;

	auto bufferHandle = 1;

	Mesh_Header* meshHeader = (Mesh_Header*)data;

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

	auto index = guidToBufferInfoIndex[guid];
	bufferInfo[index].bufferHandle = bufferHandle;

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
