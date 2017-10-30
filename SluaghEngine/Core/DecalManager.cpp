#include "DecalManager.h"

using namespace SE::Graphics;

SE::Core::DecalManager::DecalManager(const IDecalManager::InitializationInfo& initInfo)
{
	this->initInfo = initInfo;

	vertexShader = "DecalVS.hlsl";
	pixelShader = "DecalPS.hlsl";
	worldConstantBuffer = "DecalsWorld";
	inverseViewProj = "DecalsInverseViewProj";
	inverseWorld = "DecalsInverseWorld";
	blendState = "DecalBlend";
	textureBindName = "DecalDiffuse";
	rasterizerState = "DecalRS";

	this->initInfo.resourceHandler->LoadResource(vertexShader, [this](auto guid, void* data, size_t size)
	{
		auto res = this->initInfo.renderer->GetPipelineHandler()->CreateVertexShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::InvokeReturn::Fail;
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	this->initInfo.resourceHandler->LoadResource(pixelShader, [this](auto guid, void* data, size_t size)
	{
		auto res = this->initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::InvokeReturn::Fail;
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});

	BlendState bs;
	bs.enable = true;
	bs.blendOperation = BlendOperation::ADD;
	bs.blendOperationAlpha = BlendOperation::MAX;
	bs.srcBlend = Blend::INV_SRC_ALPHA;
	bs.srcBlendAlpha = Blend::ONE;
	bs.dstBlend = Blend::INV_SRC_ALPHA;
	bs.dstBlendAlpha = Blend::ONE;
	
	this->initInfo.renderer->GetPipelineHandler()->CreateBlendState(blendState, bs);

	RasterizerState rsState;
	rsState.cullMode = CullMode::CULL_NONE;
	rsState.fillMode = FillMode::FILL_SOLID;
	rsState.windingOrder = WindingOrder::CLOCKWISE;
	this->initInfo.renderer->GetPipelineHandler()->CreateRasterizerState(rasterizerState, rsState);

	
	defaultPipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_STRIP;
	defaultPipeline.VSStage.shader = vertexShader;
	defaultPipeline.PSStage.textureBindings[0] = "DepthBuffer";
	defaultPipeline.PSStage.textureBindings[1] = textureBindName;
	defaultPipeline.PSStage.textures[0] = "backbufferdepth";
	//The texture name will be set for each decal that uses a different texture
	defaultPipeline.PSStage.textureCount = 2;
	defaultPipeline.PSStage.shader = pixelShader;
	defaultPipeline.OMStage.renderTargets[0] = "backbuffer";
	defaultPipeline.OMStage.renderTargetCount = 1;
	defaultPipeline.OMStage.depthStencilView = "";
	defaultPipeline.RStage.rasterizerState = rasterizerState;
	//defaultPipeline.OMStage.blendState = blendState;
	
	DirectX::XMFLOAT4X4 vp = initInfo.cameraManager->GetViewProjection(initInfo.cameraManager->GetActive());

	cachedViewProj = DirectX::XMLoadFloat4x4(&vp);

	initInfo.transformManager->RegisterSetDirty({ this, &DecalManager::SetDirty });
	
}

SE::Core::DecalManager::~DecalManager()
{

}

void SE::Core::DecalManager::Frame(Utilz::TimeCluster* timer)
{
	DirectX::XMFLOAT4X4 vp = initInfo.cameraManager->GetViewProjection(initInfo.cameraManager->GetActive());
	const DirectX::XMMATRIX viewProj = DirectX::XMLoadFloat4x4(&vp);
	DirectX::XMFLOAT4X4 invVP;
	DirectX::XMStoreFloat4x4(&invVP, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, viewProj)));
	DirectX::XMStoreFloat4x4(&vp, DirectX::XMMatrixTranspose(viewProj));

	initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer(inverseViewProj, &invVP, sizeof(DirectX::XMFLOAT4X4));

}

int SE::Core::DecalManager::Create(const Entity& entity, const Utilz::GUID& textureName)
{
	const auto exists = entityToTextureGuid.find(entity);
	if (exists != entityToTextureGuid.end())
		return -1;

	const auto textureExists = decalToTransforms.find(textureName);
	if (textureExists == decalToTransforms.end())
	{
		int result =  initInfo.resourceHandler->LoadResource(textureName, [this](auto guid, void* data, size_t size)
		{
			Graphics::TextureDesc td;
			td = *((Graphics::TextureDesc*)data);
			void* textureData = ((char*)data) + sizeof(Graphics::TextureDesc);
			int res = this->initInfo.renderer->GetPipelineHandler()->CreateTexture(guid, textureData, td.width, td.height);
			if (res < 0)
				return ResourceHandler::InvokeReturn::Fail;
			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		}, false);
		if (result)
			return -1;
	}

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
	
	const auto renderJob = decalToJobID.find(textureName);
	if(renderJob == decalToJobID.end())
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
			this->initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer(worldConstantBuffer, &worlds[drawn], sizeof(DirectX::XMFLOAT4X4) * toDrawNow);
			this->initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer(inverseWorld, &invWorlds[drawn], sizeof(DirectX::XMFLOAT4X4) * toDrawNow);

		});
		decalToJobID[textureName] = initInfo.renderer->AddRenderJob(j, Graphics::RenderGroup::THIRD_PASS);
	}
	else
	{
		initInfo.renderer->ChangeRenderJob(renderJob->second, [this](RenderJob& job)
		{
			++job.instanceCount;
		});
	}


	return 0;
}

int SE::Core::DecalManager::Remove(const Entity& entity)
{
	return 0;
}

void SE::Core::DecalManager::SetDirty(const Entity& entity, size_t index)
{
	auto texture = entityToTextureGuid.find(entity);
	if(texture != entityToTextureGuid.end())
	{
		const size_t transformIndex = entityToTransformIndex[entity];
		DirectX::XMMATRIX mWorld = DirectX::XMLoadFloat4x4(&initInfo.transformManager->GetCleanedTransforms()[index]);
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 invWorld;
		DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranspose(mWorld));
		DirectX::XMStoreFloat4x4(&invWorld, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, mWorld)));
		decalToTransforms[texture->second].world[transformIndex] = world;
		decalToTransforms[texture->second].inverseWorld[transformIndex] = invWorld;
		dirty = true;
	}
}

void SE::Core::DecalManager::Destroy(size_t index)
{
}

void SE::Core::DecalManager::Destroy(const Entity& entity)
{
}

void SE::Core::DecalManager::GarbageCollection()
{
}
