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
	defaultTextureHandle = 0;

	initInfo.renderableManager->RegisterToSetRenderObjectInfo({ this, &MaterialManager::SetRenderObjectInfo });


	auto res = initInfo.resourceHandler->LoadResource(Utilz::GUID("SimplePS.hlsl"), { this , &MaterialManager::LoadDefaultShader });
	if (res)
		throw std::exception("Could not load default pixel shader.");

	res = initInfo.resourceHandler->LoadResource("Cube.mat",
		[this](auto guid, auto data, auto size) {
		matDataInfo matinfo;
		LoadMaterialFile(data, size, matinfo);
		materials.push_back(matinfo);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	if (res)
		throw std::exception("Could not load default material.");

	res = initInfo.resourceHandler->LoadResource("BlackPink.sei",
		[this](auto guid, auto data, auto size) {
		defaultTextureHandle = LoadTexture(data, size);
		if (defaultTextureHandle == -1)
			return ResourceHandler::InvokeReturn::Fail;
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	if (res)
		throw std::exception("Could not load default texture.");
}

SE::Core::MaterialManager::~MaterialManager()
{
	for (auto& mat : materials)
	{
		if (mat.info.amountOfTex > 0)
		{
			delete[] mat.info.tex;
			delete[] mat.info.textureChannel;
		}
	}
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
	const auto shaderFind = guidToShaderIndex.find(info.shader);
	auto& shaderIndex = guidToShaderIndex[info.shader];
	if(shaderFind == guidToShaderIndex.end())
	{
		//Not loaded, load it.
		shaderIndex = shaders.size();
		shaders.push_back({ defaultShaderHandle, 0 });

		auto res = initInfo.resourceHandler->LoadResource(info.shader, { this, &MaterialManager::LoadShader });
		if (res)
		{
			initInfo.console->PrintChannel("Could not load shader. Using default instead. GUID: %u, Error: %d\n", "Resources", info.shader, res);
		}
	}
	shaders[shaderIndex].refCount++;
	materialInfo.shaderIndex[newEntry] = shaderIndex;
	
	matDataInfo matinfo;
	// material file
	const auto materialFind = guidToMaterialIndex.find(info.materialFile);
	auto& materialIndex = guidToMaterialIndex[info.materialFile];
	if (materialFind == guidToMaterialIndex.end())
	{
		materialIndex = materials.size();
		materials.push_back(materials[0]);

		auto res = initInfo.resourceHandler->LoadResource(info.materialFile, [this, materialIndex, &matinfo](auto guid, auto data, auto size) {

			LoadMaterialFile(data, size, matinfo);
			materials[materialIndex] = matinfo;

			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		}, false, behavior);

		if (res)
		{
			initInfo.console->PrintChannel("Could not load material. Using default instead. GUID: %u, Error: %d\n", "Resources", info.materialFile, res);
		}
	}
	materialInfo.materialIndex[newEntry] = materialIndex;

	for (uint32_t i = 0; i < materials[materialIndex].info.amountOfTex; ++i)
	{
		auto& reflection = shaders[shaderIndex].shaderReflection;
		const auto bindName = reflection.textureNameToBindSlot.find(Utilz::GUID(materials[materialIndex].info.textureChannel[i]));
		if (bindName != reflection.textureNameToBindSlot.end())
		{
			materialInfo.textureBindings[newEntry].bindings[0] = bindName->second;
		}
		const auto bindName2 = reflection.textureNameToBindSlot.find(Utilz::GUID(materials[materialIndex].info.textureChannel[i]));
		if (bindName2 != reflection.textureNameToBindSlot.end())
		{
			materialInfo.textureBindings[newEntry].bindings[1] = bindName2->second;
		}
	}
	if (materials[materialIndex].info.amountOfTex == 0)	//sets default tex if no tex
	{
		auto& reflection = shaders[shaderIndex].shaderReflection;
		const auto bindName = reflection.textureNameToBindSlot.find(Utilz::GUID("DiffuseColor"));
		if (bindName != reflection.textureNameToBindSlot.end())
		{
			materialInfo.textureBindings[newEntry].bindings[0] = bindName->second;
		}
	}

	
	// Textures, materials.
	{	
		for (uint32_t i = 0; i < materials[materialIndex].info.amountOfTex; ++i)
		{
			const auto textureFind = guidToTextureIndex.find(materials[materialIndex].info.tex[i]);
			auto& textureIndex = guidToTextureIndex[materials[materialIndex].info.tex[i]];
			if (textureFind == guidToTextureIndex.end())
			{
				textureIndex = textures.size();
				textures.push_back({ defaultTextureHandle });


				auto res = initInfo.resourceHandler->LoadResource(materials[materialIndex].info.tex[i], [this, textureIndex, async](auto guid, auto data, auto size) {

					auto handle = LoadTexture(data, size);
					if (handle == -1)
						return ResourceHandler::InvokeReturn::Fail;

					if (async)
						toUpdate.push({ textureIndex, handle });
					else
						textures[textureIndex].textureHandle = handle;

					return ResourceHandler::InvokeReturn::DecreaseRefcount;
				}, async, behavior);

				if (res)
				{
					initInfo.console->PrintChannel("Could not load texture. Using default instead. GUID: %u, Error: %d\n", "Resources", matinfo.info.tex[i], res);
				}
			}
			textures[textureIndex].entities.push_back(entity);
			materialInfo.textureIndices[newEntry].indices[i] = textureIndex;
		}
		if (materials[materialIndex].info.amountOfTex == 0)	//sets default tex if no tex
		{
			auto& textureIndex = guidToTextureIndex[Utilz::GUID("BlackPink.sei")];
			if (textures.size() == 0)
				textures.push_back({ defaultTextureHandle });
			textures[textureIndex].entities.push_back(entity);
			materialInfo.textureIndices[newEntry].indices[0] = textureIndex;
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
		textures[job.textureIndex].textureHandle = job.newHandle;
		for (auto& e : textures[job.textureIndex].entities)
			initInfo.renderableManager->UpdateRenderableObject(e);
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
	newData.textureBindings = (TextureBindings*)(newData.entity + newData.allocated);
	newData.textureIndices = (TextureIndices*)(newData.textureBindings + newData.allocated);
	newData.shaderIndex = (size_t*)(newData.textureIndices + newData.allocated);
	newData.materialIndex = (size_t*)(newData.shaderIndex + newData.allocated);

	// Copy data
	memcpy(newData.entity, materialInfo.entity, materialInfo.used * sizeof(Entity));
	memcpy(newData.textureBindings, materialInfo.textureBindings, materialInfo.used * sizeof(TextureBindings));
	memcpy(newData.textureIndices, materialInfo.textureIndices, materialInfo.used * sizeof(TextureIndices));
	memcpy(newData.shaderIndex, materialInfo.shaderIndex, materialInfo.used * sizeof(size_t));
	memcpy(newData.materialIndex, materialInfo.materialIndex, materialInfo.used * sizeof(size_t));

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

	auto& reflection = shaders[materialInfo.shaderIndex[index]].shaderReflection;
	const size_t textureCount = reflection.textureNameToBindSlot.size();
	for(int i = 0; i < textureCount; ++i)
	{
		textures[materialInfo.textureIndices[index].indices[i]].entities.remove(entity);
		materialInfo.textureIndices[index].indices[i] = materialInfo.textureIndices[last].indices[i];
		materialInfo.textureBindings[index].bindings[i] = materialInfo.textureBindings[last].bindings[i];
	}

	shaders[materialInfo.shaderIndex[index]].refCount--;
	materialInfo.shaderIndex[index] = materialInfo.shaderIndex[last];

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

void SE::Core::MaterialManager::SetRenderObjectInfo(const Entity & entity, Graphics::RenderObjectInfo * info)
{
	auto& find = entityToMaterialInfo.find(entity);
	if (find != entityToMaterialInfo.end())
	{
		info->pixelShader = shaders[materialInfo.shaderIndex[find->second]].shaderHandle;
		auto& reflection = shaders[materialInfo.shaderIndex[find->second]].shaderReflection;
		const int textureCount = reflection.textureNameToBindSlot.size();
		info->textureCount = textureCount;
		for (int i = 0; i < textureCount; ++i)
		{
			info->textureBindings[i] = materialInfo.textureBindings[find->second].bindings[i];
			info->textureHandles[i] = textures[materialInfo.textureIndices[find->second].indices[i]].textureHandle;
		}
	}
	else
	{
		info->pixelShader = defaultShaderHandle;
		auto& reflection = defaultShaderReflection;
		const int textureCount = reflection.textureNameToBindSlot.size();
		info->textureCount = textureCount;
		int i = 0;
		for (auto& b : reflection.textureNameToBindSlot)
		{
			info->textureBindings[i] = b.second;
			info->textureHandles[i] = defaultTextureHandle;
			++i;
		}
	}
}

SE::ResourceHandler::InvokeReturn SE::Core::MaterialManager::LoadDefaultShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	defaultShaderHandle = initInfo.renderer->CreatePixelShader(data, size, &defaultShaderReflection);
	if (defaultShaderHandle == -1)
		ProfileReturnConst(ResourceHandler::InvokeReturn::Fail);
	ProfileReturnConst(ResourceHandler::InvokeReturn::DecreaseRefcount);
}

int SE::Core::MaterialManager::LoadTexture(void * data, size_t size)
{
	StartProfile;
	Graphics::TextureDesc td;
	memcpy(&td, data, sizeof(td));

	//using namespace std::chrono_literals;

	//std::this_thread::sleep_for(1s);

	/*Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel*/
	if (td.width * td.height * 4 != size - sizeof(td))
		ProfileReturnConst( -1);

	void* rawTextureData = ((char*)data) + sizeof(td);
	auto handle = initInfo.renderer->CreateTexture(rawTextureData, td);
	ProfileReturnConst(handle);
	
}

void SE::Core::MaterialManager::LoadMaterialFile(void * data, size_t size, matDataInfo& dataIinfo)
{
	StartProfile;
	//using namespace std::chrono_literals;

	//std::this_thread::sleep_for(1s);
	size_t offset = sizeof(uint32_t);
	memcpy(&dataIinfo.info.amountOfTex, (char*)data, sizeof(uint32_t));
	if (dataIinfo.info.amountOfTex > 0)
	{
		dataIinfo.info.tex = new Utilz::GUID[dataIinfo.info.amountOfTex];
		dataIinfo.info.textureChannel = new Utilz::GUID[dataIinfo.info.amountOfTex];
	}
	memcpy(&dataIinfo.attrib, (char*)data + offset, sizeof(Graphics::MaterialAttributes));
	offset += sizeof(Graphics::MaterialAttributes);
	for (int i = 0; i < dataIinfo.info.amountOfTex; i++)
	{
		memcpy(&dataIinfo.info.tex[i], (char*)data + offset, sizeof(Utilz::GUID));
		offset += sizeof(Utilz::GUID);
		memcpy(&dataIinfo.info.textureChannel[i], (char*)data + offset, sizeof(Utilz::GUID));
		offset += sizeof(Utilz::GUID);
	}
	ProfileReturnVoid;
}

SE::ResourceHandler::InvokeReturn SE::Core::MaterialManager::LoadShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	const size_t shaderIndex = guidToShaderIndex[guid];

	auto handle = initInfo.renderer->CreatePixelShader(data, size, &shaders[shaderIndex].shaderReflection);
	if (handle == -1)
		ProfileReturnConst(ResourceHandler::InvokeReturn::Fail);
	shaders[shaderIndex].shaderHandle = handle;

	ProfileReturnConst(ResourceHandler::InvokeReturn::DecreaseRefcount);
}
