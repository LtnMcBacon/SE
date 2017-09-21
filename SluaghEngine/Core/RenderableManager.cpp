#include "RenderableManager.h"
#include <Profiler.h>
#include <Utilz\Console.h>
#include <Utilz\Delegator.h>
#include <OBJParser\Parsers.h>
#include <Core\Engine.h>
#include <MaterialManager.h>

#ifdef _DEBUG
#pragma comment(lib, "OBJParserD.lib")
#else
#pragma comment(lib, "OBJParser.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

SE::Core::RenderableManager::RenderableManager(const EntityManager& entityManager) : entityManager(entityManager)
{
	StartProfile;

	Allocate(128);
	Core::Engine::GetInstance().GetTransformManager().SetDirty.Add<RenderableManager, &RenderableManager::SetDirty>(this);
	defaultMeshHandle = 0;
	defaultShader = 0;

	auto rh = Core::Engine::GetInstance().GetResourceHandler();
	auto res = rh->LoadResource(Utilz::GUID("Placeholder_MC.obj"), ResourceHandler::LoadResourceDelegate::Make([this](const Utilz::GUID& guid, void*data, size_t size)->int {
		
		ArfData::Data arfData;
		ArfData::DataPointers arfp;
		auto r = Arf::ParseObj(data, size, &arfData, &arfp);
		if (r)
			return r;
		Arf::Mesh::Data* parsedData;
		size_t parsedSize;
		r = Arf::Interleave(arfData, arfp, &parsedData, &parsedSize);
		if (r)
			return r;

		delete arfp.buffer;

		auto& mD = *(Arf::Mesh::Data*)parsedData;
		auto defaultShader = Core::Engine::GetInstance().GetRenderer()->CreateVertexBuffer(mD.vertices, mD.NumVertices, sizeof(float) * 3 * 2 + sizeof(float) * 2);
		if (defaultShader == -1)
			return -1;

		delete parsedData;

		return 0;
	}), true);
	if (res)
		throw std::exception("Could not load default mesh.");

	res = rh->LoadResource(Utilz::GUID("SimpleVS.hlsl"), ResourceHandler::LoadResourceDelegate::Make([this](const Utilz::GUID& guid, void*data, size_t size) -> int {
		defaultShader = Core::Engine::GetInstance().GetRenderer()->CreateVertexShader(data, size);
		if (defaultShader == -1)
			return -1;
		return 0;
	}), true);
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
		auto& engine = Core::Engine::GetInstance();
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


	
		{
			// Load model
			auto& findBuffer = guidToBufferInfoIndex.find(meshGUID); // See if it the mesh is loaded.
			auto& bufferIndex = guidToBufferInfoIndex[meshGUID]; // Get a reference to the buffer index
			if (findBuffer == guidToBufferInfoIndex.end())	// If it wasn't loaded, load it.	
			{
				bufferInfo.push_back({ defaultMeshHandle , 0 }); // Init the texture to default texture.
				bufferIndex = bufferInfo.size() - 1;
				auto res = engine.GetResourceHandler()->LoadResource(meshGUID, ResourceHandler::LoadResourceDelegate::Make<RenderableManager, &RenderableManager::LoadModel>(this));
				if (res)
					Utilz::Console::Print("Model %u could not be loaded. Using default instead.\n", meshGUID);
			}
			// Increase ref Count and save the index to the material info.
			bufferInfo[bufferIndex].refCount++;
			renderableObjectInfo.bufferIndex[newEntry] = bufferIndex;
		}

		// Transform binding
		renderableObjectInfo.transformHandle[newEntry] = engine.GetRenderer()->CreateTransform();
	
		renderableObjectInfo.used++;
	}
	StopProfile;
}

void SE::Core::RenderableManager::ToggleRenderableObject(const Entity & entity, bool visible)
{
	auto r = Core::Engine::GetInstance().GetRenderer();
	// See so that the entity exist
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		Graphics::RenderObjectInfo info;
		auto vBufferIndex = renderableObjectInfo.bufferIndex[find->second];
		info.bufferHandle = bufferInfo[vBufferIndex].bufferHandle;
		info.transformHandle = renderableObjectInfo.transformHandle[find->second];
		info.vertexShader = defaultShader;
		auto& mm = Core::Engine::GetInstance().GetMaterialManager();
		auto& find = mm.entityToMaterialInfo.find(entity);
		if (find != mm.entityToMaterialInfo.end())
		{
			info.pixelShader = mm.shaderInfo[mm.materialInfo.shaderIndex[find->second]].shaderHandle;
			info.diffuseTexture = mm.textureInfo[mm.materialInfo.textureIndex[find->second]].textureHandle;
		}
		else
		{
			info.pixelShader = mm.defaultShaderHandle;
			info.diffuseTexture = mm.defaultTextureHandle;
		}
		visible ? r->EnableRendering(info) : r->DisableRendering(info);
	}
}

