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
	auto res = initInfo.resourceHandler->LoadResource("ParticleGS.hlsl", [&initInfo](auto guid, void* data, size_t size) {
			auto res = initInfo.renderer->GetPipelineHandler()->CreateGeometryShader(guid, data, size);
			if (res < 0)
			{
				return ResourceHandler::InvokeReturn::FAIL;
			}
				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
		});
	if (res < 0)
	{
		throw std::exception("Failed to create ParticleGS");
	}

	res = initInfo.resourceHandler->LoadResource("ParticleVS.hlsl", [&initInfo](auto guid, void* data, size_t size) {
			auto res = initInfo.renderer->GetPipelineHandler()->CreateVertexShader(guid, data, size);
			if (res < 0)
			{
				return ResourceHandler::InvokeReturn::FAIL;
			}
			return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
		});
	if (res < 0)
	{
		throw std::exception("Failed to create ParticleVS");
	}
	res = initInfo.resourceHandler->LoadResource("ParticlePS.hlsl", [&initInfo](auto guid, void* data, size_t size) {
			auto res = initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
			if (res < 0)
			{
				return ResourceHandler::InvokeReturn::FAIL;
			}
			return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	if (res < 0)
	{
		throw std::exception("Failed to create ParticlePS");
	}
	res = initInfo.resourceHandler->LoadResource("ParticleGSUpdate.hlsl", [&initInfo](auto guid, void* data, size_t size) {
			auto res =initInfo.renderer->GetPipelineHandler()->CreateGeometryShaderStreamOut(guid, data, size);
			if (res < 0)
			{
				return ResourceHandler::InvokeReturn::FAIL;
			}
			return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	if (res < 0)
	{
		throw std::exception("Failed to create ParticleGSUpdate");
	}
	//Pipeline for the update geometry shader
	Particle p;
	p.opacity = 1.0f;


	res = initInfo.renderer->GetPipelineHandler()->CreateBuffer("OutStreamBuffer1", nullptr, 0, sizeof(Particle), 10000, Graphics::BufferFlags::BIND_VERTEX | Graphics::BufferFlags::BIND_STREAMOUT);
	if (res < 0)
	{
		throw std::exception("Failed to create OutStreamBuffer1");
	}
	res = initInfo.renderer->GetPipelineHandler()->CreateBuffer("OutStreamBuffer2", &p, 1, sizeof(Particle), 10000, Graphics::BufferFlags::BIND_VERTEX | Graphics::BufferFlags::BIND_STREAMOUT);
	if (res < 0)
	{
		throw std::exception("Failed to create OutStreamBuffer2");
	}
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

	res = initInfo.renderer->GetPipelineHandler()->CreateBlendState("ParticleBlend", bs);
	if (res < 0)
	{
		throw std::exception("Failed to create particle blend state");
	}

	//Setting particle sampler state
	Graphics::SamplerState sampState;
	sampState.addressU = Graphics::AddressingMode::CLAMP;
	sampState.addressV = Graphics::AddressingMode::CLAMP;
	sampState.addressW = Graphics::AddressingMode::CLAMP;
	sampState.filter = Graphics::Filter::LINEAR;
	sampState.maxAnisotropy = 0;
	res = initInfo.renderer->GetPipelineHandler()->CreateSamplerState("ParticleSampler", sampState);
	if (res < 0)
	{
		throw std::exception("Failed to create particle SamplerState");
	}


	res = initInfo.renderer->GetPipelineHandler()->CreateDepthStencilState("noDepth", { false, false, Graphics::ComparisonOperation::NO_COMPARISON });
	if (res < 0)
	{
		throw std::exception("Failed to create particle DepthStencilState");
	}
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
		indexToEntity.push_back(entity);
		entityToIndex[entity] = newEntry;

		particleSystemData.push_back({});
		// Register instant destroy
		initInfo.entityManager->RegisterDestroyCallback(entity, { this, &ParticleSystemManager::Destroy });

		particleSystemData[newEntry].visible = 0u;
		particleSystemData[newEntry].loaded = 0u;

		// Load the particle system file
		{
			auto res = initInfo.resourceHandler->LoadResource(info.systemFile, [entity, newEntry, this](auto guid, auto data, auto size) {
				memcpy(&particleSystemData[newEntry].randVelocity, data, sizeof(bool));
				memcpy(&particleSystemData[newEntry].textureName, (char*)data + sizeof(bool), sizeof(Utilz::GUID));
				memcpy(&particleSystemData[newEntry].particleFileInfo, (char*)data + sizeof(bool) + sizeof(Utilz::GUID), sizeof(ParticleSystemFileInfo));
				memcpy(&particleSystemData[newEntry].velocityRange, (char*)data + sizeof(bool) + sizeof(Utilz::GUID) + sizeof(ParticleSystemFileInfo), sizeof(DirectX::XMFLOAT2) * 3);
				memcpy(&particleSystemData[newEntry].emitRange, (char*)data + sizeof(bool) + sizeof(Utilz::GUID) + sizeof(ParticleSystemFileInfo) + (sizeof(DirectX::XMFLOAT2) * 3), sizeof(DirectX::XMFLOAT2) * 3);
				
				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
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
				auto& fileInfo = particleSystemData[entityIndex->second].particleFileInfo;
				auto& PSD = particleSystemData[entityIndex->second];
				fileInfo.vel[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (PSD.velocityRange[0].x/*Max*/ - PSD.velocityRange[0].y/*Min*/) + PSD.velocityRange[0].y;
				fileInfo.vel[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (PSD.velocityRange[1].x/*Max*/ - PSD.velocityRange[1].y/*Min*/) + PSD.velocityRange[1].y;
				fileInfo.vel[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (PSD.velocityRange[2].x/*Max*/ - PSD.velocityRange[2].y/*Min*/) + PSD.velocityRange[2].y;

				fileInfo.emitRange[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (PSD.emitRange[0].x/*Max*/ - PSD.emitRange[0].y/*Min*/) + PSD.emitRange[0].y;
				fileInfo.emitRange[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (PSD.emitRange[1].x/*Max*/ - PSD.emitRange[1].y/*Min*/) + PSD.emitRange[1].y;
				fileInfo.emitRange[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * (PSD.emitRange[2].x/*Max*/ - PSD.emitRange[2].y/*Min*/) + PSD.emitRange[2].y;
				initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("velocityBuffer", &fileInfo, sizeof(ParticleSystemFileInfo));
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
		RPP.PSStage.textureBindings[0] = "particleTex";
		RPP.PSStage.textureCount = 1;
		RPP.PSStage.samplers[0] = "ParticleSampler";
		RPP.PSStage.samplerCount = 1;
		RPP.OMStage.blendState = "ParticleBlend";
		RPP.OMStage.renderTargets[0] = "backbuffer";
		RPP.OMStage.depthStencilView = "backbuffer";
		RPP.OMStage.depthStencilState = "noDepth";
		RPP.OMStage.renderTargetCount = 1;
		//Loading texture for the current particle system
		auto res = initInfo.resourceHandler->LoadResource(particleSystemData[newEntry].textureName, [this](auto guid, void* data, size_t size) {
					Graphics::TextureDesc texDesc;
					texDesc = *(Graphics::TextureDesc*)data;
					data = (char*)data + sizeof(Graphics::TextureDesc);
					auto res =	initInfo.renderer->GetPipelineHandler()->CreateTexture(guid, data, texDesc.width, texDesc.height);
					if (res < 0)
					{
						return ResourceHandler::InvokeReturn::FAIL;
					}
					return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
		});
		if (res < 0)
		{
			throw std::exception("Failed to load particle texture");
		}

		Graphics::RenderJob renderParticleJob;
		renderParticleJob.pipeline = RPP;

		int renderParticleJobID = initInfo.renderer->AddRenderJob(renderParticleJob, SE::Graphics::RenderGroup::RENDER_PASS_5);

		//Storing render jobs and job IDs
		particleSystemData[newEntry].updateJob = updateParticleJob;
		particleSystemData[newEntry].updateJobID = updateParticleJobID;
		particleSystemData[newEntry].renderJob = renderParticleJob;
		particleSystemData[newEntry].renderJobID = renderParticleJobID;
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
	timer->Start(("ParticleSystemManager"));
	GarbageCollection();

	//while (!toUpdate.wasEmpty())
	//{
	//	auto& top = toUpdate.top();

	//	auto find = entityToIndex.find(top.entity);
	//	if (find != entityToIndex.end())
	//	{
	//		particleSystemData[find->second].particleFileInfo = top.info;
	//		particleSystemData[find->second].loaded = 1u;
	//		if (particleSystemData[find->second].visible == 1u)
	//		{
	//			// Tell renderer
	//		}
	//	}

	//	toUpdate.pop();

	//}
	//Swapping the buffers for the update and render particle jobs
	for (size_t i = 0; i < particleSystemData.size(); i++)
	{
		static int k = 0;
		if (k == 1)
		{

			particleSystemData[i].updateJob.vertexCount = 0;
			initInfo.renderer->ChangeRenderJob(particleSystemData[i].updateJobID, particleSystemData[i].updateJob);
		}
		if (k >= 1)
		{
			std::swap(particleSystemData[i].updateJob.pipeline.SOStage.streamOutTarget, particleSystemData[i].updateJob.pipeline.IAStage.vertexBuffer);
			particleSystemData[i].renderJob.pipeline.IAStage.vertexBuffer = particleSystemData[i].updateJob.pipeline.SOStage.streamOutTarget;
			initInfo.renderer->ChangeRenderJob(particleSystemData[i].updateJobID, particleSystemData[i].updateJob);
			initInfo.renderer->ChangeRenderJob(particleSystemData[i].renderJobID, particleSystemData[i].renderJob);
		}
		k++;
	}

	timer->Stop(("ParticleSystemManager"));
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
