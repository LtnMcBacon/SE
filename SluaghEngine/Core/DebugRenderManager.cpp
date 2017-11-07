#include "DebugRenderManager.h"
#include <Profiler.h>
#include <random>


SE::Core::DebugRenderManager::DebugRenderManager(const InitializationInfo & initInfo) : initInfo(initInfo)
{
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.perFrameStackAllocator);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.transformManager);
	_ASSERT(initInfo.collisionManager);

	
	vertexShaderID = "DebugLineVS.hlsl";
	pixelShaderID = "DebugLinePS.hlsl";
	transformBufferID = "DebugLineW";
	initInfo.resourceHandler->LoadResource(pixelShaderID, [this](auto guid, auto data, auto size) {
		this->initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	initInfo.resourceHandler->LoadResource(vertexShaderID, [this](auto guid, auto data, auto size) {
		this->initInfo.renderer->GetPipelineHandler()->CreateVertexShader(guid, data, size);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	initInfo.transformManager->RegisterSetDirty({ this, &DebugRenderManager::SetDirty });

	auto pipelineHandler = initInfo.renderer->GetPipelineHandler();

	vertexBufferID = Utilz::GUID("DebugRenderManager");
	pipelineHandler->CreateVertexBuffer(vertexBufferID, nullptr, maximumLinesToRender * 2, sizeof(Point3D), true);
	

	pipeline.IAStage.topology = Graphics::PrimitiveTopology::LINE_LIST;
	pipeline.IAStage.vertexBuffer = vertexBufferID;
	pipeline.IAStage.inputLayout = vertexShaderID;
	pipeline.VSStage.shader = vertexShaderID;
	//pipeline.VSStage.constantBuffers[0] = transformBufferID; //Created when shader is created.
	//pipeline.VSStage.constantBuffers[1] = "OncePerFrame"; //Updated by camera manager
	//pipeline.VSStage.constantBufferCount = 2;
	pipeline.PSStage.shader = pixelShaderID;
	pipeline.OMStage.renderTargets[0] = "backbuffer";
	pipeline.OMStage.renderTargetCount = 1;
	pipeline.OMStage.depthStencilView = "backbuffer";
}

SE::Core::DebugRenderManager::~DebugRenderManager()
{
}

void SE::Core::DebugRenderManager::Frame(Utilz::TimeCluster * timer)
{
	StartProfile;
	timer->Start(("DebugRenderManager"));
	GarbageCollection();
	CreateBoundingBoxes();
	if (dirty)
	{
		size_t bufferSize = 0;
		for (auto& m : entityToLineList)
			bufferSize += m.second.size();
		bufferSize *= sizeof(LineSegment);
		void* lineData = initInfo.perFrameStackAllocator->GetMemoryAligned(bufferSize, 4);
		if (!lineData)
			ProfileReturnVoid;
		void* cur = lineData;
		for (auto& m : entityToLineList)
		{
			const size_t cpySize = m.second.size() * sizeof(LineSegment);
			memcpy(cur, m.second.data(), cpySize);
			cur = ((uint8_t*)cur) + cpySize;
		}
		
		initInfo.renderer->GetPipelineHandler()->UpdateDynamicVertexBuffer(vertexBufferID, lineData, bufferSize);
		uint32_t startVertex = 0;
		for (auto& m : entityToLineList)
		{
			const size_t verticesToDraw = m.second.size() * 2;
			Graphics::RenderJob job;
			job.pipeline = pipeline;
			Entity ent = m.first;
			job.vertexOffset = startVertex;
			job.vertexCount = verticesToDraw;
			job.mappingFunc .push_back( [this, ent](int a, int b)
			{
				initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer(transformBufferID, &cachedTransforms[ent], sizeof(DirectX::XMFLOAT4X4));
			});
			auto f = entityToJobID.find(m.first);
			if (f == entityToJobID.end())
			{
				
				entityToJobID[m.first] = initInfo.renderer->AddRenderJob(job, Graphics::RenderGroup::RENDER_PASS_3);
			}
			else
			{
				job.vertexOffset = startVertex;
				job.vertexCount = verticesToDraw;
				initInfo.renderer->ChangeRenderJob(f->second, job);
			}
			startVertex += verticesToDraw;
		}
		dirty = false;
	}
	timer->Stop(("DebugRenderManager"));
	ProfileReturnVoid;
}

bool SE::Core::DebugRenderManager::ToggleDebugRendering(const Entity& entity, bool enable)
{
	StartProfile;
	if (!enable)
	{
		lineCount -= entityToLineList[entity].size();
		entityToLineList.erase(entity);
		auto find = entityToJobID.find(entity);
		if (find != entityToJobID.end())
			initInfo.renderer->RemoveRenderJob(find->second);
		entityToJobID.erase(entity);
		//In case we don't leave it up to the caller to not enable the same entity twice
		entityRendersBoundingVolume.erase(entity);
		dirty = true;
	}
	else
	{
		//In case we don't leave it up to the caller to not enable the same entity twice
		const auto alreadyRendering = entityRendersBoundingVolume.find(entity);
		if (alreadyRendering != entityRendersBoundingVolume.end())
			ProfileReturnConst(false);
		initInfo.transformManager->Create(entity);
		initInfo.transformManager->SetAsDirty(entity);
		entityRendersBoundingVolume.insert(entity);
		bool found = false;
		for (int i = 0; i < awaitingBoundingBoxes.size(); ++i)
			if (awaitingBoundingBoxes[i] == entity)
				found = true;
		
		if(!found)
			awaitingBoundingBoxes.push_back(entity);
	}
	ProfileReturnConst(true);
}

void SE::Core::DebugRenderManager::DrawCross(const Entity& entity, float scale, float x, float y, float z)
{
	StartProfile;
	if (lineCount + 3 > maximumLinesToRender)
		ProfileReturnVoid;
	lineCount += 3;
	const LineSegment lines[3] = { {{ x - scale, y, z }	,{ x + scale, y, z }},
	{{ x, y, z - scale },{ x, y, z + scale } },
	{{ x, y - scale, z },{ x, y + scale, z } } };
	auto& lineList = entityToLineList[entity];
	for (int i = 0; i < 3; i++)
		lineList.push_back(lines[i]);
	dirty = true;
	ProfileReturnVoid;
}

void SE::Core::DebugRenderManager::DrawLine(const Entity& entity, const Point3D& a, const Point3D& b)
{
	StartProfile;
	if (lineCount + 1 > maximumLinesToRender)
		ProfileReturnVoid;
	lineCount += 1;
	const LineSegment l = { a, b };
	entityToLineList[entity].push_back(l);
	dirty = true;
	ProfileReturnVoid;
}



void SE::Core::DebugRenderManager::CreateBoundingBoxes()
{
	for (int i = 0; i < awaitingBoundingBoxes.size(); i++)
	{
		DirectX::BoundingBox aabb;
		const bool hasBounding = initInfo.collisionManager->GetLocalBoundingBox(awaitingBoundingBoxes[i], &aabb);
		if (hasBounding && lineCount + 12 < maximumLinesToRender)
		{
			const auto& center = aabb.Center;
			auto ex = aabb.Extents;

			const DirectX::XMFLOAT3 mic = { center.x - ex.x, center.y - ex.y, center.z - ex.z };
			const DirectX::XMFLOAT3 mac = { center.x + ex.x, center.y + ex.y, center.z + ex.z };
			ex.x *= 2;
			ex.y *= 2;
			ex.z *= 2;
			const LineSegment lines[12] = {
				{ { mic.x, mic.y, mic.z },{ mic.x + ex.x,mic.y,mic.z } },
				{ { mic.x + ex.x,mic.y,mic.z },{ mic.x + ex.x,mic.y,mic.z + ex.z } },
				{ { mic.x + ex.x,mic.y,mic.z + ex.z },{ mic.x,mic.y,mic.z + ex.z } },
				{ { mic.x,mic.y,mic.z + ex.z },{ mic.x,mic.y,mic.z } },

				{ { mic.x,mic.y,mic.z },{ mic.x,mic.y + ex.y,mic.z } },
				{ { mic.x + ex.x,mic.y,mic.z },{ mic.x + ex.x,mic.y + ex.y,mic.z } },
				{ { mic.x,mic.y,mic.z + ex.z },{ mic.x,mic.y + ex.y,mic.z + ex.z } },
				{ { mic.x + ex.x,mic.y,mic.z + ex.z },{ mic.x + ex.x,mic.y + ex.y,mic.z + ex.z } },

				{ { mac.x, mac.y, mac.z },{ mac.x - ex.x,mac.y,mac.z } },
				{ { mac.x - ex.x, mac.y,mac.z },{ mac.x - ex.x,mac.y,mac.z - ex.z } },
				{ { mac.x - ex.x,mac.y,mac.z - ex.z },{ mac.x,mac.y,mac.z - ex.z } },
				{ { mac.x, mac.y, mac.z - ex.z },{ mac.x,mac.y,mac.z } }
			};
			auto& lineList = entityToLineList[awaitingBoundingBoxes[i]];
			for (int i = 0; i < 12; ++i)
			{
				lineList.push_back(lines[i]);
			}
			dirty = true;
			lineCount += 12;
			awaitingBoundingBoxes[i] = awaitingBoundingBoxes.back();
			awaitingBoundingBoxes.pop_back();
			--i;
		}
	}
}

void SE::Core::DebugRenderManager::SetDirty(const Entity& entity, size_t index)
{
	StartProfile;
	//const auto find = entityToLineList.find(entity);
	//if (find != entityToLineList.end())
	{
		DirectX::XMStoreFloat4x4(&cachedTransforms[entity], DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&initInfo.transformManager->GetCleanedTransforms()[index])));
	}
	ProfileReturnVoid;
}

void SE::Core::DebugRenderManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	size_t activeJobs = entityToJobID.size();
	while (activeJobs > 0 && alive_in_row < 40U)
	{
		const std::uniform_int_distribution<uint32_t> distribution(0U, activeJobs - 1U);
		uint32_t i = distribution(generator);
		auto iterator = entityToJobID.begin();
		std::advance(iterator, i);
		if (initInfo.entityManager->Alive(iterator->first))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(iterator->first);
		--activeJobs;
	}
	ProfileReturnVoid;
}

void SE::Core::DebugRenderManager::Destroy(size_t index)
{

}

void SE::Core::DebugRenderManager::Destroy(const Entity& e)
{
	StartProfile;
	initInfo.renderer->RemoveRenderJob(entityToJobID[e]);
	entityToLineList.erase(e);
	entityToJobID.erase(e);
	cachedTransforms.erase(e);
	dirty = true;
	ProfileReturnVoid;
}