void SE::Core::RenderableManager::HideRenderableObject(const Entity & entity)
{
	auto r = Core::Engine::GetInstance().GetRenderer();
	// See so that the entity exist
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		Graphics::RenderObjectInfo info;
		auto vBufferIndex = renderableObjectInfo.bufferIndex[find->second];
		info.bufferHandle = bufferInfo[vBufferIndex].bufferHandle;
		info.transformHandle = renderableObjectInfo.transformHandle[find->second];
		info.vertexShader = defaultShader;
		auto& mm = Core::Engine::GetInstance().GetMaterialManager();
		auto& find = mm.entityToMaterialInfo.find(entity);
		if (find != mm.entityToMaterialInfo.end())
			info.pixelShader = mm.shaderInfo[mm.materialInfo.shaderIndex[find->second]].shaderHandle;
		else
			info.pixelShader = mm.defaultShaderHandle;
		r->DisableRendering(info);
	}
}

void SE::Core::RenderableManager::ShowRenderableObject(const Entity & entity)
{
	auto r = Core::Engine::GetInstance().GetRenderer();
	// See so that the entity exist
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		Graphics::RenderObjectInfo info;
		auto vBufferIndex = renderableObjectInfo.bufferIndex[find->second];
		info.bufferHandle = bufferInfo[vBufferIndex].bufferHandle;
		info.transformHandle = renderableObjectInfo.transformHandle[find->second];
		info.vertexShader = defaultShader;
		auto& mm = Core::Engine::GetInstance().GetMaterialManager();
		auto& find = mm.entityToMaterialInfo.find(entity);
		if (find != mm.entityToMaterialInfo.end())
			info.pixelShader = mm.shaderInfo[mm.materialInfo.shaderIndex[find->second]].shaderHandle;
		else
			info.pixelShader = mm.defaultShaderHandle;
		r->EnableRendering(info);
	}
}

void SE::Core::RenderableManager::Frame()
{
	GarbageCollection();
	UpdateDirtyTransforms();
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
	newData.transformHandle = (int*)(newData.bufferIndex + newData.allocated);

	// Copy data
	memcpy(newData.entity, renderableObjectInfo.entity, renderableObjectInfo.used * sizeof(Entity));
	memcpy(newData.bufferIndex, renderableObjectInfo.bufferIndex, renderableObjectInfo.used * sizeof(size_t));
	memcpy(newData.transformHandle, renderableObjectInfo.transformHandle, renderableObjectInfo.used * sizeof(int));

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

	// Copy the data
	renderableObjectInfo.entity[index] = last_entity;
	renderableObjectInfo.bufferIndex[index] = renderableObjectInfo.bufferIndex[last];
	renderableObjectInfo.transformHandle[index] = renderableObjectInfo.transformHandle[last];

	// Replace the index for the last_entity 
	entityToRenderableObjectInfoIndex[last_entity] = index;
	entityToRenderableObjectInfoIndex.erase(entity);

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
	auto& engine = Core::Engine::GetInstance();
	auto& tm = engine.GetTransformManager();
	auto renderer = engine.GetRenderer();

	for (auto& dirty : dirtyEntites)
	{
		auto& transform = tm.dirtyTransforms[dirty.transformIndex];
		renderer->UpdateTransform(renderableObjectInfo.transformHandle[dirty.renderableIndex], (float*)&transform);
	}

	dirtyEntites.clear();
}

int SE::Core::RenderableManager::LoadModel(const Utilz::GUID& guid, void* data, size_t size)
{
	StartProfile;
	ArfData::Data arfData;
	ArfData::DataPointers arfp;
	auto r = Arf::ParseObj(data, size, &arfData, &arfp);
	if (r)
		ProfileReturnConst( r);
	Arf::Mesh::Data* parsedData;
	size_t parsedSize;
	r = Arf::Interleave(arfData, arfp, &parsedData, &parsedSize);
	if (r)
		ProfileReturnConst( r);

	delete arfp.buffer;

	auto& mD = *(Arf::Mesh::Data*)parsedData;
	auto bufferHandle = Core::Engine::GetInstance().GetRenderer()->CreateVertexBuffer(mD.vertices, mD.NumVertices, sizeof(float)*3*2 + sizeof(float)*2);
	if (bufferHandle == -1)
		ProfileReturnConst(-1);

	auto index = guidToBufferInfoIndex[guid];
	bufferInfo[index].bufferHandle = bufferHandle;

	delete parsedData;

	ProfileReturnConst(0);
}

void SE::Core::RenderableManager::SetDirty(const Entity & entity, size_t index)
{
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		dirtyEntites.push_back({ index, find->second });
	}
}
