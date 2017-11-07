#include "ParticleSystemManager.h"
#include <Profiler.h>


SE::Core::ParticleSystemManager::ParticleSystemManager(const InitializationInfo& initInfo) : initInfo(initInfo)
{
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.transformManager);
	_ASSERT(initInfo.renderableManager);
	_ASSERT(initInfo.console);
	initInfo.resourceHandler->LoadResource("ParticleGS.hlsl", [&initInfo](auto guid, void* data, size_t size) {
		initInfo.renderer->GetPipelineHandler()->CreateGeometryShader(guid, data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	initInfo.resourceHandler->LoadResource("ParticleVS.hlsl", [&initInfo](auto guid, void* data, size_t size) {
		initInfo.renderer->GetPipelineHandler()->CreateVertexShader(guid, data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	initInfo.resourceHandler->LoadResource("ParticlePS.hlsl", [&initInfo](auto guid, void* data, size_t size) {
		initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	initInfo.resourceHandler->LoadResource("ParticleGSUpdate.hlsl", [&initInfo](auto guid, void* data, size_t size) {
		initInfo.renderer->GetPipelineHandler()->CreateGeometryShaderStreamOut(guid, data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	//Pipeline for the update geometry shader
	Particle p[2];
	p[0].opacity = 1.0f;
	p[1].opacity = 1.0f;

	initInfo.renderer->GetPipelineHandler()->CreateBuffer("OutStreamBuffer1", nullptr, 0, sizeof(Particle), 10000, Graphics::BufferFlags::BIND_VERTEX | Graphics::BufferFlags::BIND_STREAMOUT);
	initInfo.renderer->GetPipelineHandler()->CreateBuffer("OutStreamBuffer2", p, 2, sizeof(Particle), 10000, Graphics::BufferFlags::BIND_VERTEX | Graphics::BufferFlags::BIND_STREAMOUT);
	updatePipeline.IAStage.vertexBuffer = "OutStreamBuffer1";
	updatePipeline.IAStage.topology = Graphics::PrimitiveTopology::POINT_LIST;
	updatePipeline.IAStage.inputLayout = "ParticleVS.hlsl";
	updatePipeline.VSStage.shader = "ParticleVS.hlsl";
	updatePipeline.GSStage.shader = "ParticleGSUpdate.hlsl";
	updatePipeline.SOStage.streamOutTarget = "OutStreamBuffer2";

	//Setting blend state
	Graphics::BlendState bs;
	bs.enable = true;
	bs.blendOperation = Graphics::BlendOperation::ADD;
	bs.blendOperationAlpha = Graphics::BlendOperation::MAX;
	bs.srcBlend = Graphics::Blend::SRC_ALPHA;
	bs.srcBlendAlpha = Graphics::Blend::ONE;
	bs.dstBlend = Graphics::Blend::INV_SRC_ALPHA;
	bs.dstBlendAlpha = Graphics::Blend::ONE;

	initInfo.renderer->GetPipelineHandler()->CreateBlendState("ParticleBlend", bs);

	//Setting particle sampler state
	Graphics::SamplerState sampState;
	sampState.addressU = Graphics::AddressingMode::CLAMP;
	sampState.addressV = Graphics::AddressingMode::CLAMP;
	sampState.addressW = Graphics::AddressingMode::CLAMP;
	sampState.filter = Graphics::Filter::LINEAR;
	sampState.maxAnisotropy = 0;
	initInfo.renderer->GetPipelineHandler()->CreateSamplerState("ParticleSampler", sampState);

	initInfo.renderer->GetPipelineHandler()->CreateDepthStencilState("noDepth", { false, false, Graphics::ComparisonOperation::NO_COMPARISON });
}


SE::Core::ParticleSystemManager::~ParticleSystemManager()
{
}

void SE::Core::ParticleSystemManager::CreateSystem(const Entity & entity, const CreateInfo & info)
{
	StartProfile;
	auto find = entityToIndex.find(entity);
	if (find == entityToIndex.end()) // The entity had no system
	{
		/*Register the entity*/
		auto newEntry = particleSystemData.size();
		indexToEntity[newEntry] = entity;

		// Register instant destroy
		initInfo.entityManager->RegisterDestroyCallback(entity, { this, &ParticleSystemManager::Destroy });

		particleSystemData[newEntry].visible = 0u;
		particleSystemData[newEntry].loaded = 0u;

		particleSystemData.push_back({});
		// Load the particle system file
		{
			auto res = initInfo.resourceHandler->LoadResource(info.systemFile, [entity, newEntry, this](auto guid, auto data, auto size) {
				memcpy(&particleSystemData[newEntry].randVelocity, data, sizeof(bool));
				memcpy(&particleSystemData[newEntry].textureName, (char*)data + sizeof(bool), sizeof(Utilz::GUID));
				memcpy(&particleSystemData[newEntry].particleFileInfo, (char*)data + sizeof(bool) + sizeof(Utilz::GUID), sizeof(particleSystemData[newEntry].particleFileInfo));
				
				return ResourceHandler::InvokeReturn::DecreaseRefcount;
			});

			if (res)
				initInfo.console->PrintChannel("Resources", "Could not load particle system file. GUID: %u, Error: %d",  info.systemFile, res);
		}
		Graphics::RenderJob updateParticleJob;
		updateParticleJob.pipeline = updatePipeline;
		updateParticleJob.mappingFunc.push_back([this, entity](int a, int b) {
			auto const entityIndex = entityToIndex.find(entity);
			if (entityIndex != entityToIndex.end())
			{
				initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("velocityBuffer", &particleSystemData[entityIndex->second].particleFileInfo, sizeof(ParticleSystemFileInfo));
			}
		});
		updateParticleJob.vertexCount = 1;
		int updateParticleJobID = initInfo.renderer->AddRenderJob(updateParticleJob, SE::Graphics::RenderGroup::PRE_PASS_0);

		//Pipeline for each particle system
		Graphics::Pipeline RPP;
		RPP.IAStage.topology = Graphics::PrimitiveTopology::POINT_LIST;
		RPP.IAStage.inputLayout = "ParticleVS.hlsl";
		RPP.IAStage.vertexBuffer = "OutStreamBuffer2";
		RPP.VSStage.shader = "ParticleVS.hlsl";
		RPP.GSStage.shader = "ParticleGS.hlsl";
		RPP.PSStage.shader = "ParticlePS.hlsl";
		RPP.PSStage.textures[0] = particleSystemData[newEntry].textureName;
		RPP.PSStage.textureBindings[0] = "fireTex";
		RPP.PSStage.textureCount = 1;
		RPP.PSStage.samplers[0] = "ParticleSampler";
		RPP.PSStage.samplerCount = 1;
		RPP.OMStage.blendState = "ParticleBlend";
		RPP.OMStage.renderTargets[0] = "backbuffer";
		RPP.OMStage.depthStencilView = "backbuffer";
		RPP.OMStage.depthStencilState = "noDepth";
		RPP.OMStage.renderTargetCount = 1;
		//Loading texture for the current particle system
		initInfo.resourceHandler->LoadResource(particleSystemData[newEntry].textureName, [this](auto guid, void* data, size_t size) {
			Graphics::TextureDesc texDesc;
			texDesc = *(Graphics::TextureDesc*)data;
			data = (char*)data + sizeof(Graphics::TextureDesc);
			initInfo.renderer->GetPipelineHandler()->CreateTexture(guid, data, texDesc.width, texDesc.height);
			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		});

		Graphics::RenderJob renderParticleJob;
		renderParticleJob.pipeline = RPP;

		initInfo.renderer->AddRenderJob(renderParticleJob, SE::Graphics::RenderGroup::RENDER_PASS_5);

	}

	StopProfile;
}

void SE::Core::ParticleSystemManager::ToggleVisible(const Entity & entity, bool visible)
{
	StartProfile;
	auto find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
	{
		//If the visibility state is switched to what it already is we dont do anything.
		if ((bool)particleSystemData[find->second].visible == visible)
		{
			ProfileReturnVoid;
		}
		if (particleSystemData[find->second].loaded == 1u) // Only show the system if it has been loaded. Otherwise wait.
		{
			// Tell renderer.
			if (visible)
			{

			}
			else
			{

			}
		}

		particleSystemData[find->second].visible = visible ? 1u : 0u;
		
		
	}
	StopProfile;
}

void SE::Core::ParticleSystemManager::Frame(Utilz::TimeCluster * timer)
{
	StartProfile;
	timer->Start(CREATE_ID_HASH("ParticleSystemManager"));
	GarbageCollection();

	while (!toUpdate.wasEmpty())
	{
		auto& top = toUpdate.top();

		auto find = entityToIndex.find(top.entity);
		if (find != entityToIndex.end())
		{
			particleSystemData[find->second].particleFileInfo = top.info;
			particleSystemData[find->second].loaded = 1u;
			if (particleSystemData[find->second].visible == 1u)
			{
				// Tell renderer
			}
		}

		toUpdate.pop();

	}
	//std::swap(updateParticleJob.pipeline.SOStage.streamOutTarget, updateParticleJob.pipeline.IAStage.vertexBuffer);
	//renderParticleJob.pipeline.IAStage.vertexBuffer = updateParticleJob.pipeline.SOStage.streamOutTarget;
	//Renderer->ChangeRenderJob(updateParticleJobID, updateParticleJob);
	//Renderer->ChangeRenderJob(RPPID, renderParticleJob);
	timer->Stop(CREATE_ID_HASH("ParticleSystemManager"));
	StopProfile;
}

void SE::Core::ParticleSystemManager::Destroy(size_t index)
{
	StartProfile;
	size_t last = particleSystemData.size() - 1;
	const Entity entity = indexToEntity[index];
	const Entity last_entity = indexToEntity[last];

	if(particleSystemData[index].visible == 1u)
		ToggleVisible(entity, false);

	// Copy the data
	indexToEntity[index] = last_entity;
	particleSystemData[index].visible = particleSystemData[last].visible;


	// Replace the index for the last_entity 
	entityToIndex[last_entity] = index;
	entityToIndex.erase(entity);

	particleSystemData.pop_back();
	StopProfile;
}

void SE::Core::ParticleSystemManager::Destroy(const Entity & entity)
{
	auto find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
		Destroy(find->second);
}

void SE::Core::ParticleSystemManager::GarbageCollection()
{
	StartProfile;
	uint8_t alive_in_row = 0;
	while (particleSystemData.size() > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, particleSystemData.size() - 1U);
		size_t i = distribution(generator);
		if (initInfo.entityManager->Alive(indexToEntity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}
