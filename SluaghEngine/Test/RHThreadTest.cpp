#include "RHThreadTest.h"
#include <ResourceHandler\IResourceHandler.h>
#include <Graphics\IRenderer.h>
#include <Window\IWindow.h>
#pragma comment(lib, "ResourceHandler.lib")
#include <Graphics\VertexStructs.h>
#include <Graphics\FileHeaders.h>
#include <atomic>
#include <thread>

namespace SE::Test
{
	struct TextureInfo
	{
		uint32_t numTextures;
		Utilz::GUID bindings[Graphics::ShaderStage::maxTextures];
		Utilz::GUID textures[Graphics::ShaderStage::maxTextures];
	};
	struct MaterialFileData
	{
		Graphics::MaterialAttributes attrib;
		TextureInfo textureInfo;
	};
	RHThreadTest::RHThreadTest()
	{
	}


	RHThreadTest::~RHThreadTest()
	{
	}

	bool SE::Test::RHThreadTest::Run(SE::DevConsole::IConsole * console)
	{
		auto w = Window::CreateNewWindow();
		w->Initialize();

		auto r = Graphics::CreateRenderer();
		Graphics::InitializationInfo gi;
		gi.window = w->GetHWND();
		r->Initialize(gi);

		auto rh = ResourceHandler::CreateResourceHandler();
		ResourceHandler::InitializationInfo ii;
		ii.RAM.max = -1;
		ii.VRAM.max = -1;
		rh->Initialize(ii);

		std::vector<Utilz::GUID> meshes;
		rh->GetAllGUIDsWithExtension("mesh", meshes);

		std::vector<Utilz::GUID> textures;
		rh->GetAllGUIDsWithExtension("jpg", textures);

		std::vector<Utilz::GUID> materials;
		rh->GetAllGUIDsWithExtension("mat", materials);

		std::atomic<int> loaded(0);
		ResourceHandler::Callbacks meshCallbacks;
		meshCallbacks.loadCallback = [r](const Utilz::GUID& guid, void* data, size_t size, void** udata, size_t* usize) {
			auto vertexCount = new size_t;
			*udata = (void*)vertexCount;
			*usize = size;
			int result = 0;
			auto meshHeader = (Graphics::Mesh_Header*)data;
			*vertexCount = meshHeader->nrOfVertices;
			if (meshHeader->vertexLayout == 0) {
				Vertex* v = (Vertex*)(meshHeader + 1);

				result = r->GetPipelineHandler()->CreateVertexBuffer(guid, v, meshHeader->nrOfVertices, sizeof(Vertex));
			}
			else {
				VertexDeformer* v = (VertexDeformer*)(meshHeader + 1);
				result = r->GetPipelineHandler()->CreateVertexBuffer(guid, v, meshHeader->nrOfVertices, sizeof(VertexDeformer));
			}
			if (result < 0)
				return ResourceHandler::LoadReturn::FAIL;
			return ResourceHandler::LoadReturn::SUCCESS;
		};
		meshCallbacks.invokeCallback = [&loaded](const Utilz::GUID& guid, void* data, size_t size)
		{
			loaded++;
			return ResourceHandler::InvokeReturn::SUCCESS;
		};

		ResourceHandler::Callbacks textureCallbacks;
		textureCallbacks.loadCallback = [r](const Utilz::GUID& guid, void* data, size_t size, void** udata, size_t* usize) {
			Graphics::TextureDesc td;
			memcpy(&td, data, sizeof(td));
			*usize = size - sizeof(td);
			/*Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel*/
			if (td.width * td.height * 4 != size - sizeof(td))
				return ResourceHandler::LoadReturn::FAIL;

			void* rawTextureData = ((char*)data) + sizeof(td);
			auto result = r->GetPipelineHandler()->CreateTexture(guid, rawTextureData, td.width, td.height);
			if (result < 0)
				return ResourceHandler::LoadReturn::FAIL;
			return ResourceHandler::LoadReturn::SUCCESS;
		};
		textureCallbacks.invokeCallback = [&loaded](const Utilz::GUID& guid, void* data, size_t size)
		{
			loaded++;
			return ResourceHandler::InvokeReturn::SUCCESS;
		};


		ResourceHandler::Callbacks materialCallbacks;
		materialCallbacks.loadCallback = [r](const Utilz::GUID& guid, void* data, size_t size, void** udata, size_t* usize) {
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
		materialCallbacks.invokeCallback = [&loaded](const Utilz::GUID& guid, void* data, size_t size)
		{
			loaded++;
			return ResourceHandler::InvokeReturn::SUCCESS;
		};

		for (int i = 0; i < 20; i++)
			 rh->LoadResource(meshes[i], meshCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::ASYNC);
		for (int i = 0; i < 20; i++)
			rh->LoadResource(materials[i], materialCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_RAM | ResourceHandler::LoadFlags::ASYNC);
		for (int i = 0; i < 20; i++)
			rh->LoadResource(textures[i], textureCallbacks, ResourceHandler::LoadFlags::LOAD_FOR_VRAM | ResourceHandler::LoadFlags::ASYNC);


		using namespace std::chrono_literals;
		while (loaded.load() < 60)
			std::this_thread::sleep_for(1ms);




		r->Shutdown();
		w->Shutdown();
		rh->Shutdown();
		delete rh;
		delete r;
		delete w;
		return false;
	}
}