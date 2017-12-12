#include "DecalManager.h"
#include <Profiler.h>
#include <random>
#include <algorithm>

using namespace SE::Graphics;

SE::Core::DecalManager::DecalManager(const IDecalManager::InitializationInfo& initInfo)
{
	this->initInfo = initInfo;

	vertexShader = "DecalVS.hlsl";
	pixelShader = "DecalPS.hlsl";
	worldConstantBuffer = "DecalsWorld";
	inverseViewProj = "DecalsInverseViewProj";
	inverseWorld = "DecalsInverseWorld";
	opacities = "DecalOpacity";
	blendState = "DecalBlend";
	textureBindName = "DecalDiffuse";
	rasterizerState = "DecalRS";

	auto res = this->initInfo.resourceHandler->LoadResource(vertexShader, [this](auto guid, void* data, size_t size)
	{
		auto res = this->initInfo.renderer->GetPipelineHandler()->CreateVertexShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::InvokeReturn::FAIL;
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});

	if (res < 0)
		throw std::exception("Could not load decal vertex shader");

	res = this->initInfo.resourceHandler->LoadResource(pixelShader, [this](auto guid, void* data, size_t size)
	{
		auto res = this->initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::InvokeReturn::FAIL;
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	if (res < 0)
		throw std::exception("Could not load decal pixel shader");
	Graphics::BlendState bs;
	bs.enable = true;
	bs.blendOperation = Graphics::BlendOperation::ADD;
	bs.blendOperationAlpha = Graphics::BlendOperation::MAX;
	bs.srcBlend = Graphics::Blend::SRC_ALPHA;
	bs.srcBlendAlpha = Graphics::Blend::ONE;
	bs.dstBlend = Graphics::Blend::INV_SRC_ALPHA;
	bs.dstBlendAlpha = Graphics::Blend::ONE;
	
	res = this->initInfo.renderer->GetPipelineHandler()->CreateBlendState(blendState, bs);
	if (res < 0)
		throw std::exception("Could not Create decal blendstate");

	RasterizerState rsState;
	rsState.cullMode = CullMode::CULL_NONE;
	rsState.fillMode = FillMode::FILL_SOLID;
	rsState.windingOrder = WindingOrder::CLOCKWISE;
	res = this->initInfo.renderer->GetPipelineHandler()->CreateRasterizerState(rasterizerState, rsState);
	if (res < 0)
		throw std::exception("Could not load decal rasterizer state");
	
	defaultPipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_STRIP;
	defaultPipeline.VSStage.shader = vertexShader;
	defaultPipeline.PSStage.textureBindings[0] = "DepthBuffer";
	defaultPipeline.PSStage.textureBindings[1] = textureBindName;
	defaultPipeline.PSStage.textureBindings[2] = "ShadowMap";
	defaultPipeline.PSStage.textures[0] = "backbufferdepth";
	//The texture name will be set for each decal that uses a different texture
	defaultPipeline.PSStage.textures[2] = "DepthCube";
	defaultPipeline.PSStage.textureCount = 2;
	defaultPipeline.PSStage.shader = pixelShader;
	defaultPipeline.OMStage.renderTargets[0] = "backbuffer";
	defaultPipeline.OMStage.renderTargetCount = 1;
	defaultPipeline.OMStage.depthStencilView = "";
	defaultPipeline.RStage.rasterizerState = rasterizerState;
	defaultPipeline.OMStage.blendState = blendState;
	
	DirectX::XMFLOAT4X4 vp = initInfo.cameraManager->GetViewProjection(initInfo.cameraManager->GetActive());

	cachedViewProj = DirectX::XMLoadFloat4x4(&vp);

	initInfo.transformManager->RegisterSetDirty({ this, &DecalManager::SetDirty });
	initInfo.eventManager->RegisterToToggleVisible({ this, &DecalManager::ToggleVisible });
}

SE::Core::DecalManager::~DecalManager()
{

}

void SE::Core::DecalManager::Frame(Utilz::TimeCluster* timer)
{
	StartProfile;
	GarbageCollection();
	DirectX::XMFLOAT4X4 vp = initInfo.cameraManager->GetViewProjection(initInfo.cameraManager->GetActive());
	const DirectX::XMMATRIX viewProj = DirectX::XMLoadFloat4x4(&vp);
	DirectX::XMFLOAT4X4 invVP;
	DirectX::XMStoreFloat4x4(&invVP, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, viewProj)));
	DirectX::XMStoreFloat4x4(&vp, DirectX::XMMatrixTranspose(viewProj));

	initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer(inverseViewProj, &invVP, sizeof(DirectX::XMFLOAT4X4));
	ProfileReturnVoid;
}

