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
			GUIManager(const IGUIManager::InitializationInfo& initInfo);
			~GUIManager();

			/**
			* @brief Create a new 2D texture for GUI
			* @param[in] texFile The GUID to the texture file.
			* @retval 0+ Texture ID
			* @retval -1 Already loaded or currently loading.
			* @endcode
			*/
			int Create(const Entity& entity, const CreateInfo& info)override;

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

			inline void SetTexturePos(const Entity& entity, long x, long y)override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					
					textureInfo[fileLoaded->second.ID].posX = x;
					textureInfo[fileLoaded->second.ID].posY = y;
					/*if (textureInfo[fileLoaded->second.ID].anchor == true)
						textureInfo[fileLoaded->second.ID].pos = pos;
					else
						textureInfo[fileLoaded->second.ID].pos = DirectX::XMFLOAT2(textureInfo[fileLoaded->second.ID].pos.x / width, textureInfo[fileLoaded->second.ID].pos.y / height);
*/
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextureScreenAnchor(const Entity& entity, DirectX::XMFLOAT2 anchor) override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{			

					textureInfo[fileLoaded->second.ID].screenAnchor = anchor;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}
			};

			inline void SetTextureDimensions(const Entity& entity, long x, long y) override {
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					auto& ti = textureInfo[fileLoaded->second.ID];
					if (x == -1)
					{
						ti.width = textureGUID[fileLoaded->second.GUID].width;
					}
					else
					{
						ti.scale.x = x / (float)textureGUID[fileLoaded->second.GUID].width;
						ti.width = textureGUID[fileLoaded->second.GUID].width;

					}

					if (y == -1)
					{
						ti.height = textureGUID[fileLoaded->second.GUID].height;
					}
					else
					{
						ti.scale.y = y / (float)textureGUID[fileLoaded->second.GUID].height;
						ti.height = textureGUID[fileLoaded->second.GUID].height;
					}
					/*if (textureInfo[fileLoaded->second.ID].anchor == true)
						textureInfo[fileLoaded->second.ID]. = scale;*/
					/*else
						textureInfo[fileLoaded->second.ID].dimensions = DirectX::XMFLOAT2(textureInfo[fileLoaded->second.ID].scale.x / width, textureInfo[fileLoaded->second.ID].scale.y / height);
				*/	if (fileLoaded->second.show == true)
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

			inline void SetTextureScale(const Entity& entity, DirectX::XMFLOAT2 scale)override
			{
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureInfo[fileLoaded->second.ID].scale = scale;
					if (fileLoaded->second.show == true)
					{
						dirtyEnt[entity] = true;
					}
				}


			};

			inline void SetTexture(const Entity& entity, Utilz::GUID texture)override
			{
				auto texLoaded = textureGUID.find(texture);
				if (texLoaded == textureGUID.end())
				{
					textureGUID[texture].textureHandle = -1;
					auto res = initInfo.resourceHandler->LoadResource(texture, [this](auto guid, auto data, auto size) {
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
						textureGUID[texture].textureHandle = 0;
				}
				// chexk if entity exist in texture 
				auto fileLoaded = entTextureID.find(entity);
				if (fileLoaded != entTextureID.end())
				{
					textureGUID[fileLoaded->second.GUID].refCount--;
					fileLoaded->second.GUID = texture;
					textureGUID[fileLoaded->second.GUID].refCount++;
					entTextureID[entity].textureHandle = textureGUID[texture].textureHandle;
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
				originalScreenHeight = inHeight;
				originalScreenWidth = inWidth;
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
				int textureHandle;
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

			
			long originalScreenWidth = 1280;
			long originalScreenHeight = 720;
		};
	}
}
#endif