#include <MaterialManager.h>
#include <Profiler.h>
#include <Utilz\Console.h>

SE::Core::MaterialManager::MaterialManager(ResourceHandler::IResourceHandler* resourceHandler, Graphics::IRenderer* renderer, const EntityManager& entityManager, RenderableManager* renderableManager)
	: resourceHandler(resourceHandler), renderer(renderer), entityManager(entityManager), renderableManager(renderableManager)
{
	_ASSERT(resourceHandler);
	_ASSERT(renderer);
	_ASSERT(renderableManager);
	Allocate(128);
	defaultTextureHandle = 0;


	renderableManager->RegisterToSetRenderObjectInfo({ this, &MaterialManager::SetRenderObjectInfo });


	auto res = resourceHandler->LoadResource(Utilz::GUID("SimplePS.hlsl"), {this , &MaterialManager::LoadDefaultShader });
	if (res)
		throw std::exception("Could not load default pixel shader.");

	res = resourceHandler->LoadResource("BlackPink.sei",
		[this](auto guid, auto data, auto size) {
		defaultTextureHandle = LoadTexture(data, size);
		if (defaultTextureHandle == -1)
			return -1;
		return 1;
	});
	if (res)
		throw std::exception("Could not load default texture.");

	

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
	auto& shaderIndex = guidToShaderIndex[info.shader];
	if(shaderFind == guidToShaderIndex.end())
	{
		//Not loaded, load it.
		shaderIndex = shaders.size();
		shaders.push_back({ defaultShaderHandle, 0 });

		auto res = resourceHandler->LoadResource(info.shader, { this, &MaterialManager::LoadShader });
		if (res)
		{
			Utilz::Console::Print("Could not load shader. Using default instead. GUID: %u, Error: %d\n", info.shader, res);
		}
	}
	shaders[shaderIndex].refCount++;
	materialInfo.shaderIndex[newEntry] = shaderIndex;


	auto& reflection = shaders[shaderIndex].shaderReflection;
	for (uint32_t i = 0; i < info.textureCount; ++i)
	{
		const auto bindName = reflection.textureNameToBindSlot.find(info.shaderResourceNames[i]);
		if (bindName != reflection.textureNameToBindSlot.end())
		{
			materialInfo.textureBindings[newEntry].bindings[i] = bindName->second;
		}
	}


	// Textures, materials.
	{	
		for (uint32_t i = 0; i < info.textureCount; ++i)
		{
			const auto textureFind = guidToTextureIndex.find(info.textureFileNames[i]);
			auto& textureIndex = guidToTextureIndex[info.textureFileNames[i]];
			if (textureFind == guidToTextureIndex.end())
			{
				textureIndex = textures.size();
				textures.push_back({ defaultTextureHandle });

				resourceHandler->LoadResource(info.textureFileNames[i], [this, textureIndex](auto guid, auto data, auto size) {
					auto handle = LoadTexture(data, size);
					if (handle == -1)
						return -1;

						toUpdate.push({ textureIndex, handle });

					return 1;
				}, async, behavior);
			}

			textures[textureIndex].entities.push_back(entity);
			materialInfo.textureIndices[newEntry].indices[i] = textureIndex;
		}

		
	}
	StopProfile;
}


void SE::Core::MaterialManager::Frame()
{
	GarbageCollection();

	while (!toUpdate.wasEmpty())
	{
		auto& job = toUpdate.top();
		textures[job.textureIndex].textureHandle = job.newHandle;
		for (auto& e : textures[job.textureIndex].entities)
			renderableManager->UpdateRenderableObject(e);
		toUpdate.pop();
	}
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

	// Copy data
	memcpy(newData.entity, materialInfo.entity, materialInfo.used * sizeof(Entity));
	memcpy(newData.textureBindings, materialInfo.textureBindings, materialInfo.used * sizeof(TextureBindings));
	memcpy(newData.textureIndices, materialInfo.textureIndices, materialInfo.used * sizeof(TextureIndices));
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

void SE::Core::MaterialManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (materialInfo.used > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, materialInfo.used - 1U);
		size_t i = distribution(generator);
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

int SE::Core::MaterialManager::LoadDefaultShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	defaultShaderHandle = renderer->CreatePixelShader(data, size, &defaultShaderReflection);
	if (defaultShaderHandle == -1)
		ProfileReturnConst(-1);
	ProfileReturnConst(0);
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
	auto handle = renderer->CreateTexture(rawTextureData, td);
	ProfileReturnConst(handle);
	
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