int SE::Core::DecalManager::Create(const Entity& entity, const DecalCreateInfo& createInfo)
{
	StartProfile;
	const auto& textureName = createInfo.textureName;
	const auto exists = entityToTextureGuid.find(entity);
	if (exists != entityToTextureGuid.end())
		ProfileReturnConst(-1);

	const auto textureExists = decalToTransforms.find(textureName);
	if (textureExists == decalToTransforms.end())
	{
		int result = initInfo.resourceHandler->LoadResource(textureName, [this](auto guid, void* data, size_t size)
		{
			Graphics::TextureDesc td;
			td = *((Graphics::TextureDesc*)data);
			void* textureData = ((char*)data) + sizeof(Graphics::TextureDesc);
			int res = this->initInfo.renderer->GetPipelineHandler()->CreateTexture(guid, textureData, td.width, td.height);
			if (res < 0)
				return ResourceHandler::InvokeReturn::FAIL;
			return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
		});
		if (result)
			ProfileReturnConst(-1);
	}
	initInfo.entityManager->RegisterDestroyCallback(entity, { this, &DecalManager::Destroy });
	initInfo.transformManager->Create(entity);
	entityToTextureGuid[entity] = textureName;
	DirectX::XMFLOAT4X4 world = initInfo.transformManager->GetTransform(entity);
	DirectX::XMFLOAT4X4 invWorld;
	DirectX::XMMATRIX mWorld = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMStoreFloat4x4(&invWorld, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, mWorld)));
	DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(mWorld));
	entityToTransformIndex[entity] = decalToTransforms[textureName].world.size();
	decalToTransforms[textureName].world.push_back(world);
	decalToTransforms[textureName].inverseWorld.push_back(invWorld);
	decalToTransforms[textureName].localTransform.push_back(DirectX::XMFLOAT4X4(createInfo.transform));
	decalToTransforms[textureName].owners.push_back(entity);
	decalToTransforms[textureName].decalInfos.push_back({ createInfo.opacity, createInfo.ambiance });

	const auto renderJob = decalToJobID.find(textureName);
	if (renderJob == decalToJobID.end())
	{
		Graphics::RenderJob j;
		j.pipeline = defaultPipeline;
		j.pipeline.PSStage.textures[1] = textureName;
		j.instanceCount = 1;
		j.vertexCount = 14;
		j.maxInstances = 256;
		j.mappingFunc.push_back([textureName, this](int drawn, int toDrawNow)
		{
			auto& worlds = decalToTransforms[textureName].world;
			auto& invWorlds = decalToTransforms[textureName].inverseWorld;
			auto& decalInfo = decalToTransforms[textureName].decalInfos;
			this->initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer(worldConstantBuffer, &worlds[drawn], sizeof(DirectX::XMFLOAT4X4) * toDrawNow);
			this->initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer(inverseWorld, &invWorlds[drawn], sizeof(DirectX::XMFLOAT4X4) * toDrawNow);
			this->initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer(opacities, &decalInfo[drawn], sizeof(DecalInfo) * toDrawNow);

		});
		decalToJobID[textureName] = initInfo.renderer->AddRenderJob(j, Graphics::RenderGroup::POST_PASS_0);
	}
	else
	{
		initInfo.renderer->ChangeRenderJob(renderJob->second, [this](RenderJob& job)
		{
			++job.instanceCount;
		});
	}
	entities.push_back(entity);
	ToggleVisible(entity, false);
	ProfileReturnConst(0);
}


int SE::Core::DecalManager::SetLocalTransform(const Entity& entity, float* transform16rowMajor)
{
	const auto texture = entityToTextureGuid.find(entity);
	if(texture != entityToTextureGuid.end())
	{
		auto transforms = decalToTransforms.find(texture->second);
		const auto transformIndex = entityToTransformIndex[entity];
		transforms->second.localTransform[transformIndex] = *((DirectX::XMFLOAT4X4*)transform16rowMajor);
		initInfo.transformManager->SetAsDirty(entity); //Triggers SetDirty
		return 0;
	}
	return -1;
}

int SE::Core::DecalManager::SetOpacity(const Entity& entity, float opacity)
{
	const auto texture = entityToTextureGuid.find(entity);
	if(texture != entityToTextureGuid.end())
	{
		auto transforms = decalToTransforms.find(texture->second);
		const auto transformIndex = entityToTransformIndex[entity];
		if (opacity > 1.0f) opacity = 1.0f;
		if (opacity < 0.0f) opacity = 0.0f;
		transforms->second.decalInfos[transformIndex].opacity = opacity;
		return 0;
	}
	return -1;
}

int SE::Core::DecalManager::ModifyOpacity(const Entity& entity, float amount)
{
	const auto texture = entityToTextureGuid.find(entity);
	if (texture != entityToTextureGuid.end())
	{
		auto transforms = decalToTransforms.find(texture->second);
		const auto transformIndex = entityToTransformIndex[entity];
		float opacity = transforms->second.decalInfos[transformIndex].opacity + amount;
		if (opacity > 1.0f) opacity = 1.0f;
		if (opacity < 0.0f) opacity = 0.0f;
		transforms->second.decalInfos[transformIndex].opacity = opacity;
		return 0;
	}
	return -1;
}

