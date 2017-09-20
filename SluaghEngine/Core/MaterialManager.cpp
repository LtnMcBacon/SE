#include "MaterialManager.h"
#include <Core\Engine.h>
#include <Profiler.h>

SE::Core::MaterialManager::MaterialManager(const EntityManager& entityManager) : entityManager(entityManager)
{
	Allocate(128);
	defaultTextureHandle = 0;

	auto rh = Core::Engine::GetInstance().GetResourceHandler();

	auto res = rh->LoadResource(Utilz::GUID("SimplePS.hlsl"), ResourceHandler::LoadResourceDelegate::Make([this](const Utilz::GUID& guid, void*data, size_t size) -> int {
		defaultShaderHandle = Core::Engine::GetInstance().GetRenderer()->CreatePixelShader(data, size);
		if (defaultShaderHandle == -1)
			return -1;
		return 0;
	}), true);
	if (res)
		throw std::exception("Could not load default pixel shader.");


}


SE::Core::MaterialManager::~MaterialManager()
{
	delete materialInfo.data;
}

void SE::Core::MaterialManager::Create(const Entity & entity, const CreateInfo& info)
{
	StartProfile;
	auto& find = entityToMaterialInfo.find(entity);
	if (find == entityToMaterialInfo.end())
	{
		auto& engine = Core::Engine::GetInstance();

		// Check if the entity is alive
		if (!entityManager.Alive(entity))
			ProfileReturnVoid;

		// Make sure we have enough memory.
		if (materialInfo.used + 1 > materialInfo.allocated)
			Allocate(materialInfo.allocated * 2);

		// Register the entity
		size_t newEntry = materialInfo.used;
		entityToMaterialInfo[entity] = newEntry;
		materialInfo.entity[newEntry] = entity;


		// TODO: Multiple textures (sub-meshes)
		{
			// Load texture
			auto& findTexture = guidToTextureInfo.find(info.textures[0]); // See if it the texture is loaded.
			auto& textureIndex = guidToTextureInfo[info.textures[0]]; // Get a reference to the texture index
			if (findTexture == guidToTextureInfo.end())	// If it wasn't loaded, load it.	
			{
				textureInfo.push_back({ defaultTextureHandle , 0 }); // Init the texture to default texture.
				textureIndex = textureInfo.size() - 1;
				engine.GetResourceHandler()->LoadResource(info.textures[0], ResourceHandler::LoadResourceDelegate::Make<MaterialManager, &MaterialManager::LoadTexture>(this));
			}
			// Increase ref Count and save the index to the material info.
			textureInfo[textureIndex].refCount++;
			materialInfo.textureIndex[newEntry] = textureIndex;
		}

		// TODO: Multiple shaders (sub-meshes)
		{
			// Load shader
			auto& findShader = guidToShaderInfo.find(info.shader[0]); // See if the shader is loaded
			auto& shaderIndex = guidToTextureInfo[info.shader[0]]; // Get a reference to the shader index
			if (findShader == guidToShaderInfo.end())// If it wasn't loaded, load it.	
			{
				shaderInfo.push_back({ defaultShaderHandle , 0 }); // Init the texture to default texture.
				shaderIndex = shaderInfo.size() - 1;
				engine.GetResourceHandler()->LoadResource(info.shader[0], ResourceHandler::LoadResourceDelegate::Make<MaterialManager, &MaterialManager::LoadShader>(this));
			}
				
			// Increase refCount
			shaderInfo[shaderIndex].refCount++;
			materialInfo.shaderIndex[newEntry] = shaderIndex;
		}


	}
	StopProfile;
}

void SE::Core::MaterialManager::Frame()
{
	GarbageCollection();
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
	newData.textureIndex = (size_t*)(newData.entity + newData.allocated);
	newData.shaderIndex = (size_t*)(newData.textureIndex + newData.allocated);

	// Copy data
	memcpy(newData.entity, materialInfo.entity, materialInfo.used * sizeof(Entity));
	memcpy(newData.textureIndex, materialInfo.textureIndex, materialInfo.used * sizeof(size_t));
	memcpy(newData.shaderIndex, materialInfo.shaderIndex, materialInfo.used * sizeof(size_t));


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
	const Entity& entity = materialInfo.entity[index];
	const Entity& last_entity = materialInfo.entity[last];

	// Copy the data
	materialInfo.entity[index] = last_entity;

	textureInfo[materialInfo.textureIndex[index]].refCount--;
	materialInfo.textureIndex[index] = materialInfo.textureIndex[last];

	shaderInfo[materialInfo.shaderIndex[index]].refCount--;
	materialInfo.shaderIndex[index] = materialInfo.shaderIndex[last];

	// Replace the index for the last_entity 
	entityToMaterialInfo[last_entity] = index;
	entityToMaterialInfo.erase(entity);

	materialInfo.used--;
	StopProfile;
}

void SE::Core::MaterialManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (materialInfo.used > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<uint32_t> distribution(0U, materialInfo.used - 1U);
		uint32_t i = distribution(generator);
		if (entityManager.Alive(materialInfo.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}

int SE::Core::MaterialManager::LoadTexture(const Utilz::GUID & guid, void * data, size_t size)
{
	Graphics::TextureDesc td;
	memcpy(&td, data, sizeof(td));
	/*Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel*/
	if (td.width * td.height * 4 != size - sizeof(td))
		return -1;
	void* rawTextureData = ((char*)data) + sizeof(td);
	auto handle = Core::Engine::GetInstance().GetRenderer()->CreateTexture(rawTextureData, td);
	if (handle == -1)
		return -1;
	auto index = guidToTextureInfo[guid];
	textureInfo[index].textureHandle = handle;

	return 0;
}

int SE::Core::MaterialManager::LoadShader(const Utilz::GUID & guid, void * data, size_t size)
{
	auto handle = Core::Engine::GetInstance().GetRenderer()->CreatePixelShader(data, size);
	if (handle == -1)
		return -1;
	auto index = guidToShaderInfo[guid];
	shaderInfo[index].shaderHandle = handle;

	return 0;
}
