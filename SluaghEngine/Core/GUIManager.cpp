#include "GUIManager.h"
#include <Profiler.h>


namespace SE {
	namespace Core {
		GUIManager::GUIManager(const InitializationInfo & initInfo) : initInfo(initInfo)
		{
			_ASSERT(initInfo.resourceHandler);
			_ASSERT(initInfo.renderer);
			_ASSERT(initInfo.entityManager);
		}

		GUIManager::~GUIManager()
		{
			for (auto& t : textureInfo)
				delete t.rect;
		}

		int GUIManager::Create(CreateInfo info)
		{
			StartProfile;
			auto fileLoaded = textureGUID.find(info.texFile);
			if (fileLoaded == textureGUID.end())
			{
				textureGUID[info.texFile].textureHandle = -1;
				initInfo.resourceHandler->LoadResource(info.texFile, { this, &GUIManager::LoadTexture });
			}

			// Check if the entity is alive
			if (!initInfo.entityManager->Alive(info.entity))
				ProfileReturnConst(-1);

			auto entLoaded = entTextureID.find(info.entity);
			if (entLoaded == entTextureID.end())
			{
				entTextureID[info.entity].ID = textureInfo.size();
				entTextureID[info.entity].GUID = info.texFile;
				textureGUID[info.texFile].refCount++;
				textureEnt.push_back(info.entity);
				textureInfo.push_back(info.texInfo);
				textureInfo[textureInfo.size() - 1].textureID = textureGUID[info.texFile].textureHandle;
				if (!textureInfo[textureInfo.size() - 1].anchor)
				{
					textureInfo[textureInfo.size() - 1].origin = DirectX::XMFLOAT2(textureGUID[info.texFile].width / 2, textureGUID[info.texFile].height / 2);
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
			for (auto& dirt : dirtyEnt)
			{
				// Check if the entity is alive
				if (!initInfo.entityManager->Alive(dirt.first))
					continue;
				if (dirt.second == true && entTextureID[dirt.first].show == true)
				{
					ToggleRenderableTexture(dirt.first, false);
					ToggleRenderableTexture(dirt.first, true);
				}
				dirt.second = false;
			}
			dirtyEnt.clear();
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
			StopProfile;
		}

		void GUIManager::Destroy(size_t index)
		{
			StartProfile;
			// Temp variables
			size_t last = textureInfo.size() - 1;
			const Entity entity = textureEnt[index];
			const Entity last_entity = textureEnt[last];

			// Copy the data
			textureEnt[index] = last_entity;
			delete textureInfo[index].rect;
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
				Destroy(i);
			}
			StopProfile;
		}

		void GUIManager::Destroy(const Entity & entity)
		{
			StartProfile;
			auto fileLoaded = entTextureID.find(entity);
			if (fileLoaded != entTextureID.end())
			{
				// Temp variables
				size_t last = textureInfo.size() - 1;
				size_t index = entTextureID[entity].ID;
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
			}

			StopProfile;
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