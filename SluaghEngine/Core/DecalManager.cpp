#include "DecalManager.h"

using namespace SE::Graphics;

SE::Core::DecalManager::DecalManager(const IDecalManager::InitializationInfo& initInfo)
{
	this->initInfo = initInfo;

	vertexShader = "DecalVS.hlsl";
	pixelShader = "DecalPS.hlsl";
	wvpConstantBuffer = "DecalsWVP";
	inverseViewProj = "DecalsInverseViewProj";
	inverseWorld = "DecalsInverseWorld";
	blendState = "DecalBlend";
	textureBindName = "DecalDiffuse";

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

	
	//Decals do not use the input assembler.
	defaultPipeline.VSStage.shader = vertexShader;
	defaultPipeline.PSStage.textureBindings[0] = "DepthSRV";
	defaultPipeline.PSStage.textures[0] = "DepthSRV";
	defaultPipeline.PSStage.textureBindings[1] = textureBindName;
	//The texture name will be set for each decal that uses a different texture
	defaultPipeline.PSStage.textureCount = 2;
	defaultPipeline.OMStage.renderTargets[0] = "backbuffer";
	defaultPipeline.OMStage.renderTargetCount = 1;
	defaultPipeline.OMStage.depthStencilView = "";
	defaultPipeline.OMStage.blendState = blendState;
	

	
}

SE::Core::DecalManager::~DecalManager()
{

}

void SE::Core::DecalManager::Frame(Utilz::TimeCluster* timer)
{

}

int SE::Core::DecalManager::Create(const Entity& entity, const Utilz::GUID& textureName)
{
	return 0;
}

int SE::Core::DecalManager::Remove(const Entity& entity)
{
	return 0;
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
