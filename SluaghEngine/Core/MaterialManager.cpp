#include "MaterialManager.h"
#include <Profiler.h>
//#include <Utilz\Console.h>

static const SE::Utilz::GUID defaultMaterial("default.mat");
static const SE::Utilz::GUID defaultTexture("BlackPink.sei");
static const SE::Utilz::GUID defaultPixelShader("SimplePS.hlsl");
static const SE::Utilz::GUID defaultTextureBinding("DiffuseColor");
static const SE::Utilz::GUID defaultSampler("AnisotropicSampler");
static const SE::Utilz::GUID backBuffer("backbuffer");
static const SE::Utilz::GUID bloomTarget("bloomTarget");

SE::Core::MaterialManager::MaterialManager(const InitializationInfo & initInfoIn) : initInfo(initInfoIn)//, mLoading(initInfo.renderer, initInfo.resourceHandler)
{
	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.eventManager);
	_ASSERT(initInfo.console);
	Allocate(128);

	initInfo.eventManager->RegisterToSetRenderObjectInfo({ this, &MaterialManager::SetRenderObjectInfo });

	ResourceHandler::Callbacks shaderCallbacks;
	shaderCallbacks.loadCallback = shaderLoadCallback = [this](auto guid, auto data, auto size, auto udata, auto usize)
	{
		*usize = size;
		auto res = initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
		if (res < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};
	shaderCallbacks.invokeCallback = [](auto guid, auto data, auto size) {
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	shaderCallbacks.destroyCallback = shaderDestroyCallback = [](auto guid, auto data, auto size) {

	};
	auto res = initInfo.resourceHandler->LoadResource(defaultPixelShader, shaderCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
		throw std::exception("Could not load default pixel shader");

	ResourceHandler::Callbacks materialCallback;
	materialCallback.loadCallback = materialLoadCallback = [this](auto guid, auto data, auto size, auto udata, auto usize)
	{
		*udata = (void*)new MaterialFileData;

		auto& matInfo = *(MaterialFileData*)*udata;
		size_t offset = sizeof(uint32_t);
		memcpy(&matInfo.textureInfo.numTextures, (char*)data, sizeof(uint32_t));
		if (matInfo.textureInfo.numTextures > Graphics::ShaderStage::maxTextures)
			return ResourceHandler::LoadReturn::FAIL;

		memcpy(&matInfo.attrib, (char*)data + offset, sizeof(Graphics::MaterialAttributes));
		offset += sizeof(Graphics::MaterialAttributes);
		for (int i = 0; i < matInfo.textureInfo.numTextures; i++)
		{
			memcpy(&matInfo.textureInfo.textures[i], (char*)data + offset, sizeof(Utilz::GUID));
			offset += sizeof(Utilz::GUID);
			memcpy(&matInfo.textureInfo.bindings[i], (char*)data + offset, sizeof(Utilz::GUID));
			offset += sizeof(Utilz::GUID);
		}
		*usize = sizeof(MaterialFileData);
		return ResourceHandler::LoadReturn::SUCCESS;
	};

	materialCallback.invokeCallback  = [this](auto guid, auto data, auto size) {
		defaultMaterialInfo = (MaterialFileData*)data;
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	materialCallback.destroyCallback = materialDestroyCallback = [](auto guid, auto data, auto size) {
		delete (MaterialFileData*)data;
	};
	res = initInfo.resourceHandler->LoadResource(defaultMaterial, materialCallback, ResourceHandler::LoadFlags::LOAD_FOR_RAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
		throw std::exception("Could not load default material");

	/*if(mdata.textureInfo.numTextures != 1)
		throw std::exception("Default material does not have one texture");*/

	ResourceHandler::Callbacks textureCallbacks;
	textureCallbacks.loadCallback = textureLoadCallback = [this](auto guid, auto data, auto size, auto udata, auto usize)
	{
		Graphics::TextureDesc td;
		memcpy(&td, data, sizeof(td));
		*usize = size - sizeof(td);
		/*Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel*/
		if (td.width * td.height * 4 != size - sizeof(td))
			return ResourceHandler::LoadReturn::FAIL;

		void* rawTextureData = ((char*)data) + sizeof(td);
		auto result = initInfo.renderer->GetPipelineHandler()->CreateTexture(guid, rawTextureData, td.width, td.height);
		if(result < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};

	textureCallbacks.invokeCallback = [](auto guid, auto data, auto size) {
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	textureCallbacks.destroyCallback = textureDestroyCallback = [this](auto guid, auto data, auto size) {
		initInfo.renderer->GetPipelineHandler()->DestroyTexture(guid);
	};

	res = initInfo.resourceHandler->LoadResource(defaultTexture, textureCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::IMMUTABLE);
	if (res)
		throw std::exception("Could not load default texture");

	Graphics::RenderTarget rt;
	rt.bindAsShaderResource = true;
	rt.format = Graphics::TextureFormat::R8G8B8A8_UNORM;
	rt.width = initInfo.optionsHandler->GetOptionUnsignedInt("Window", "width", 800);
	rt.height = initInfo.optionsHandler->GetOptionUnsignedInt("Window", "height", 640);
	rt.clearColor[0] = 0.0f;
	rt.clearColor[1] = 0.0f;
	rt.clearColor[2] = 0.0f;
	rt.clearColor[3] = 0.0f;
	res = initInfo.renderer->GetPipelineHandler()->CreateRenderTarget(bloomTarget, rt);
	if(res < 0)
		throw std::exception("Could not Create bloom render target.");


	Graphics::SamplerState info;
	info.filter = Graphics::Filter::ANISOTROPIC;
	info.maxAnisotropy = 4;
	info.addressU = Graphics::AddressingMode::WRAP;
	info.addressV = Graphics::AddressingMode::WRAP;
	info.addressW = Graphics::AddressingMode::WRAP;
	initInfo.renderer->GetPipelineHandler()->CreateSamplerState(defaultSampler, info);

	Graphics::SamplerState pointSampler;
	pointSampler.filter = Graphics::Filter::POINT;
	pointSampler.addressU = Graphics::AddressingMode::CLAMP;
	pointSampler.addressV = Graphics::AddressingMode::CLAMP;
	pointSampler.addressW = Graphics::AddressingMode::CLAMP;
	pointSampler.maxAnisotropy = 0;

	res = this->initInfo.renderer->GetPipelineHandler()->CreateSamplerState("shadowPointSampler", pointSampler);
	if (res < 0)
		throw std::exception("Could not create shadowpointsampler");
}

SE::Core::MaterialManager::~MaterialManager()
{
	delete materialInfo.data;
}

void SE::Core::MaterialManager::Create(const Entity & entity, const CreateInfo& info)
{
	StartProfile;
	auto find = entityToMaterialInfo.find(entity);
	if (find != entityToMaterialInfo.end())
		ProfileReturnVoid;
	
	// Check if the entity is alive
	if (!initInfo.entityManager->Alive(entity))
		ProfileReturnVoid;

	// Make sure we have enough memory.
	if (materialInfo.used + 1 > materialInfo.allocated)
		Allocate(materialInfo.allocated * 2);

	// Register the entity
	size_t newEntry = materialInfo.used++;
	entityToMaterialInfo[entity] = newEntry;
	materialInfo.entity[newEntry] = entity;
	materialInfo.bloom[newEntry] = info.bloom ? 1u : 0u;
	materialInfo.shader[newEntry] = defaultPixelShader; 
	materialInfo.material[newEntry] = defaultMaterialInfo;
	materialInfo.materialGUID[newEntry] = defaultMaterial;


	ResourceHandler::Callbacks shaderCallbacks;
	shaderCallbacks.loadCallback = shaderLoadCallback;
	shaderCallbacks.destroyCallback = shaderDestroyCallback;
	shaderCallbacks.invokeCallback = [this, info, entity](auto guid, auto data, auto size) {

		MaterialFileData* mdata;
		ResourceHandler::Callbacks materialCallbacks;
		materialCallbacks.loadCallback = materialLoadCallback;
		materialCallbacks.destroyCallback = materialDestroyCallback;
		materialCallbacks.invokeCallback = [&mdata](auto guid, auto data, auto size)
		{
			mdata = (MaterialFileData*)data;
			return ResourceHandler::InvokeReturn::SUCCESS;
		};

		auto res = initInfo.resourceHandler->LoadResource(info.materialFile, materialCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_RAM);
		if (res < 0)
			return ResourceHandler::InvokeReturn::FAIL;


		ResourceHandler::Callbacks textureCallbacks;
		textureCallbacks.loadCallback = textureLoadCallback;
		textureCallbacks.destroyCallback = textureDestroyCallback;
		textureCallbacks.invokeCallback = [](auto guid, auto data, auto size)
		{
			return ResourceHandler::InvokeReturn::SUCCESS;
		};


		for (uint32_t i = 0; i < mdata->textureInfo.numTextures; i++)
		{
			res = initInfo.resourceHandler->LoadResource(mdata->textureInfo.textures[i], textureCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM);
			if (res < 0)
				mdata->textureInfo.textures[i] = defaultTexture;
		}

		if (!toUpdate.push({guid, info.materialFile, mdata, entity }))
			return ResourceHandler::InvokeReturn::FAIL;

		return ResourceHandler::InvokeReturn::SUCCESS;
	};

	auto res = initInfo.resourceHandler->LoadResource(info.shader, shaderCallbacks,  ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::ASYNC);


	StopProfile;
}

void SE::Core::MaterialManager::Frame(Utilz::TimeCluster * timer)
{
	StartProfile;
	timer->Start(("MaterialManager"));
	GarbageCollection();
	std::vector<Entity> eTu;
	while (!toUpdate.wasEmpty())
	{
		auto& job = toUpdate.top();

		auto find = entityToMaterialInfo.find(job.entity);
		if (find != entityToMaterialInfo.end())
		{
			materialInfo.shader[find->second] = job.shader;
			materialInfo.material[find->second] = job.material;
			materialInfo.materialGUID[find->second] = job.mat;
			initInfo.eventManager->TriggerUpdateRenderableObject(job.entity);
		}
		toUpdate.pop();
	}
	timer->Stop(("MaterialManager"));
	StopProfile;
}

void SE::Core::MaterialManager::Allocate(size_t size)
{
	StartProfile;
	_ASSERT(size > materialInfo.allocated);
	// Allocate new memory
	MaterialData newData;
	newData.allocated = size;
	newData.data = operator new(size * MaterialData::size);
	newData.used = materialInfo.used;

	// Setup the new pointers
	newData.entity = (Entity*)newData.data;
	newData.shader = (Utilz::GUID*)(newData.entity + newData.allocated);
	newData.materialGUID = (Utilz::GUID*)(newData.shader + newData.allocated);
	newData.material = (MaterialFileData**)(newData.materialGUID + newData.allocated);
	newData.bloom = (uint8_t*)(newData.material + newData.allocated);

	// Copy data
	memcpy(newData.entity, materialInfo.entity, materialInfo.used * sizeof(Entity));
	memcpy(newData.shader, materialInfo.shader, materialInfo.used * sizeof(Utilz::GUID));
	memcpy(newData.materialGUID, materialInfo.materialGUID, materialInfo.used * sizeof(Utilz::GUID));
	memcpy(newData.material, materialInfo.material, materialInfo.used * sizeof(MaterialFileData*));
	memcpy(newData.bloom, materialInfo.bloom, materialInfo.used * sizeof(uint8_t));

	// Delete old data;
	operator delete(materialInfo.data);
	materialInfo = newData;

	StopProfile;
}

void SE::Core::MaterialManager::Destroy(size_t index)
{
	StartProfile;
	// Temp variables
	size_t last = materialInfo.used - 1;
	const Entity entity = materialInfo.entity[index];
	const Entity last_entity = materialInfo.entity[last];

	for (uint32_t i = 0; i < materialInfo.material[index]->textureInfo.numTextures; i++)
		initInfo.resourceHandler->UnloadResource(materialInfo.material[index]->textureInfo.textures[i], ResourceHandler::UnloadFlags::VRAM);
	initInfo.resourceHandler->UnloadResource(materialInfo.materialGUID[index], ResourceHandler::UnloadFlags::RAM);
	initInfo.resourceHandler->UnloadResource(materialInfo.shader[index], ResourceHandler::UnloadFlags::VRAM);
	
	// Copy the data
	materialInfo.entity[index] = last_entity;
	materialInfo.shader[index] = materialInfo.shader[last];
	materialInfo.materialGUID[index] = materialInfo.materialGUID[last];
	materialInfo.material[index] = materialInfo.material[last];
	materialInfo.bloom[index] = materialInfo.bloom[last];


	// Replace the index for the last_entity 
	entityToMaterialInfo[last_entity] = index;
	entityToMaterialInfo.erase(entity);

	materialInfo.used--;

	StopProfile;
}

void SE::Core::MaterialManager::Destroy(const Entity & entity)
{
}

void SE::Core::MaterialManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (materialInfo.used > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, materialInfo.used - 1U);
		size_t i = distribution(generator);
		if (initInfo.entityManager->Alive(materialInfo.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}

void SE::Core::MaterialManager::SetRenderObjectInfo(const Entity & entity, Graphics::RenderJob* info)
{
	auto find = entityToMaterialInfo.find(entity);
	if (find != entityToMaterialInfo.end())
	{
		
		auto& mdata = *materialInfo.material[find->second];
		info->pipeline.PSStage.shader = materialInfo.shader[find->second];
		info->pipeline.PSStage.textureCount = mdata.textureInfo.numTextures;

		for (uint8_t i = 0; i < mdata.textureInfo.numTextures; i++)
		{
			info->pipeline.PSStage.textureBindings[i] = mdata.textureInfo.bindings[i];
			info->pipeline.PSStage.textures[i] = mdata.textureInfo.textures[i];
		}
		
		info->pipeline.PSStage.samplers[0] = defaultSampler;
		info->pipeline.PSStage.samplerCount = 1;
		
		if (materialInfo.bloom[find->second] == 1u)
		{
			info->pipeline.OMStage.renderTargets[0] = backBuffer;
			info->pipeline.OMStage.renderTargets[1] = bloomTarget;
			info->pipeline.OMStage.renderTargetCount = 2;
			info->pipeline.OMStage.depthStencilView = backBuffer;
		}


		auto& attrib = mdata.attrib;
		info->mappingFunc.push_back([this,attrib](auto a, auto b)
		{
			initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("MaterialAttributes", (void*)&attrib, sizeof(attrib));
		});

		

	}
	else
	{
		info->pipeline.PSStage.shader = defaultPixelShader;
		info->pipeline.PSStage.textureCount = 0;
		info->pipeline.PSStage.samplerCount = 0;
	}
	info->pipeline.PSStage.textures[info->pipeline.PSStage.textureCount] = "DepthCube";
	info->pipeline.PSStage.textureBindings[info->pipeline.PSStage.textureCount] = "ShadowMap";
	++info->pipeline.PSStage.textureCount;
	info->pipeline.PSStage.samplers[1] = "shadowPointSampler";
	info->pipeline.PSStage.samplerCount = 2;
}
