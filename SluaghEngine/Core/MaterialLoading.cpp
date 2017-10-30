#include "MaterialLoading.h"
static const SE::Utilz::GUID defaultTexture("ft_stone01_c.sei");

int SE::Core::MaterialLoading::LoadMaterialFile(const Utilz::GUID& material)
{
	int result = 0;
	if (!IsMaterialFileLoaded(material))
	{
		auto& matInfo = guidToMaterial[material];
		result = resourceHandler->LoadResource(material,
			[this, &matInfo](auto guid, auto data, auto size) {

			size_t offset = sizeof(uint32_t);
			memcpy(&matInfo.textureInfo.numTextures, (char*)data, sizeof(uint32_t));
			if (matInfo.textureInfo.numTextures > Graphics::ShaderStage::maxTextures)
				return ResourceHandler::InvokeReturn::Fail;

			memcpy(&matInfo.attrib, (char*)data + offset, sizeof(Graphics::MaterialAttributes));
			offset += sizeof(Graphics::MaterialAttributes);
			for (int i = 0; i < matInfo.textureInfo.numTextures; i++)
			{
				memcpy(&matInfo.textureInfo.textures[i], (char*)data + offset, sizeof(Utilz::GUID));
				offset += sizeof(Utilz::GUID);
				memcpy(&matInfo.textureInfo.bindings[i], (char*)data + offset, sizeof(Utilz::GUID));
				offset += sizeof(Utilz::GUID);
			}

			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		});
	}

	return result;
}

SE::Core::MaterialFileData & SE::Core::MaterialLoading::GetMaterialFile(const Utilz::GUID & guid)
{
	return guidToMaterial[guid];
}

bool SE::Core::MaterialLoading::IsMaterialFileLoaded(const Utilz::GUID & guid) const
{
	auto findMFile = guidToMaterial.find(guid);
	return findMFile != guidToMaterial.end();
}

void SE::Core::MaterialLoading::UnloadMaterialFile(const Utilz::GUID & guid, const Entity& entity)
{
	auto& material = guidToMaterial[guid];
	
	material.entities.remove(entity);
	for (size_t i = 0; i < material.textureInfo.numTextures; i++)
	{
		auto& texture = guidToTexture[material.textureInfo.textures[i]];
		texture.refCount--;
	}

}

int SE::Core::MaterialLoading::LoadShaderAndMaterialFileAndTextures(const Utilz::GUID & shader, const Utilz::GUID & materialFile, bool async, ResourceHandler::Behavior behavior)
{

	//// Load the shader, material and textures
	//resourceHandler->LoadResource(shader, [this, materialFile](auto guid, auto data, auto size)
	//{
	//	// Create the shader
	//	auto result = this->initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
	//	if (result < 0)
	//		return ResourceHandler::InvokeReturn::Fail;
	//	

	//	// Load the material
	//	FullUpdateStruct fus;
	//	result = LoadMaterialFile(materialFile, fus.mdata);
	//	if (result < 0)
	//		return ResourceHandler::InvokeReturn::Fail;

	//	// Load all the textures
	//	result = LoadTextures(mdata, false, ResourceHandler::Behavior::QUICK);
	//	if (result < 0)
	//		return ResourceHandler::InvokeReturn::Fail;

	//	// All things are now loaded
	//	toUpdateFull.push({guid, materialFile, mdata})

	//	return ResourceHandler::InvokeReturn::DecreaseRefcount;
	//},async, behavior);
	return 0;
}

int SE::Core::MaterialLoading::LoadShaderAndTextures(const Utilz::GUID & shader, const Utilz::GUID & materialFile, bool async, ResourceHandler::Behavior behavior)
{
	return 0;
}

int SE::Core::MaterialLoading::LoadMaterialFileAndTextures(const Utilz::GUID & materialFile, bool async, ResourceHandler::Behavior behavior)
{
	return 0;
}

int SE::Core::MaterialLoading::LoadTextures(const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior, const std::function<void(const Utilz::GUID&, int)>& errorCallback)
{
	auto& material = guidToMaterial[materialFile];
	for (uint8_t i = 0; i < material.textureInfo.numTextures; i++)
	{
		bool isLoaded = IsTextureLoaded(material.textureInfo.textures[i]);
		auto& texture = guidToTexture[material.textureInfo.textures[i]];
		if (!isLoaded)
		{
			resourceHandler->LoadResource(material.textureInfo.textures[i],
				[this](auto guid, auto data, auto size) {
				auto result = LoadTexture(guid, data, size);
				if (result < 0)
					return ResourceHandler::InvokeReturn::Fail;
				return ResourceHandler::InvokeReturn::DecreaseRefcount;
			}, async, behavior);
		}
	}

	return 0;
}

int SE::Core::MaterialLoading::LoadTexture(const Utilz::GUID & texture)
{
	int result = 0;
	if (!IsTextureLoaded(texture))
	{
	
		result= resourceHandler->LoadResource(texture,
			[this](auto guid, auto data, auto size) {
			auto result = LoadTexture(guid, data, size);
			if (result < 0)
				return ResourceHandler::InvokeReturn::Fail;


			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		});
	}
	if(result >= 0)
		guidToTexture[texture].refCount ++;

	return result;
}

