#include <MaterialManager.h>
#include <Profiler.h>

SE::Core::MaterialManager::MaterialManager(ResourceHandler::IResourceHandler* resourceHandler, Graphics::IRenderer* renderer, const EntityManager& entityManager)
	: resourceHandler(resourceHandler), renderer(renderer), entityManager(entityManager)
{
	_ASSERT(resourceHandler);
	_ASSERT(renderer);
	Allocate(128);
	defaultTextureHandle = 0;

	auto res = resourceHandler->LoadResource(Utilz::GUID("SimplePS.hlsl"), ResourceHandler::LoadResourceDelegate::Make<MaterialManager, &MaterialManager::LoadDefaultShader>(this));
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
	auto find = entityToMaterialInfo.find(entity);
	if (find != entityToMaterialInfo.end())
		ProfileReturnVoid;
	
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
	materialInfo.used++;

	//Check if shader already is loaded.
	const auto shaderFind = guidToShaderIndex.find(info.shader);
	if(shaderFind == guidToShaderIndex.end())
	{
		//Not loaded, load it.
		const size_t shaderIndex = shaders.size();
		shaders.push_back({ defaultShaderHandle, 1 });
		guidToShaderIndex[info.shader] = shaderIndex;
		resourceHandler->LoadResource(info.shader, ResourceHandler::LoadResourceDelegate::Make<MaterialManager, &MaterialManager::LoadShader>(this));
	}
	const int shaderIndex = guidToShaderIndex[info.shader];
	shaders[shaderIndex].refCount++;
	materialInfo.shaderIndex[newEntry] = shaderIndex;
	for(int i = 0; i < info.textureCount; ++i)
	{
		const auto textureFind = guidToTextureIndex.find(info.textureFileNames[i]);
		if(textureFind == guidToTextureIndex.end())
		{
			const size_t textureIndex = textures.size();
			textures.push_back({ defaultTextureHandle, 0 });
			guidToTextureIndex[info.textureFileNames[i]] = textureIndex;
			resourceHandler->LoadResource(info.textureFileNames[i], ResourceHandler::LoadResourceDelegate::Make<MaterialManager, &MaterialManager::LoadTexture>(this));
		}
		const int textureIndex = guidToTextureIndex[info.textureFileNames[i]];
		textures[textureIndex].refCount++;
	}

	auto& reflection = shaders[shaderIndex].shaderReflection;
	for (int i = 0; i < info.textureCount; ++i)
	{
		const auto bindName = reflection.textureNameToBindSlot.find(info.shaderResourceNames[i]);
		if (bindName != reflection.textureNameToBindSlot.end())
		{
			materialInfo.textureBindings[newEntry].bindings[i] = bindName->second;
			materialInfo.textureIndices[newEntry].indices[i] = guidToTextureIndex[info.textureFileNames[i]];
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
	newData.textureBindings = (TextureBindings*)(newData.entity + newData.allocated);
	newData.textureIndices = (TextureIndices*)(newData.textureBindings + newData.allocated);
	newData.shaderIndex = (uint32_t*)(newData.textureIndices + newData.allocated);

	// Copy data
	memcpy(newData.entity, materialInfo.entity, materialInfo.used * sizeof(Entity));
	memcpy(newData.textureBindings, materialInfo.textureBindings, materialInfo.used * sizeof(TextureBindings));
	memcpy(newData.textureIndices, materialInfo.textureIndices, materialInfo.used * sizeof(TextureIndices));
	memcpy(newData.shaderIndex, materialInfo.shaderIndex, materialInfo.used * sizeof(uint32_t));


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

	auto& reflection = shaders[materialInfo.shaderIndex[index]].shaderReflection;
	const size_t textureCount = reflection.textureNameToBindSlot.size();
	for(int i = 0; i < textureCount; ++i)
	{
		textures[materialInfo.textureIndices[index].indices[i]].refCount--;
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

int SE::Core::MaterialManager::LoadDefaultShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	defaultShaderHandle = renderer->CreatePixelShader(data, size, &defaultShaderReflection);
	if (defaultShaderHandle == -1)
		ProfileReturnConst(-1);
	ProfileReturnConst(0);
}

int SE::Core::MaterialManager::LoadTexture(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	Graphics::TextureDesc td;
	memcpy(&td, data, sizeof(td));
	/*Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel*/
	if (td.width * td.height * 4 != size - sizeof(td))
		ProfileReturnConst( -1);
	void* rawTextureData = ((char*)data) + sizeof(td);
	auto handle = renderer->CreateTexture(rawTextureData, td);
	if (handle == -1)
		ProfileReturnConst(-1);
	const size_t index = guidToTextureIndex[guid];
	textures[index].textureHandle = handle;

	ProfileReturnConst(0);
}

int SE::Core::MaterialManager::LoadShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	const size_t shaderIndex = guidToShaderIndex[guid];

	auto handle = renderer->CreatePixelShader(data, size, &shaders[shaderIndex].shaderReflection);
	if (handle == -1)
		ProfileReturnConst(-1);
	shaders[shaderIndex].shaderHandle = handle;

	ProfileReturnConst(0);
}
