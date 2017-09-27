#pragma once
#ifndef SE_CORE_GUI_MANAGER_H_
#define SE_CORE_GUI_MANAGER_H_
#include "EntityManager.h"
#include <Utilz\GUID.h>
#include <unordered_map>
#include <stack>
#include <vector>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>
#include <Graphics\GUIInfo.h>
#include <random>


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
		class GUIManager
		{
		public:
			GUIManager(ResourceHandler::IResourceHandler* resourceHandler, Graphics::IRenderer* renderer, const EntityManager& entityManager);
			~GUIManager();

			/**
			* @brief	Bind a renderable text to entity
			*
			* @param[in] entity The entity to bind the renderable text to.
			* @param[in] textInfo Struct with the required information.
			*
			*/
			void CreateRenderableText(const Entity& entity, Graphics::TextGUI textInfo);

			/**
			* @brief Create a new font
			* @param[in] fontFile The GUID to the font file.
			* @retval 0+ Font ID
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int CreateTextFont(Utilz::GUID fontFile);

			/**
			* @brief Create a new 2D texture for GUI
			* @param[in] texFile The GUID to the texture file.
			* @retval 0+ Texture ID
			* @retval -1 Already loaded or currently loading.
			* @endcode
			*/
			int Create2D(Utilz::GUID texFile);

			/**
			* @brief	Hide/Show the renderable text
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			void ToggleRenderableText(const Entity& entity, bool show);

			/**
			* @brief	Hide/Show the renderable texture
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			void ToggleRenderableTexture(const Entity& entity, bool show);

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame();

			/**
			* @brief Create a new 2D texture for GUI
			* @param[in] entity Which entity.
			* @param[in] texFile The GUID to the texture file to use.
			* @retval 0 Bind successful
			* @retval -1 Entity not alive or texFile none existing.
			* @endcode
			*/
			int Bind2D(const Entity& entity, Utilz::GUID texFile, Graphics::GUITextureInfo& texInfo);

			
			// sets for Text
			inline void SetText(const Entity& entity, std::wstring text) {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[entID[entity]].text = text;
					loadedTexts[entID[entity]].hashString = std::hash<std::wstring>()(text);
				}
			};

			inline void SetTextFontID(const Entity& entity, size_t fontID) {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[entID[entity]].fontID = fontID;
				}
			};

			inline void SetTextColour(const Entity& entity, DirectX::XMFLOAT3 colour) {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[entID[entity]].colour = colour;
				}
			};
			
			inline void SetTextPos(const Entity& entity, DirectX::XMFLOAT2 pos) {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[entID[entity]].pos = pos;
				}
			};
			
			inline void SetTextOrogin(const Entity& entity, DirectX::XMFLOAT2 origin) {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[entID[entity]].origin = origin;
				}
			};
			
			inline void SetTextScale(const Entity& entity, DirectX::XMFLOAT2 scale) {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[entID[entity]].scale = scale;
				}
			};
			
			inline void SetTextEffect(const Entity& entity, DirectX::SpriteEffects effect) {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[entID[entity]].effect = effect;
				}
			};
			
			inline void SetTextRotation(const Entity& entity, float rotation) {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[entID[entity]].rotation = rotation;
				}
			};
			
			inline void SetTextLayerDepth(const Entity& entity, float layerDepth) {
				// chexk if entity exist in text 
				auto fileLoaded = entID.find(entity);
				if (fileLoaded != entID.end())
				{
					loadedTexts[entID[entity]].layerDepth = layerDepth;
				}
			};

			// sets for texture
			inline void SetTextureColour(const Entity& entity, DirectX::XMFLOAT3 colour) {
				// chexk if entity exist in texture
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[entTextureID[entity].ID].colour = colour;
				}
			};

			inline void SetTexturePos(const Entity& entity, DirectX::XMFLOAT2 pos) {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[entTextureID[entity].ID].pos = pos;
				}
			};

			inline void SetTextureOrogin(const Entity& entity, DirectX::XMFLOAT2 origin) {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[entTextureID[entity].ID].origin = origin;
				}
			};

			inline void SetTextureScale(const Entity& entity, DirectX::XMFLOAT2 scale) {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[entTextureID[entity].ID].scale = scale;
				}
			};

			inline void SetTextureEffect(const Entity& entity, DirectX::SpriteEffects effect) {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[entTextureID[entity].ID].effect = effect;
				}
			};

			inline void SetTextureRotation(const Entity& entity, float rotation) {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[entTextureID[entity].ID].rotation = rotation;
				}
			};

			inline void SetTextureLayerDepth(const Entity& entity, float layerDepth) {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[entTextureID[entity].ID].layerDepth = layerDepth;
				}
			};

			inline void SetTextureID(const Entity& entity, Utilz::GUID& guid) {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				auto guidLoaded = textureGUID.find(guid);
				if (fileLoaded != entTextureID.end() && guidLoaded != textureGUID.end())
				{
					textureGUID[entTextureID[entity].GUID].refCount--;
					entTextureID[entity].GUID = guid;
					textureInfo[entTextureID[entity].ID].textureID = textureGUID[guid].textureHandle;
					textureGUID[guid].refCount++;
				}
			};

			inline void SetTextureRect(const Entity& entity, RECT* rect) {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[entTextureID[entity].ID].rect = rect;
				}
			};


			void Shutdown();
		private:
			void GarbageCollection();
			void Destroy(size_t index);
			int LoadTexture(const Utilz::GUID& guid, void*data, size_t size);

			// Text variables
			std::unordered_map<Entity, size_t, EntityHasher> entID;
			std::vector<Graphics::TextGUI> loadedTexts;
			std::vector<Entity> ent;

			//Texture variables
			std::unordered_map<Entity, Graphics::EntBindIDGUID, EntityHasher> entTextureID;
			std::map<Utilz::GUID, Graphics::TexUsage, Utilz::GUID::Compare> textureGUID;
			std::vector<Graphics::GUITextureInfo> textureInfo;
			std::vector<Entity> textureEnt;

			std::default_random_engine generator;

			ResourceHandler::IResourceHandler* resourceHandler;
			Graphics::IRenderer* renderer;
			const EntityManager& entityManager;

			int amountOfFonts;
			bool garbage = false;
		};
	}
}
#endif