#pragma once
#ifndef SE_CORE_GUI_MANAGER_H_
#define SE_CORE_GUI_MANAGER_H_

#include <IGUIManager.h>
#include <unordered_map>
#include <stack>
#include <vector>
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
		class GUIManager : public IGUIManager
		{
		public:
			GUIManager(const InitializationInfo& initInfo);
			~GUIManager();

			/**
			* @brief Create a new 2D texture for GUI
			* @param[in] texFile The GUID to the texture file.
			* @retval 0+ Texture ID
			* @retval -1 Already loaded or currently loading.
			* @endcode
			*/
			int Create(CreateInfo info)override;

			/**
			* @brief	Hide/Show the renderable texture
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			void ToggleRenderableTexture(const Entity& entity, bool show)override;

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;

			// sets for texture
			inline void SetTextureColour(const Entity& entity, DirectX::XMFLOAT4 colour)override {
				// chexk if entity exist in texture
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[fileLoaded->second.ID].colour = colour;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTexturePos(const Entity& entity, DirectX::XMFLOAT2 pos)override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					
					if (textureInfo[fileLoaded->second.ID].anchor == true)
						textureInfo[fileLoaded->second.ID].pos = pos;
					else
						textureInfo[fileLoaded->second.ID].pos = DirectX::XMFLOAT2(textureInfo[fileLoaded->second.ID].pos.x / width, textureInfo[fileLoaded->second.ID].pos.y / height);

					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextureOrogin(const Entity& entity, DirectX::XMFLOAT2 origin) override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{			
					if (textureInfo[fileLoaded->second.ID].anchor == true)
						textureInfo[fileLoaded->second.ID].origin = origin;
					else
						textureInfo[fileLoaded->second.ID].origin = DirectX::XMFLOAT2(textureGUID[fileLoaded->second.GUID].width / 2, textureGUID[fileLoaded->second.GUID].height / 2);
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextureScale(const Entity& entity, DirectX::XMFLOAT2 scale) override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					if (textureInfo[fileLoaded->second.ID].anchor == true)
						textureInfo[fileLoaded->second.ID].scale = scale;
					else
						textureInfo[fileLoaded->second.ID].scale = DirectX::XMFLOAT2(textureInfo[fileLoaded->second.ID].scale.x / width, textureInfo[fileLoaded->second.ID].scale.y / height);
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextureEffect(const Entity& entity, Graphics::Effect effect)override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[fileLoaded->second.ID].effect = effect;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextureRotation(const Entity& entity, float rotation)override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[fileLoaded->second.ID].rotation = rotation;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}	
				}
			};

			inline void SetTextureLayerDepth(const Entity& entity, float layerDepth)override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[fileLoaded->second.ID].layerDepth = layerDepth;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
				
			};

			inline void SetTextureID(const Entity& entity, Utilz::GUID& guid) override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				auto guidLoaded = textureGUID.find(guid);
				if (fileLoaded != entTextureID.end() && guidLoaded != textureGUID.end())
				{
					textureGUID[fileLoaded->second.GUID].refCount--;
					fileLoaded->second.GUID = guid;
					textureInfo[fileLoaded->second.ID].textureID = guidLoaded->second.textureHandle;
					guidLoaded->second.refCount++;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextureRect(const Entity& entity, Graphics::RECT& rect)override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					*textureInfo[fileLoaded->second.ID].rect = rect;
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
			void updateGUI()override;

		private:
			void GarbageCollection()override;
			void Destroy(size_t index)override;
			void Destroy(const Entity& entity)override;

			struct EntBindIDGUID
			{
				Utilz::GUID GUID;
				size_t ID;
				size_t jobID;
				bool show = false;
			};

			//Texture variables
			std::unordered_map<Entity, EntBindIDGUID, EntityHasher> entTextureID;
			std::map<Utilz::GUID, Graphics::TexUsage, Utilz::GUID::Compare> textureGUID;		
			std::map<size_t, Entity> jobToEnt;
			std::vector<Graphics::GUITextureInfo> textureInfo;
			std::vector<Entity> textureEnt;
			std::unordered_map<Entity, bool, EntityHasher> dirtyEnt;

			std::default_random_engine generator;

			InitializationInfo initInfo;

			
			
			float height = 720.0;
			float width = 1280.0;
		};
	}
}
#endif