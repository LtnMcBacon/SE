#include "GUIManager.h"
#include <Profiler.h>


namespace SE {
	namespace Core {
		GUIManager::GUIManager(const InitializationInfo & initInfo) : initInfo(initInfo)
		{
			_ASSERT(initInfo.resourceHandler);
			_ASSERT(initInfo.renderer);
			_ASSERT(initInfo.entityManager);

			auto ret = initInfo.resourceHandler->LoadResource("moonhouse.spritefont", { this, &GUIManager::LoadFont });
			if (ret)
				throw std::exception("Could not load default font.");
		}

		GUIManager::~GUIManager()
		{

		}

		void GUIManager::CreateRenderableText(const Entity& entity,const Graphics::TextGUI& inTextInfo)
		{
			StartProfile;

			// Check if the entity is alive
			if (!initInfo.entityManager->Alive(entity))
				ProfileReturnVoid;

			if (inTextInfo.fontID >= guidToFont.size())
				ProfileReturnVoid;
			
			entID[entity].ID = loadedTexts.size();
			textEnt.push_back(entity);
			loadedTexts.push_back(inTextInfo);
			if (!loadedTexts[loadedTexts.size() - 1].anchor)
			{
				loadedTexts[loadedTexts.size() - 1].pos = DirectX::XMFLOAT2(loadedTexts[loadedTexts.size() - 1].pos.x / width, loadedTexts[loadedTexts.size() - 1].pos.y / height);
				loadedTexts[loadedTexts.size() - 1].scale = DirectX::XMFLOAT2(loadedTexts[loadedTexts.size() - 1].scale.x / width, loadedTexts[loadedTexts.size() - 1].scale.y / height);
			}
			ProfileReturnVoid;	
		}

		void GUIManager::ToggleRenderableText(const Entity & entity, bool show)
		{
			StartProfile;
			// chexk if entity exist in text 
			auto fileLoaded = entID.find(entity);
			if (fileLoaded != entID.end())
			{
				if (show && !fileLoaded->second.show)
				{
					fileLoaded->second.jobID = initInfo.renderer->EnableTextRendering(loadedTexts[fileLoaded->second.ID]);
					textJobobToEnt[fileLoaded->second.jobID] = entity;
					fileLoaded->second.show = true;
				}
				else if (!show && fileLoaded->second.show)
				{
					size_t tempJobID = initInfo.renderer->DisableTextRendering(fileLoaded->second.jobID);
					fileLoaded->second.show = false;
					entID[textJobobToEnt[tempJobID]].jobID = fileLoaded->second.jobID;
					textJobobToEnt[fileLoaded->second.jobID] = textJobobToEnt[tempJobID];
					textJobobToEnt.erase(tempJobID);
				}
				ProfileReturnVoid;
			}
			StopProfile;
		}

		int GUIManager::CreateTextFont(const Utilz::GUID& fontFile)
		{
			StartProfile;
			auto ret = initInfo.resourceHandler->LoadResource(fontFile, { this, &GUIManager::LoadFont });
			
			ProfileReturnConst(0);
		}

		int GUIManager::Create2D(const Utilz::GUID& texFile)
		{
			StartProfile;
			auto fileLoaded = textureGUID.find(texFile);
			if (fileLoaded == textureGUID.end())
			{
				textureGUID[texFile].textureHandle = -1;
				initInfo.resourceHandler->LoadResource(texFile, { this, &GUIManager::LoadTexture });
				ProfileReturnConst(-1);
			}
			else if (textureGUID[texFile].textureHandle != -1)
			{
				ProfileReturn(textureGUID[texFile].textureHandle);
			}
			ProfileReturnConst(-1);
		}

		int GUIManager::Bind2D(const Entity & entity, Utilz::GUID texFile, Graphics::GUITextureInfo & texInfo)
		{
			StartProfile;
			auto fileLoaded = textureGUID.find(texFile);
			if (fileLoaded != textureGUID.end())
			{
				// Check if the entity is alive
				if (!initInfo.entityManager->Alive(entity))
					ProfileReturnConst(-1);

				entTextureID[entity].ID = textureInfo.size();
				entTextureID[entity].GUID = texFile;
				textureGUID[texFile].refCount++;
				textureEnt.push_back(entity);
				textureInfo.push_back(texInfo);
				textureInfo[textureInfo.size() - 1].textureID = textureGUID[texFile].textureHandle;
				if (!textureInfo[textureInfo.size() - 1].anchor)
				{
					textureInfo[textureInfo.size() - 1].origin = DirectX::XMFLOAT2(textureGUID[texFile].width / 2, textureGUID[texFile].height / 2);
					textureInfo[textureInfo.size() - 1].pos = DirectX::XMFLOAT2(textureInfo[textureInfo.size() - 1].pos.x / width, textureInfo[textureInfo.size() - 1].pos.y / height);
					textureInfo[textureInfo.size() - 1].scale = DirectX::XMFLOAT2(textureInfo[textureInfo.size() - 1].scale.x / width, textureInfo[textureInfo.size() - 1].scale.y / height);
				}
				ProfileReturnConst(0);
			}
			ProfileReturnConst(-1);
		}