void SE::Core::MaterialLoading::LoadTextures(const Entity& entity, const Utilz::GUID & materialFile, bool async, ResourceHandler::Behavior behavior)
{
	auto& mat = guidToMaterial[materialFile];
	for (uint8_t i = 0; i < mat.textureInfo.numTextures; i++)
	{
		auto& t = guidToTexture.find(mat.textureInfo.textures[i]);
		if (t == guidToTexture.end())
		{
			auto temp = mat.textureInfo.textures[i];
			mat.textureInfo.textures[i] = defaultTexture;
			auto result = resourceHandler->LoadResource(temp, [this, materialFile, &mat, i, async, entity](auto guid, auto data, auto size)
			{
				auto texture = guid;
				auto result = LoadTexture(guid, data, size);
				if (result < 0)
				{
					console->PrintChannel("Resources", "Could not load texture. Using default instead. GUID: %u, Error: %d\n", guid, result);
					texture = defaultTexture;
				}

				if (async)
				{
					toUpdate.push({ materialFile , i , texture , entity});
				}
				else
				{
					mat.textureInfo.textures[i] = texture;
				}
				return ResourceHandler::InvokeReturn::DecreaseRefcount;
			}, true, behavior);
		}
	}
}

bool SE::Core::MaterialLoading::IsTextureLoaded(const Utilz::GUID & guid) const
{
	auto findTexture = guidToTexture.find(guid);
	return findTexture != guidToTexture.end();
}

int SE::Core::MaterialLoading::LoadTexture(const Utilz::GUID & guid, void * data, size_t size)const
{
	Graphics::TextureDesc td;
	memcpy(&td, data, sizeof(td));

	/*Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel*/
	if (td.width * td.height * 4 != size - sizeof(td))
		return -1;

	void* rawTextureData = ((char*)data) + sizeof(td);
	return renderer->GetPipelineHandler()->CreateTexture(guid, rawTextureData, td.width, td.height);
}

int SE::Core::MaterialLoading::LoadShader(const Utilz::GUID & shader)
{
	int result = 0;
	if (!IsShaderLoaded(shader))
	{
		result = resourceHandler->LoadResource(shader, [this](auto guid, auto data, auto size)
		{
			auto result = renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
			if (result < 0)
				return ResourceHandler::InvokeReturn::Fail;
			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		});
	}
	if(result >= 0)
		guidToShader[shader].refCount++;
	return result;
}

bool SE::Core::MaterialLoading::IsShaderLoaded(const Utilz::GUID & guid) const
{
	auto findShader = guidToShader.find(guid);
	return findShader != guidToShader.end();
}

SE::Core::MaterialLoading::MaterialLoading(Graphics::IRenderer * renderer, ResourceHandler::IResourceHandler * resourceHandler, DevConsole::IConsole* console)
	: renderer(renderer), resourceHandler(resourceHandler), console(console)
{
}

SE::Core::MaterialLoading::~MaterialLoading()
{
}

void SE::Core::MaterialLoading::LoadStuff(const LoadInfo& info, bool async, ResourceHandler::Behavior b)
{
	//shaderLock.lock();
	//
	//auto result = renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
	//if (result < 0)
	//{
	//	console->PrintChannel("Resources", "Could not load shader. Using default instead. GUID: %u, Error: %d\n", info.shader, result);
	//	//materialInfo.shader[newEntry] = defaultPixelShader;
	//}
	//shaderLock.unlock();
	//materialLock.lock();
	//const auto findM = guidToMaterial.find(info.material);
	//if (findM == guidToMaterial.end())
	//{
	//	result = resourceHandler->LoadResource(info.material, [](auto guid, auto data, auto size) {
	//		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	//	}, false, b);
	//	if (result < 0)
	//	{
	//		console->PrintChannel("Resources", "Could not load material. Using default instead. GUID: %u, Error: %d\n", info.material, result);
	//		//materialInfo.material[newEntry] = defaultMaterial;
	//	}
	//}
	//materialLock.unlock();
	//auto& mat = guidToMaterial[info.material];
	//for (uint8_t i = 0; i < mat.textureInfo.numTextures; i++)
	//{

	//}
	//auto result = resourceHandler->LoadResource(info.shader, [](auto guid, auto data, auto size) {
	//	return ResourceHandler::InvokeReturn::DecreaseRefcount;
	//}, async, b);

}

bool SE::Core::MaterialLoading::DoUpdate(std::vector<Entity>& entitiesToUpdate)
{
	bool s = false;
	while (!toUpdate.wasEmpty())
	{
		s = true;
		auto& top = toUpdate.top();

		auto& mat = guidToMaterial[top.material];
		mat.textureInfo.textures[top.index] = top.texture;
		entitiesToUpdate.push_back(top.entity);
		toUpdate.pop();
	}

	return s;
}
