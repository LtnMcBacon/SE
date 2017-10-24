#include "MaterialManager.h"
#include <Profiler.h>
//#include <Utilz\Console.h>

SE::Core::MaterialManager::MaterialManager(const InitializationInfo & initInfo) : initInfo(initInfo)
{
	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.renderableManager);
	_ASSERT(initInfo.console);
	Allocate(128);
	defaultTexture = "BlackPink.sei";
	defaultPixelShader = "SimplePS.hlsl";
	defaultTextureBinding = "DiffuseColor";
	defaultSampler = "AnisotropicSampler";

	initInfo.renderableManager->RegisterToSetRenderObjectInfo({ this, &MaterialManager::SetRenderObjectInfo });

	auto res = initInfo.resourceHandler->LoadResource(defaultPixelShader, [this](auto guid, auto data, auto size)
	{
		this->initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
		/*if (result < 0)
			return ResourceHandler::InvokeReturn::Fail;*/
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	if (res)
		throw std::exception("Could not load default pixel shader.");
	auto& defM = guidToMaterial[defaultMaterial];

	res = initInfo.resourceHandler->LoadResource("Cube.mat",
		[this, &defM](auto guid, auto data, auto size) {
		LoadMaterialFile(data, size, defM);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	if (res)
		throw std::exception("Could not load default material.");
	defaultTexture = defM.info.textures[0];
	defaultTextureBinding = defM.info.bindings[0];

	res = initInfo.resourceHandler->LoadResource(defaultTexture,
		[this](auto guid, auto data, auto size) {
		auto result = LoadTexture(guid, data, size);
		if (result < 0)
			return ResourceHandler::InvokeReturn::Fail;
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	if (res)
		throw std::exception("Could not load default texture.");

	Graphics::SamplerState info;
	info.filter = Graphics::Filter::ANISOTROPIC;
	info.maxAnisotropy = 4;
	info.addressU = Graphics::AddressingMode::WRAP;
	info.addressV = Graphics::AddressingMode::WRAP;
	initInfo.renderer->GetPipelineHandler()->CreateSamplerState(defaultSampler, info);
}

SE::Core::MaterialManager::~MaterialManager()
{
	delete materialInfo.data;
}

void SE::Core::MaterialManager::Create(const Entity & entity, const CreateInfo& info, bool async, ResourceHandler::Behavior behavior)
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
	size_t newEntry = materialInfo.used;
	entityToMaterialInfo[entity] = newEntry;
	materialInfo.entity[newEntry] = entity;
	materialInfo.used++;

	//Check if shader already is loaded.
	const auto shaderFind = guidToShader.find(info.shader);
	auto& shaderInfo = guidToShader[info.shader];
	if(shaderFind == guidToShader.end())
	{
		//Not loaded, load it.

		auto res = initInfo.resourceHandler->LoadResource(info.shader, [this](auto guid, auto data, auto size) {
			initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
			/*if (result < 0)
			return ResourceHandler::InvokeReturn::Fail;*/
			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		});
		if (res)
		{
			initInfo.console->PrintChannel("Could not load shader. Using default instead. GUID: %u, Error: %d\n", "Resources", info.shader, res);
		}
	}
	shaderInfo.refCount++;
	materialInfo.shader[newEntry] = info.shader;
	
	// material file
	const auto materialFind = guidToMaterial.find(info.materialFile);
	auto& material = guidToMaterial[info.materialFile];
	if (materialFind == guidToMaterial.end())
	{
		auto res = initInfo.resourceHandler->LoadResource(info.materialFile, [this, &material, async, entity](auto guid, auto data, auto size) {

			auto result = LoadMaterialFile(data, size, material);
			if(result < 0)
				return ResourceHandler::InvokeReturn::Fail;

			LoadTextures(material, entity);

			if (async)
				toUpdate.push({ guid });

			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		}, async, behavior);

		if (res)
		{
			initInfo.console->PrintChannel("Could not load material. Using default instead. GUID: %u, Error: %d\n", "Resources", info.materialFile, res);
		}
	}

	if (async)
		materialInfo.material[newEntry] = defaultMaterial;
	else
		materialInfo.material[newEntry] = info.materialFile;


	//for (uint32_t i = 0; i < material.info.amountOfTex; ++i)
	//{

	//	materialInfo.textures[newEntry].textures[i] = materials[materialIndex].info.tex[i];
	//	auto& reflection = shaders[shaderIndex].shaderReflection;
	//	const auto bindName = reflection.textureNameToBindSlot.find(Utilz::GUID(materials[materialIndex].info.textureChannel[i]));
	//	if (bindName != reflection.textureNameToBindSlot.end())
	//	{
	//		materialInfo.textureBindings[newEntry].bindings[0] = bindName->second;
	//	}
	//	const auto bindName2 = reflection.textureNameToBindSlot.find(Utilz::GUID(materials[materialIndex].info.textureChannel[i]));
	//	if (bindName2 != reflection.textureNameToBindSlot.end())
	//	{
	//		materialInfo.textureBindings[newEntry].bindings[1] = bindName2->second;
	//	}
	//}
	//if (materials[materialIndex].info.amountOfTex == 0)	//sets default tex if no tex
	//{
	//	auto& reflection = shaders[shaderIndex].shaderReflection;
	//	const auto bindName = reflection.textureNameToBindSlot.find(Utilz::GUID("DiffuseColor"));
	//	if (bindName != reflection.textureNameToBindSlot.end())
	//	{
	//		materialInfo.textureBindings[newEntry].bindings[0] = bindName->second;
	//	}
	//}

	
	// Textures, materials.
	{	
		
		if (material.info.amountOfTex == 0)	//sets default tex if no tex
		{
			materialInfo.material[newEntry] = defaultMaterial;
		}
		
	}
	StopProfile;
}

void SE::Core::MaterialManager::Frame(Utilz::TimeCluster * timer)
{
	StartProfile;
	timer->Start("MaterialManager");
	GarbageCollection();

	while (!toUpdate.wasEmpty())
	{
		auto& job = toUpdate.top();
		auto& material = guidToMaterial[job.material];
		for (auto& e : material.entities)
		{
			auto find = entityToMaterialInfo.find(e);
			if (find != entityToMaterialInfo.end())
			{
				materialInfo.material[entityToMaterialInfo[e]] = job.material;
				initInfo.renderableManager->UpdateRenderableObject(e);
			}
			
		}
			
		toUpdate.pop();
	}
	timer->Stop("MaterialManager");
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
	newData.material = (Utilz::GUID*)(newData.shader + newData.allocated);

	// Copy data
	memcpy(newData.entity, materialInfo.entity, materialInfo.used * sizeof(Entity));
	memcpy(newData.shader, materialInfo.shader, materialInfo.used * sizeof(Utilz::GUID));
	memcpy(newData.material, materialInfo.material, materialInfo.used * sizeof(Utilz::GUID));

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

	// Copy the data
	materialInfo.entity[index] = last_entity;

	/*auto& reflection = shaders[materialInfo.shaderIndex[index]].shaderReflection;
	const size_t textureCount = reflection.textureNameToBindSlot.size();
	for(int i = 0; i < textureCount; ++i)
	{
		textures[materialInfo.textureIndices[index].indices[i]].entities.remove(entity);
		materialInfo.textureIndices[index].indices[i] = materialInfo.textureIndices[last].indices[i];
		materialInfo.textureBindings[index].bindings[i] = materialInfo.textureBindings[last].bindings[i];
	}
*/

	auto& material = guidToMaterial[materialInfo.material[index]];
	material.entities.remove(entity);
	for (size_t i = 0; i < material.info.amountOfTex; i++)
	{
		auto& texture = guidToTexture[material.info.textures[i]];
		texture.entities.remove(entity);
	}



	materialInfo.shader[index] = materialInfo.shader[last];
	materialInfo.material[index] = materialInfo.material[last];


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
	auto& find = entityToMaterialInfo.find(entity);
	if (find != entityToMaterialInfo.end())
	{


		//info->pipeline.PSStage.shader = shaders[materialInfo.shaderIndex[find->second]].shaderHandle;
		//auto& reflection = shaders[materialInfo.shaderIndex[find->second]].shaderReflection;
		//const int textureCount = reflection.textureNameToBindSlot.size();
		//info->textureCount = textureCount;
		//for (int i = 0; i < textureCount; ++i)
		//{
		//	info->textureBindings[i] = materialInfo.textureBindings[find->second].bindings[i];
		//	info->textureHandles[i] = textures[materialInfo.textureIndices[find->second].indices[i]].textureHandle;
		//}
	}
	else
	{
		info->pipeline.PSStage.shader = defaultPixelShader;
		info->pipeline.PSStage.textureCount = 1;
		info->pipeline.PSStage.textureBindings[0] = defaultTextureBinding;
		info->pipeline.PSStage.textures[0] = defaultTexture;
		info->pipeline.PSStage.samplers[0] = defaultSampler;
		info->pipeline.PSStage.samplerCount = 1;

	/*	info->pixelShader = defaultShaderHandle;
		auto& reflection = defaultShaderReflection;
		const int textureCount = reflection.textureNameToBindSlot.size();
		info->textureCount = textureCount;
		int i = 0;
		for (auto& b : reflection.textureNameToBindSlot)
		{
			info->textureBindings[i] = b.second;
			info->textureHandles[i] = defaultTextureHandle;
			++i;
		}*/
	}
}


void SE::Core::MaterialManager::LoadTextures(matDataInfo material, const Entity& entity)
{
	for (uint32_t i = 0; i < material.info.amountOfTex; ++i)
	{
		const auto textureFind = guidToTexture.find(material.info.textures[i]);
		auto& texture = guidToTexture[material.info.textures[i]];
		if (textureFind == guidToTexture.end())
		{
			auto res = initInfo.resourceHandler->LoadResource(material.info.textures[i], [this, &material](auto guid, auto data, auto size) {

				auto handle = LoadTexture(guid, data, size);
				if (handle < 0)
					return ResourceHandler::InvokeReturn::Fail;

				return ResourceHandler::InvokeReturn::DecreaseRefcount;
			});

			if (res)
			{
				initInfo.console->PrintChannel("Could not load texture. Using default instead. GUID: %u, Error: %d\n", "Resources", material.info.textures[i], res);
			}
		}
		texture.entities.push_back(entity);
	}
}

int SE::Core::MaterialManager::LoadTexture(const Utilz::GUID& guid, void * data, size_t size)
{
	StartProfile;
	Graphics::TextureDesc td;
	memcpy(&td, data, sizeof(td));

	//using namespace std::chrono_literals;

	//std::this_thread::sleep_for(1s);

	/*Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel*/
	if (td.width * td.height * 4 != size - sizeof(td))
		return -1;

	void* rawTextureData = ((char*)data) + sizeof(td);
	/*auto result = */initInfo.renderer->GetPipelineHandler()->CreateTexture(guid, rawTextureData, td.width, td.height);

	return 0;
	
}

int SE::Core::MaterialManager::LoadMaterialFile(void * data, size_t size, matDataInfo& dataIinfo)
{
	//using namespace std::chrono_literals;

	//std::this_thread::sleep_for(1s);
	size_t offset = sizeof(uint32_t);
	memcpy(&dataIinfo.info.amountOfTex, (char*)data, sizeof(uint32_t));
	if (dataIinfo.info.amountOfTex > Graphics::ShaderStage::maxTextures)
		return -1;

	memcpy(&dataIinfo.attrib, (char*)data + offset, sizeof(Graphics::MaterialAttributes));
	offset += sizeof(Graphics::MaterialAttributes);
	for (int i = 0; i < dataIinfo.info.amountOfTex; i++)
	{
		memcpy(&dataIinfo.info.textures[i], (char*)data + offset, sizeof(Utilz::GUID));
		offset += sizeof(Utilz::GUID);
		memcpy(&dataIinfo.info.bindings[i], (char*)data + offset, sizeof(Utilz::GUID));
		offset += sizeof(Utilz::GUID);
	}
}
