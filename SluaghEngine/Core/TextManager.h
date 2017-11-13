#pragma once
#ifndef SE_CORE_TEXT_MANAGER_H_
#define SE_CORE_TEXT_MANAGER_H_

#include <ITextManager.h>
#include <unordered_map>
#include <stack>
#include <vector>
#include <random>
#include <map>

namespace SE
{
	namespace Core
	{
		/**
		*
		* @brief This class is used to bind renderable text and texture to an entity.
		*
		* @details Use this class when you want an entity to be rendered as a text or texture in the GUI.
		*
		**/
		class TextManager : public ITextManager
		{
		public:
			TextManager(const ITextManager::InitializationInfo& initInfo);
			~TextManager();

			/**
			* @brief	Bind a renderable text to entity
			*
			* @param[in] entity The entity to bind the renderable text to.
			* @param[in] textInfo Struct with the required information.
			*
			*/
			void Create(const Entity& entity, const CreateInfo& info)override;

			/**
			* @brief Create a new font
			* @param[in] fontFile The GUID to the font file.
			* @retval 0+ Font ID
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int MakeFont(const Utilz::GUID& fontFile)override;

			/**
			* @brief	Hide/Show the renderable text
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			void ToggleRenderableText(const Entity& entity, bool show)override;

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;

			inline void SetText(const Entity& entity, std::wstring text)override {
				// chexk if entity exist in text 
				auto entry = entityToEntry.find(entity);
				if (entry != entityToEntry.end())
				{
					entry->second.info.text = text;
					entry->second.info.hashString = std::hash<std::wstring>()(text);
					if (entry->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextFontID(const Entity& entity, size_t fontID)override {
				// chexk if entity exist in text 
			/*	auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].fontID = fontID;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}*/
			};

			inline void SetTextColour(const Entity& entity, DirectX::XMFLOAT4 colour) override {
				// chexk if entity exist in text 
				auto entry = entityToEntry.find(entity);
				if (entry != entityToEntry.end())
				{
					entry->second.info.colour = colour;
					if (entry->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextPos(const Entity& entity, long x, long y)override {
				// chexk if entity exist in text 
				auto entry = entityToEntry.find(entity);
				if (entry != entityToEntry.end())
				{
					entry->second.info.posX = x;
					entry->second.info.posY = y;
					if (entry->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextScreenAnchor(const Entity& entity, DirectX::XMFLOAT2 anchor)override {
				// chexk if entity exist in text 
				auto entry = entityToEntry.find(entity);
				if (entry != entityToEntry.end())
				{
					entry->second.info.screenAnchor = anchor;
					if (entry->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextScale(const Entity& entity, DirectX::XMFLOAT2 scale)override {
				// chexk if entity exist in text 
				auto entry = entityToEntry.find(entity);
				if (entry != entityToEntry.end())
				{
					entry->second.info.scale = scale;
					if (entry->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextEffect(const Entity& entity, Graphics::Effect effect)override {
				// chexk if entity exist in text 
				auto entry = entityToEntry.find(entity);
				if (entry != entityToEntry.end())
				{
					entry->second.info.effect = effect;
					if (entry->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextRotation(const Entity& entity, float rotation)override {
				// chexk if entity exist in text 
				auto entry = entityToEntry.find(entity);
				if (entry != entityToEntry.end())
				{
					entry->second.info.rotation = rotation;
					if (entry->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextLayerDepth(const Entity& entity, float layerDepth)override {
				// chexk if entity exist in text 
				auto entry = entityToEntry.find(entity);
				if (entry != entityToEntry.end())
				{
					entry->second.info.layerDepth = layerDepth;
					if (entry->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			/**
			* @brief Sets the default height and width to be used in GUI scale calc
			* @param[in] inHeight The height.
			* @param[in] inWidth The width.
			* @endcode
			*/
			void SetDefaultScale(float inHeight, float inWidth)override
			{
				originalScreenHeight = inHeight;
				originalScreenWidth = inWidth;
			}

			/**
			* @brief Resets all GUI to be rescaled to new resolution
			* @endcode
			*/
			void updateText()override;

		private:
			void GarbageCollection()override;
			void Destroy(size_t index)override;
			void Destroy(const Entity& entity)override;

			struct Entry
			{
				Utilz::GUID font;
				Graphics::TextGUI info;
				size_t jobID;
				bool show = false;
			};

			// Text variables
			std::unordered_map<Entity, Entry, EntityHasher> entityToEntry;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToFont;
			std::vector<Entity> indexToEntity;

			std::unordered_map<Entity, bool, EntityHasher> dirtyEnt;

			std::default_random_engine generator;

			InitializationInfo initInfo;

			long originalScreenWidth = 1280;
			long originalScreenHeight = 720;
		};
	}
}
#endif