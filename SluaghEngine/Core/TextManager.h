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
			TextManager(const InitializationInfo& initInfo);
			~TextManager();

			/**
			* @brief	Bind a renderable text to entity
			*
			* @param[in] entity The entity to bind the renderable text to.
			* @param[in] textInfo Struct with the required information.
			*
			*/
			void Create(CreateInfo info)override;

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
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].text = text;
					loadedTexts[fileLoaded->second.ID].hashString = std::hash<std::wstring>()(text);
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextFontID(const Entity& entity, size_t fontID)override {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].fontID = fontID;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextColour(const Entity& entity, DirectX::XMFLOAT4 colour) override {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].colour = colour;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextPos(const Entity& entity, DirectX::XMFLOAT2 pos)override {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].pos = pos;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextOrigin(const Entity& entity, DirectX::XMFLOAT2 origin)override {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].origin = origin;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextScale(const Entity& entity, DirectX::XMFLOAT2 scale)override {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].scale = scale;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextEffect(const Entity& entity, Graphics::Effect effect)override {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].effect = effect;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextRotation(const Entity& entity, float rotation)override {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].rotation = rotation;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextLayerDepth(const Entity& entity, float layerDepth)override {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[fileLoaded->second.ID].layerDepth = layerDepth;
					if (fileLoaded->second.show == true)
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
				height = inHeight;
				width = inWidth;
			}

			/**
			* @brief Resets all GUI to be rescaled to new resolution
			* @endcode
			*/
			void updateText()override;

		private:
			ResourceHandler::InvokeReturn LoadFont(const Utilz::GUID& font, void*data, size_t size);

			void GarbageCollection()override;
			void Destroy(size_t index)override;
			void Destroy(const Entity& entity)override;

			struct showID
			{
				size_t ID;
				size_t jobID;
				bool show = false;
			};

			// Text variables
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToFont;
			std::unordered_map<Entity, showID, EntityHasher> entID;
			std::vector<Graphics::TextGUI> loadedTexts;
			std::vector<Entity> textEnt;
			std::map<size_t, Entity> textJobobToEnt;
			std::unordered_map<Entity, bool, EntityHasher> dirtyEnt;

			std::default_random_engine generator;

			InitializationInfo initInfo;

			float height = 720.0;
			float width = 1280.0;
		};
	}
}
#endif