		void GUIManager::ToggleRenderableTexture(const Entity & entity, bool show)
		{
			StartProfile;
			// chexk if entity exist in text 
			auto fileLoaded = entTextureID.find(entity);
			if (fileLoaded != entTextureID.end())
			{
				if (show && textureGUID[fileLoaded->second.GUID].textureHandle != -1 && !fileLoaded->second.show)
				{
					fileLoaded->second.jobID = initInfo.renderer->EnableTextureRendering(textureInfo[fileLoaded->second.ID]);
					jobToEnt[fileLoaded->second.jobID] = entity;
					fileLoaded->second.show = true;
				}
				else if (!show && fileLoaded->second.show)
				{
					size_t tempJobID = initInfo.renderer->DisableTextureRendering(fileLoaded->second.jobID);
					fileLoaded->second.show = false;
					entTextureID[jobToEnt[tempJobID]].jobID = fileLoaded->second.jobID;
					jobToEnt[fileLoaded->second.jobID] = jobToEnt[tempJobID];
					jobToEnt.erase(tempJobID);
				}
				ProfileReturnVoid;
			}
			StopProfile;
		}

		void GUIManager::Frame(Utilz::TimeCluster * timer)
		{
			StartProfile;
			timer->Start(CREATE_ID_HASH("GUIManager"));
			GarbageCollection();
			timer->Stop(CREATE_ID_HASH("GUIManager"));
			StopProfile;
		}

		void GUIManager::updateGUI()
		{
			StartProfile;
			if (textureInfo.size() > 0)
			{
				for (auto& entity : textureEnt)
				{
					if (!textureInfo[entTextureID[entity].ID].anchor && entTextureID[entity].show)
					{
						ToggleRenderableTexture(entity, false);
						ToggleRenderableTexture(entity, true);
					}
				}
			}
			if (loadedTexts.size() > 0)
			{
				for (auto& entity : textEnt)
				{
					if (!loadedTexts[entID[entity].ID].anchor && entID[entity].show)
					{
						ToggleRenderableText(entity, false);
						ToggleRenderableText(entity, true);
					}
				}
			}
			StopProfile;
		}

		ResourceHandler::InvokeReturn GUIManager::LoadFont(const Utilz::GUID & font, void * data, size_t size)
		{
			guidToFont[font] = initInfo.renderer->CreateTextFont(data, size);
			return ResourceHandler::InvokeReturn::Success;
		}

		void GUIManager::DestroyText(size_t index)
		{
			StartProfile;
			// Temp variables
			size_t last = loadedTexts.size() - 1;
			const Entity entity = textEnt[index];
			const Entity last_entity = textEnt[last];

			// Copy the data
			textEnt[index] = last_entity;
			loadedTexts[index] = loadedTexts[last];
			entID[last_entity] = entID[entity];

			// Remove last spot 
			entID.erase(entity);
			loadedTexts.pop_back();
			textEnt.pop_back();

			StopProfile;	
		}

		void GUIManager::DestroyTexture(size_t index)
		{
			StartProfile;
			// Temp variables
			size_t last = textureInfo.size() - 1;
			const Entity entity = textureEnt[index];
			const Entity last_entity = textureEnt[last];

			// Copy the data
			textureEnt[index] = last_entity;
			textureInfo[index] = textureInfo[last];
			textureGUID[entTextureID[entity].GUID].refCount--;
			entTextureID[last_entity].ID = entTextureID[entity].ID;

			// Remove last spot 
			entTextureID.erase(entity);
			textureInfo.pop_back();
			textureEnt.pop_back();

			StopProfile;
		}

		void GUIManager::GarbageCollection()
		{
			StartProfile;
			uint32_t alive_in_row = 0;
			if (garbage == false)
			{
				while (loadedTexts.size() > 0 && alive_in_row < 4U)
				{
					std::uniform_int_distribution<uint32_t> distribution(0U, loadedTexts.size() - 1U);
					uint32_t i = distribution(generator);
					if (initInfo.entityManager->Alive(textEnt[i]))
					{
						alive_in_row++;
						continue;
					}
					alive_in_row = 0;
					ToggleRenderableText(textEnt[i], false);
					DestroyText(i);
				}
				garbage = true;
			}
			else
			{
				while (textureInfo.size() > 0 && alive_in_row < 4U)
				{
					std::uniform_int_distribution<uint32_t> distribution(0U, textureInfo.size() - 1U);
					uint32_t i = distribution(generator);
					if (initInfo.entityManager->Alive(textureEnt[i]))
					{
						alive_in_row++;
						continue;
					}
					alive_in_row = 0;
					ToggleRenderableTexture(textureEnt[i], false);
					DestroyTexture(i);
				}
				garbage = false;
			}
			StopProfile;
		}

		void GUIManager::Destroy(size_t index)
		{
		}

		void GUIManager::Destroy(const Entity & entity)
		{
		}

		ResourceHandler::InvokeReturn GUIManager::LoadTexture(const Utilz::GUID & guid, void * data, size_t size)
		{
			StartProfile;
			Graphics::TextureDesc td;
			memcpy(&td, data, sizeof(td));
			/*Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel*/
			if (td.width * td.height * 4 != size - sizeof(td))
				ProfileReturnConst(ResourceHandler::InvokeReturn::Fail);
			void* rawTextureData = ((char*)data) + sizeof(td);
			auto handle = initInfo.renderer->CreateTexture(rawTextureData, td);
			if (handle == -1)
				ProfileReturnConst(ResourceHandler::InvokeReturn::Fail);
			textureGUID[guid].textureHandle = handle;
			textureGUID[guid].refCount = 0;
			textureGUID[guid].height = td.height;
			textureGUID[guid].width = td.width;

			ProfileReturnConst(ResourceHandler::InvokeReturn::DecreaseRefcount);
		}

	}
}