#include "DebugRenderManager.h"
#include <Profiler.h>

SE::Core::DebugRenderManager::DebugRenderManager(Graphics::IRenderer* renderer, ResourceHandler::IResourceHandler* resourceHandler, const EntityManager& entityManager,
	TransformManager* transformManager) : entityManager(entityManager), transformManager(transformManager), renderer(renderer), resourceHandler(resourceHandler), dynamicVertexBufferHandle(-1), dirty(false)
{
	dynamicVertexBufferHandle = renderer->CreateDynamicVertexBuffer(dynamicVertexBufferSize, sizeof(Point3D));
	_ASSERT_EXPR(dynamicVertexBufferHandle >= 0, L"Failed to initialize DebugRenderManager: Could not create dynamic vertex buffer");
	auto res = resourceHandler->LoadResource(Utilz::GUID("DebugLinePS.hlsl"), ResourceHandler::LoadResourceDelegate::Make<DebugRenderManager, &DebugRenderManager::LoadLinePixelShader>(this));
	if (res)
		throw std::exception("Could not load line render pixel shader.");
	res = resourceHandler->LoadResource(Utilz::GUID("DebugLineVS.hlsl"), ResourceHandler::LoadResourceDelegate::Make<DebugRenderManager, &DebugRenderManager::LoadLineVertexShader>(this));
	if (res)
		throw std::exception("Could not load line render vertex shader.");

	transformManager->SetDirty.Add<DebugRenderManager, &DebugRenderManager::SetDirty>(this);
	
}

SE::Core::DebugRenderManager::~DebugRenderManager()
{
}

void SE::Core::DebugRenderManager::Frame(Utilz::StackAllocator& perFrameStackAllocator)
{
	StartProfile;
	if(dirty)
	{
		size_t bufferSize = 0;
		for (auto& m : entityToLineList)
			bufferSize += m.second.size();
		bufferSize *= sizeof(LineSegment);
		void* lineData = perFrameStackAllocator.GetMemoryAligned(bufferSize, 4);
		if (!lineData)
			ProfileReturnVoid;
		void* cur = lineData;
		for (auto& m : entityToLineList)
		{
			const size_t cpySize = m.second.size() * sizeof(LineSegment);
			memcpy(cur, m.second.data(), cpySize);
			cur = ((uint8_t*)cur) + cpySize;
		}
		renderer->UpdateDynamicVertexBuffer(dynamicVertexBufferHandle, lineData, bufferSize, sizeof(Point3D));

		for(auto& job : entityToJobID)
		{
			renderer->RemoveLineRenderJob(job.second);
		}
		entityToJobID.clear();

		uint32_t startVertex = 0;
		for(auto& m : entityToLineList)
		{
			Graphics::LineRenderJob lineRenderJob;
			lineRenderJob.firstVertex = startVertex;
			lineRenderJob.pixelShaderHandle = lineRenderPixelShaderHandle;
			lineRenderJob.vertexShaderHandle = lineRenderVertexShaderHandle;
			lineRenderJob.transform = transformManager->GetTransform(m.first);
			lineRenderJob.vertexBufferHandle = dynamicVertexBufferHandle;
			const size_t verticesToDraw = m.second.size() * 2;
			lineRenderJob.verticesToDrawCount = verticesToDraw;
			startVertex += verticesToDraw;
			entityToJobID[m.first] = renderer->AddLineRenderJob(lineRenderJob);
		}

		dirty = false;
	}
	ProfileReturnVoid;
}

void SE::Core::DebugRenderManager::ToggleDebugRendering(const Entity& entity, bool enable)
{
	StartProfile;
	if (!enable)
	{
		entityToLineList.erase(entity);
		auto find = entityToJobID.find(entity);
		if (find != entityToJobID.end())
			renderer->RemoveLineRenderJob(find->second);
		entityToJobID.erase(entity);
	}
	ProfileReturnVoid;
}

void SE::Core::DebugRenderManager::DrawCross(const Entity& entity, float scale, float x, float y, float z)
{
	StartProfile;
	const LineSegment lines[3] = { {{ x - scale, y, z }	,{ x + scale, y, z }},
	{{ x, y, z - scale },{ x, y, z + scale } },
	{{ x, y - scale, z },{ x, y + scale, z } } };
	auto& lineList = entityToLineList[entity];
	for (int i = 0; i < 3; i++)
		lineList.push_back(lines[i]);
	dirty = true;
	ProfileReturnVoid;
}

void SE::Core::DebugRenderManager::DrawLine(const Entity& entity, float x1, float y1, float z1, float x2, float y2, float z2)
{
	StartProfile;
	const Point3D a = { x1,y1,z1 };
	const Point3D b = { x2,y2,z2 };
	const LineSegment l = { a, b };
	entityToLineList[entity].push_back(l);
	dirty = true;
	ProfileReturnVoid;
}

int SE::Core::DebugRenderManager::LoadLineVertexShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	lineRenderVertexShaderHandle = renderer->CreateVertexShader(data, size);
	ProfileReturn(lineRenderVertexShaderHandle < 0);
	
}

int SE::Core::DebugRenderManager::LoadLinePixelShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	lineRenderPixelShaderHandle = renderer->CreatePixelShader(data, size);
	ProfileReturn(lineRenderPixelShaderHandle < 0);
	
}

void SE::Core::DebugRenderManager::SetDirty(const Entity& entity, size_t index)
{
	StartProfile;
	auto find = entityToJobID.find(entity);
	if (find != entityToJobID.end())
	{
		DirectX::XMFLOAT4X4 transform = transformManager->GetTransform(entity);
		renderer->UpdateLineRenderJob(find->second, (float*)&transform);
	}
	ProfileReturnVoid;
}

