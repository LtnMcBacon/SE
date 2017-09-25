#include "DebugRenderManager.h"

SE::Core::DebugRenderManager::DebugRenderManager(Graphics::IRenderer* renderer, const EntityManager& entityManager,
	TransformManager* transformManager) : entityManager(entityManager), transformManager(transformManager), renderer(renderer), dirty(false), dynamicVertexBufferHandle(-1)
{
	dynamicVertexBufferHandle = renderer->CreateDynamicVertexBuffer(dynamicVertexBufferSize, sizeof(LineSegment));
	_ASSERT_EXPR(dynamicVertexBufferHandle >= 0, L"Failed to initialize DebugRenderManager: Could not create dynamic vertex buffer");
	
}

SE::Core::DebugRenderManager::~DebugRenderManager()
{
}

void SE::Core::DebugRenderManager::Frame(Utilz::StackAllocator& perFrameStackAllocator)
{
	if(dirty)
	{
		size_t bufferSize = 0;
		for (auto& m : entityToLineList)
			bufferSize += m.second.size();
		bufferSize *= sizeof(LineSegment);
		void* lineData = perFrameStackAllocator.GetMemoryAligned(bufferSize, 4);
		if (!lineData)
			return;
		void* cur = lineData;
		for (auto& m : entityToLineList)
		{
			const size_t cpySize = m.second.size() * sizeof(LineSegment);
			memcpy(cur, m.second.data(), cpySize);
			cur = ((uint8_t*)cur) + cpySize;
		}
		renderer->UpdateDynamicVertexBuffer(dynamicVertexBufferHandle, lineData, bufferSize, sizeof(LineSegment));
	}
}

void SE::Core::DebugRenderManager::ToggleDebugRendering(const Entity& entity, bool enable)
{
	if (!enable)
		entityToLineList.erase(entity);
}

void SE::Core::DebugRenderManager::DrawCross(const Entity& entity, float scale, float x, float y, float z)
{
	const LineSegment lines[6] = { {{ x - scale, y, z }	,{ x + scale, y, z }},
	{{ x, y, z - scale },{ x, y, z + scale } },
	{{ x, y - scale, z },{ x, y + scale, z } } };
	auto& lineList = entityToLineList[entity];
	for (auto& p : lines)
		lineList.push_back(p);
	dirty = true;
}

void SE::Core::DebugRenderManager::DrawLine(const Entity& entity, float x1, float y1, float z1, float x2, float y2, float z2)
{
	const Point3D a = { x1,y1,z1 };
	const Point3D b = { x2,y2,z2 };
	const LineSegment l = { a, b };
	entityToLineList[entity].push_back(l);
	dirty = true;
}

