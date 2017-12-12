#include "TextManager.h"
#include <Profiler.h>


namespace SE {
	namespace Core {
		TextManager::TextManager(const InitializationInfo & initInfo) : initInfo(initInfo)
		{
			_ASSERT(initInfo.resourceHandler);
			_ASSERT(initInfo.renderer);
			_ASSERT(initInfo.entityManager);

			auto ret = initInfo.resourceHandler->LoadResource("moonhouse.spritefont", [this](auto guid, auto data, auto size) {
				guidToFont[guid] = this->initInfo.renderer->CreateTextFont(data, size);
				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM; });
			if (ret)
				throw std::exception("Could not load default font.");
		}

		TextManager::~TextManager()
		{

		}

		void TextManager::Create(const Entity& entity, const CreateInfo& info)
		{
			StartProfile;

			// Check if the entity is alive
			if (!initInfo.entityManager->Alive(entity))
				ProfileReturnVoid;
			auto font = info.font;
			if (info.font == Utilz::GUID())
			{
				font = "moonhouse.spritefont";
			}
			else
			{
				auto const findFont = guidToFont.find(info.font);
				if (findFont == guidToFont.end())
				{
					auto ret = initInfo.resourceHandler->LoadResource(info.font, [this](auto guid, auto data, auto size) {
						guidToFont[guid] = this->initInfo.renderer->CreateTextFont(data, size);
						return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM; });
					if (ret)
					{
						ProfileReturnVoid;
					}
				}
			}

			auto const find = entityToEntry.find(entity);
			if (find == entityToEntry.end())
			{
				entityToEntry[entity].info = info.info;
				entityToEntry[entity].font = font;
				indexToEntity.push_back(entity);
			}
			ProfileReturnVoid;
		}

		void TextManager::ToggleRenderableText(const Entity & entity, bool show)
		{
			StartProfile;
			// chexk if entity exist in text 
			auto entry = entityToEntry.find(entity);
			if (entry != entityToEntry.end())
			{
				if (show && !entry->second.show)
				{
					Graphics::TextJob tj;
					tj.fontID = guidToFont[entry->second.font];
					tj.originalScreenHeight = originalScreenHeight;
					tj.originalScreenWidth = originalScreenWidth;
					tj.info = entry->second.info;
					entry->second.jobID = initInfo.renderer->EnableTextRendering(tj);
					entry->second.show = true;
				}
				else if (!show && entry->second.show)
				{
					initInfo.renderer->DisableTextRendering(entry->second.jobID);
					entry->second.show = false;
				}
			}
			StopProfile;
		}

		int TextManager::MakeFont(const Utilz::GUID& fontFile)
		{
			StartProfile;
			auto ret = initInfo.resourceHandler->LoadResource(fontFile, [this](auto guid, auto data, auto size) {
				guidToFont[guid] = initInfo.renderer->CreateTextFont(data, size);
				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM ; });

			ProfileReturnConst(0);
		}

		void TextManager::Frame(Utilz::TimeCluster * timer)
		{
			StartProfile;
			timer->Start(( "GUIManager"));
			for (auto& dirt : dirtyEnt)
			{
				// Check if the entity is alive
				if (!initInfo.entityManager->Alive(dirt.first))
					continue;
				if (dirt.second == true && entityToEntry[dirt.first].show == true)
				{
					ToggleRenderableText(dirt.first, false);
					ToggleRenderableText(dirt.first, true);
				}
				dirt.second = false;
			}
			dirtyEnt.clear();
			GarbageCollection();
			timer->Stop(("GUIManager"));
			StopProfile;
		}

		void TextManager::updateText()
		{
			StartProfile;

			for (auto& entity : entityToEntry)
			{
				if (entity.second.show)
				{
					ToggleRenderableText(entity.first, false);
					ToggleRenderableText(entity.first, true);
				}
			}

			StopProfile;
		}

		void TextManager::Destroy(size_t index)
		{
			StartProfile;
			// Temp variables
			size_t last = indexToEntity.size() - 1;

			entityToEntry.erase(indexToEntity[index]);
			indexToEntity[index] = indexToEntity[last];

			// Remove last spot 
			indexToEntity.pop_back();
			StopProfile;
		}

		void TextManager::GarbageCollection()
		{
			StartProfile;
			uint32_t alive_in_row = 0;
			while (indexToEntity.size() > 0 && alive_in_row < 40U)
			{
				std::uniform_int_distribution<uint32_t> distribution(0U, indexToEntity.size() - 1U);
				uint32_t i = distribution(generator);
				if (initInfo.entityManager->Alive(indexToEntity[i]))
				{
					alive_in_row++;
					continue;
				}
				alive_in_row = 0;
				ToggleRenderableText(indexToEntity[i], false);
				Destroy(i);
			}
			StopProfile;
		}

		void TextManager::Destroy(const Entity & entity)
		{
			StartProfile;
			//auto entry = entityToEntry.find(entity);
			//if (entry != entityToEntry.end())
			//{
			//	// Temp variables
			//	size_t last = loadedTexts.size() - 1;
			//	size_t index = entID[entity].ID;
			//	const Entity currentEntity = textEnt[index];
			//	const Entity last_entity = textEnt[last];

			//	// Copy the data
			//	textEnt[index] = last_entity;
			//	loadedTexts[index] = loadedTexts[last];
			//	entID[last_entity].ID = entID[currentEntity].ID;

			//	// Remove last spot 
			//	entID.erase(currentEntity);
			//	loadedTexts.pop_back();
			//	textEnt.pop_back();
			//}

			StopProfile;
		}
	}
}