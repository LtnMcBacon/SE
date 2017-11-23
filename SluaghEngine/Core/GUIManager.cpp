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

		}

		int GUIManager::Create(const Entity& entity, const CreateInfo& info)
		{
			StartProfile;
			auto fileLoaded = textureGUID.find(info.texture);
			if (fileLoaded == textureGUID.end())
			{
				textureGUID[info.texture].textureHandle = -1;
				auto res  = initInfo.resourceHandler->LoadResource(info.texture, [this](auto guid, auto data, auto size) {
					Graphics::TextureDesc td;
					memcpy(&td, data, sizeof(td));
					/*Ensure the size of the raw pixel data is the same as the width x height x size_per_pixel*/
					if (td.width * td.height * 4 != size - sizeof(td))
						return ResourceHandler::InvokeReturn::FAIL;
					void* rawTextureData = ((char*)data) + sizeof(td);
					auto handle = initInfo.renderer->CreateTexture(rawTextureData, td);
					if (handle == -1)
						return ResourceHandler::InvokeReturn::FAIL;
					textureGUID[guid].textureHandle = handle;
					textureGUID[guid].refCount = 0;
					textureGUID[guid].height = td.height;
					textureGUID[guid].width = td.width; 
					return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
				});
				if (res < 0)
					textureGUID[info.texture].textureHandle = 0;
			}

			// Check if the entity is alive
			if (!initInfo.entityManager->Alive(entity))
				ProfileReturnConst(-1);

			auto entLoaded = entTextureID.find(entity);
			if (entLoaded == entTextureID.end())
			{
				entTextureID[entity].ID = textureInfo.size();
				entTextureID[entity].GUID = info.texture;
				textureGUID[info.texture].refCount++;
				textureEnt.push_back(entity);


				
				textureInfo.push_back(info.textureInfo);
				entTextureID[entity].textureHandle = textureGUID[info.texture].textureHandle;
				
				auto& ti = textureInfo[textureInfo.size() - 1];
				if (ti.width == -1)
				{
					ti.width = textureGUID[info.texture].width;

				}
				else
				{
					ti.scale.x = ti.width / (float)textureGUID[info.texture].width;
					ti.width = textureGUID[info.texture].width;

				}
				
				if (ti.height == -1)
				{
					ti.height = textureGUID[info.texture].height;
				}
				else
				{
					ti.scale.y = ti.height / (float)textureGUID[info.texture].height;
					ti.height = textureGUID[info.texture].height;
				}
				
				//ti.origin = { ti.posX + ti.origin.x, ti.posY + ti.origin.y };

				/*if (!textureInfo[textureInfo.size() - 1].anchor)
				{
					textureInfo[textureInfo.size() - 1].origin = DirectX::XMFLOAT2(textureGUID[info.texture].width / 2, textureGUID[info.texture].height / 2);
					textureInfo[textureInfo.size() - 1].pos = DirectX::XMFLOAT2(textureInfo[textureInfo.size() - 1].pos.x / width, textureInfo[textureInfo.size() - 1].pos.y / height);
					textureInfo[textureInfo.size() - 1].scale = DirectX::XMFLOAT2(textureInfo[textureInfo.size() - 1].scale.x / width, textureInfo[textureInfo.size() - 1].scale.y / height);
				}*/
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
					auto& ti = textureInfo[fileLoaded->second.ID];
				//	Graphics::GUITextureInfo scaled;
				//	scaled.posX = static_cast<long>(ti.posX / );
					fileLoaded->second.jobID = initInfo.renderer->EnableTextureRendering({ fileLoaded->second.textureHandle, originalScreenWidth, originalScreenHeight, ti });
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
			timer->Start(("GUIManager"));
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
			timer->Stop(("GUIManager"));
			StopProfile;
		}

		void GUIManager::updateGUI()
		{
			StartProfile;
			if (textureInfo.size() > 0)
			{
				for (auto& entity : textureEnt)
				{
					if (entTextureID[entity].show)
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
			textureInfo[index] = textureInfo[last];
			textureGUID[entTextureID[entity].GUID].refCount--;
			if(textureGUID[entTextureID[entity].GUID].refCount == 0)
			{
				initInfo.renderer->GetPipelineHandler()->DestroyTexture(entTextureID[entity].GUID);
			}
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
				const Entity currentEntity = textureEnt[index];
				const Entity last_entity = textureEnt[last];

				// Copy the data
				textureEnt[index] = last_entity;
				textureInfo[index] = textureInfo[last];
				textureGUID[entTextureID[currentEntity].GUID].refCount--;
				if (textureGUID[entTextureID[entity].GUID].refCount == 0)
				{
					initInfo.renderer->GetPipelineHandler()->DestroyTexture(entTextureID[entity].GUID);
				}
				entTextureID[last_entity].ID = entTextureID[currentEntity].ID;

				// Remove last spot 
				entTextureID.erase(currentEntity);
				textureInfo.pop_back();
				textureEnt.pop_back();
			}

			StopProfile;
		}


	}
}