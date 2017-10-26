#include "TextManager.h"
#include <Profiler.h>


namespace SE {
	namespace Core {
		TextManager::TextManager(const InitializationInfo & initInfo) : initInfo(initInfo)
		{
			_ASSERT(initInfo.resourceHandler);
			_ASSERT(initInfo.renderer);
			_ASSERT(initInfo.entityManager);

			auto ret = initInfo.resourceHandler->LoadResource("moonhouse.spritefont", { this, &TextManager::LoadFont });
			if (ret)
				throw std::exception("Could not load default font.");
		}

		TextManager::~TextManager()
		{

		}

		void TextManager::Create(CreateInfo info)
		{
			StartProfile;

			// Check if the entity is alive
			if (!initInfo.entityManager->Alive(info.entity))
				ProfileReturnVoid;

			if (info.inTextInfo.fontID >= guidToFont.size())
				ProfileReturnVoid;

			entID[info.entity].ID = loadedTexts.size();
			textEnt.push_back(info.entity);
			loadedTexts.push_back(info.inTextInfo);
			if (!loadedTexts[loadedTexts.size() - 1].anchor)
			{
				loadedTexts[loadedTexts.size() - 1].pos = DirectX::XMFLOAT2(loadedTexts[loadedTexts.size() - 1].pos.x / width, loadedTexts[loadedTexts.size() - 1].pos.y / height);
				loadedTexts[loadedTexts.size() - 1].scale = DirectX::XMFLOAT2(loadedTexts[loadedTexts.size() - 1].scale.x / width, loadedTexts[loadedTexts.size() - 1].scale.y / height);
			}
			ProfileReturnVoid;
		}

		void TextManager::ToggleRenderableText(const Entity & entity, bool show)
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

		int TextManager::MakeFont(const Utilz::GUID& fontFile)
		{
			StartProfile;
			auto ret = initInfo.resourceHandler->LoadResource(fontFile, { this, &TextManager::LoadFont });

			ProfileReturnConst(0);
		}

		void TextManager::Frame(Utilz::TimeCluster * timer)
		{
			StartProfile;
			timer->Start(CREATE_ID_HASH( "GUIManager"));
			for (auto& dirt : dirtyEnt)
			{
				// Check if the entity is alive
				if (!initInfo.entityManager->Alive(dirt.first))
					continue;
				if (dirt.second == true && entID[dirt.first].show == true)
				{
					ToggleRenderableText(dirt.first, false);
					ToggleRenderableText(dirt.first, true);
				}
				dirt.second = false;
			}
			dirtyEnt.clear();
			GarbageCollection();
			timer->Stop(CREATE_ID_HASH("GUIManager"));
			StopProfile;
		}

		void TextManager::updateText()
		{
			StartProfile;
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

		ResourceHandler::InvokeReturn TextManager::LoadFont(const Utilz::GUID & font, void * data, size_t size)
		{
			guidToFont[font] = initInfo.renderer->CreateTextFont(data, size);
			return ResourceHandler::InvokeReturn::Success;
		}

		void TextManager::Destroy(size_t index)
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

		void TextManager::GarbageCollection()
		{
			StartProfile;
			uint32_t alive_in_row = 0;
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
				Destroy(i);
			}
			StopProfile;
		}

		void TextManager::Destroy(const Entity & entity)
		{
			StartProfile;
			auto fileLoaded = entID.find(entity);
			if (fileLoaded != entID.end())
			{
				// Temp variables
				size_t last = loadedTexts.size() - 1;
				size_t index = entID[entity].ID;
				const Entity last_entity = textEnt[last];

				// Copy the data
				textEnt[index] = last_entity;
				loadedTexts[index] = loadedTexts[last];
				entID[last_entity] = entID[entity];

				// Remove last spot 
				entID.erase(entity);
				loadedTexts.pop_back();
				textEnt.pop_back();
			}

			StopProfile;
		}
	}
}