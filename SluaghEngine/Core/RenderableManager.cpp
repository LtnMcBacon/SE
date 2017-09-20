#include "RenderableManager.h"
#include <Profiler.h>
#include <Utilz\Console.h>
#include <Utilz\Delegator.h>
#include <OBJParser\Parsers.h>
#include <Core\Engine.h>

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
	Core::Engine::GetInstance().GetResourceHandler()->AddParser(Utilz::GUID("obj"), [](void* rawData, size_t rawSize, void** parsedData, size_t* parsedSize) -> int
	{
		ArfData::Data arfData;
		ArfData::DataPointers arfp;
		auto r = Arf::ParseObj(rawData, rawSize, &arfData, &arfp);
		if (r)
			return r;
		auto data = (Arf::Mesh::Data**)parsedData;
		r = Arf::Interleave(arfData, arfp, data, parsedSize, ~0u);
		if (r)
			return r;

		operator delete(arfp.buffer);

		return 0;

	});
	Core::Engine::GetInstance().GetTransformManager().SetDirty.Add<RenderableManager, &RenderableManager::SetDirty>(this);

	StopProfile;
}


SE::Core::RenderableManager::~RenderableManager()
{
	operator delete(renderableObjectInfo.data);

}

void SE::Core::RenderableManager::CreateRenderableObject(const Entity & entity, const CreateRenderObjectInfo & info)
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
			Allocate(renderableObjectInfo.allocated * 4);

		// Register the entity
		size_t newEntry = renderableObjectInfo.used;
		entityToRenderableObjectInfoIndex[entity] = newEntry;
		renderableObjectInfo.entity[newEntry] = entity;


		// Load data
		auto& findBuffer = guidToBufferInfoIndex.find(info.meshGUID);
		if (findBuffer == guidToBufferInfoIndex.end())	// If not loaded load it.	
			engine.GetResourceHandler()->LoadResource(info.meshGUID, ResourceHandler::LoadResourceDelegate::Make<RenderableManager, &RenderableManager::AddResource>(this));
	
		// Increase ref Count
		auto vBufferIndex = guidToBufferInfoIndex[info.meshGUID];
		bufferInfo[vBufferIndex].refCount++;
		renderableObjectInfo.bufferIndex[newEntry] = vBufferIndex;

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
		info.bufferHandle = renderableObjectInfo.bufferIndex[find->second];
		info.transformHandle = renderableObjectInfo.transformHandle[find->second];
		visible ? r->EnableRendering(info) : r->DisableRendering(info);
	}
}

void SE::Core::RenderableManager::HideRenderableObject(const Entity & entity)
{
}

void SE::Core::RenderableManager::ShowRenderableObject(const Entity & entity)
{
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
	newData.bufferIndex = (size_t*)(newData.entity + 1);
	newData.transformHandle = (int*)(newData.bufferIndex + 1);

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

void SE::Core::RenderableManager::AddResource(const Utilz::GUID& guid, void* data, size_t size)
{
	StartProfile;
	auto mD = (Arf::Mesh::Data*)data;
	auto bufferHandle = Core::Engine::GetInstance().GetRenderer()->CreateVertexBuffer(mD->vertices, mD->NumVertices, sizeof(float)*3*2 + sizeof(float)*2);
	bufferInfo.push_back({ 0, bufferHandle });
	guidToBufferInfoIndex[guid] = bufferInfo.size() - 1;
	StopProfile;
}

void SE::Core::RenderableManager::SetDirty(const Entity & entity, size_t index)
{
	auto& find = entityToRenderableObjectInfoIndex.find(entity);
	if (find != entityToRenderableObjectInfoIndex.end())
	{
		dirtyEntites.push_back({ index, find->second });
	}
}