int SE::Core::DecalManager::Remove(const Entity& entity)
{
	StartProfile;
	int index = -1;
	for (int i = 0; i < entities.size(); i++)
	{
		if (entities[i] == entity)
		{
			index = i;
			break;
		}
	}
	if (index != -1)
		Destroy(index);
	
	ProfileReturn(index != -1 ? 0 : -1);
}

void SE::Core::DecalManager::ToggleVisible(const Entity& entity, bool visible)
{
	const auto tex = entityToTextureGuid.find(entity);
	if (tex != entityToTextureGuid.end())
	{
		const auto index = entityToTransformIndex[entity];
		float& op = decalToTransforms[tex->second].decalInfos[index].opacity;
		if ((visible && op < 0.0f) || (!visible && op > 0.0f))
			op = -op;
	}
}

void SE::Core::DecalManager::SetDirty(const Entity& entity, size_t index)
{
	StartProfile;
	auto texture = entityToTextureGuid.find(entity);
	if(texture != entityToTextureGuid.end())
	{
		
		const size_t transformIndex = entityToTransformIndex[entity];
		const DirectX::XMMATRIX mWorld = DirectX::XMLoadFloat4x4(&initInfo.transformManager->GetCleanedTransforms()[index]);
		const DirectX::XMMATRIX lWorld = DirectX::XMLoadFloat4x4(&decalToTransforms[texture->second].localTransform[transformIndex]);
		DirectX::XMVECTOR lScale, lRot, lTrans;
		DirectX::XMVECTOR wScale, wRot, wTrans;
		DirectX::XMMatrixDecompose(&lScale, &lRot, &lTrans, lWorld);
		DirectX::XMMatrixDecompose(&wScale, &wRot, &wTrans, mWorld);
		const DirectX::XMMATRIX mScale = DirectX::XMMatrixScalingFromVector(lScale) * DirectX::XMMatrixScalingFromVector(wScale);
		const DirectX::XMMATRIX mRot = DirectX::XMMatrixRotationQuaternion(lRot) * DirectX::XMMatrixRotationQuaternion(wRot);
		const DirectX::XMMATRIX mTrans = DirectX::XMMatrixTranslationFromVector(lTrans) * DirectX::XMMatrixTranslationFromVector(wTrans);
		const DirectX::XMMATRIX actualWorld = mScale * mRot * mTrans;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 invWorld;
		DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(actualWorld));
		DirectX::XMStoreFloat4x4(&invWorld, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, actualWorld)));
		decalToTransforms[texture->second].world[transformIndex] = world;
		decalToTransforms[texture->second].inverseWorld[transformIndex] = invWorld;
	}
	ProfileReturnVoid;
}

void SE::Core::DecalManager::Destroy(size_t index)
{
	StartProfile;
	const Entity entity = entities[index];
	entities[index] = entities.back();
	entities.pop_back();

	const auto texture = entityToTextureGuid[entity];
	const size_t transformIndex = entityToTransformIndex[entity];
	auto bucket = decalToTransforms.find(texture); //Guaranteed to find it, no need to check
	bucket->second.inverseWorld[transformIndex] = bucket->second.inverseWorld.back();
	bucket->second.world[transformIndex] = bucket->second.world.back();
	bucket->second.localTransform[transformIndex] = bucket->second.localTransform.back();
	bucket->second.owners[transformIndex] = bucket->second.owners.back();
	bucket->second.decalInfos[transformIndex] = bucket->second.decalInfos.back();

	entityToTransformIndex[bucket->second.owners[transformIndex]] = transformIndex;

	bucket->second.inverseWorld.pop_back();
	bucket->second.world.pop_back();
	bucket->second.localTransform.pop_back();
	bucket->second.owners.pop_back();
	bucket->second.decalInfos.pop_back();
	
	entityToTransformIndex.erase(entity);

	if (bucket->second.world.size() == 0)
	{
		initInfo.renderer->RemoveRenderJob(decalToJobID[texture]);
		decalToJobID.erase(texture);
	}
	else
	{
		initInfo.renderer->ChangeRenderJob(decalToJobID[texture], [](RenderJob& job)
		{
			--job.instanceCount;
		});
	}
	ProfileReturnVoid;
}

void SE::Core::DecalManager::Destroy(const Entity& entity)
{
	Remove(entity);
}

void SE::Core::DecalManager::GarbageCollection()
{
	StartProfile;
	uint32_t aliveInRow = 0;
	while (entities.size() > 0 && aliveInRow < 40U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, entities.size() - 1U);
		size_t i = distribution(generator);
		if (initInfo.entityManager->Alive(entities[i]))
		{
			++aliveInRow;
			continue;
		}
		aliveInRow = 0;
		Destroy(i);
	}
	ProfileReturnVoid;
}
