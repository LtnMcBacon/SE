#include "MaterialLoading.h"

int SE::Core::MaterialLoading::LoadMaterialFile(const Utilz::GUID& material, MaterialFileData & matInfo)
{
	return resourceHandler->LoadResource(material,
		[this, &matInfo](auto guid, auto data, auto size) {

		size_t offset = sizeof(uint32_t);
		memcpy(&matInfo.textureInfo.amountOfTex, (char*)data, sizeof(uint32_t));
		if (matInfo.textureInfo.amountOfTex > Graphics::ShaderStage::maxTextures)
			return ResourceHandler::InvokeReturn::Fail;

		memcpy(&matInfo.attrib, (char*)data + offset, sizeof(Graphics::MaterialAttributes));
		offset += sizeof(Graphics::MaterialAttributes);
		for (int i = 0; i < matInfo.textureInfo.amountOfTex; i++)
		{
			memcpy(&matInfo.textureInfo.textures[i], (char*)data + offset, sizeof(Utilz::GUID));
			offset += sizeof(Utilz::GUID);
			memcpy(&matInfo.textureInfo.bindings[i], (char*)data + offset, sizeof(Utilz::GUID));
			offset += sizeof(Utilz::GUID);
		}

		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
}

bool SE::Core::MaterialLoading::IsMaterialFileLoaded(const Utilz::GUID & guid) const
{
	auto findMFile = guidToMaterial.find(guid);
	return findMFile != guidToMaterial.end();
}

int SE::Core::MaterialLoading::LoadShaderAndMaterialFileAndTextures(const Utilz::GUID & shader, const Utilz::GUID & materialFile, bool async, ResourceHandler::Behavior behavior)
{
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

int SE::Core::MaterialLoading::LoadTextures(MaterialFileData & material, bool async, ResourceHandler::Behavior behavior)
{
	for (uint8_t i = 0; i < material.textureInfo.numTextures; i++)
	{
		//auto findTexture = 
	}

	return 0;
}

int SE::Core::MaterialLoading::LoadTexture(const Utilz::GUID & texture)
{

	return resourceHandler->LoadResource(texture,
		[this](auto guid, auto data, auto size) {
		auto result = LoadTexture(guid, data, size);
		if (result < 0)
			return ResourceHandler::InvokeReturn::Fail;
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});

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
	return resourceHandler->LoadResource(shader, [this](auto guid, auto data, auto size)
	{
		auto result = this->initInfo.renderer->GetPipelineHandler()->CreatePixelShader(guid, data, size);
		if (result < 0)
			return ResourceHandler::InvokeReturn::Fail;
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	return 0;
}

bool SE::Core::MaterialLoading::IsShaderLoaded(const Utilz::GUID & guid) const
{
	auto findShader = guidToShader.find(guid);
	return findShader != guidToShader.end();
}

SE::Core::MaterialLoading::MaterialLoading(Graphics::IRenderer * renderer, ResourceHandler::IResourceHandler * resourceHandler) : renderer(renderer), resourceHandler(resourceHandler)
{
}

SE::Core::MaterialLoading::~MaterialLoading()
{
}
