#ifndef SE_CORE_MATERIAL_MANAGER_H_
#define SE_CORE_MATERIAL_MANAGER_H_


#include <IMaterialManager.h>

#include <unordered_map>
#include <map>
#include <random>
#include <vector>
#include <Utilz\CircularFiFo.h>

namespace SE
{
	namespace Core
	{
		class MaterialManager : public IMaterialManager
		{
		public:
			MaterialManager(const InitializationInfo& initInfo);
			~MaterialManager();
			MaterialManager(const MaterialManager& other) = delete;
			MaterialManager(const MaterialManager&& other) = delete;
			MaterialManager& operator=(const MaterialManager& other) = delete;

			/**
			* @brief	Create a texture for the entity. This is for mesh with no submeshes.
			* @param [in] entity The entity to bind the texture to.
			* @param [in] info The info used when creating the material.
			* @param [in] async If the resource should be streamed.
			* @param [in] behavior The streaming behavior.
			* @sa CreateInfo
			*/
			void Create(const Entity& entity, const CreateInfo& info, bool async = false, ResourceHandler::Behavior behavior = ResourceHandler::Behavior::QUICK)override;


			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;
		private:
			/**
			* @brief	Allocate more memory
			*/
			void Allocate(size_t size);
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index)override;
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(const Entity& entity)override;
			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection()override;

			void SetRenderObjectInfo(const Entity& entity, Graphics::RenderJob* info);

			struct MatInfo
			{
				uint32_t amountOfTex;
				Utilz::GUID bindings[Graphics::ShaderStage::maxTextures];				
				Utilz::GUID textures[Graphics::ShaderStage::maxTextures];
			};

			struct matDataInfo
			{
				Graphics::MaterialAttributes attrib;
				MatInfo info;
				std::list<Entity> entities;
			};

			void LoadTextures(matDataInfo material, const Entity& entity);
			int LoadTexture(const Utilz::GUID& guid, void*data, size_t size);
			int LoadMaterialFile(void * data, size_t size, matDataInfo& dataIinfo);

			struct TextureData
			{
				std::list<Entity> entities;
			};
			struct ShaderData
			{
				size_t refCount;
			};
			struct MaterialData
			{
				static const size_t size = sizeof(Entity) + sizeof(Utilz::GUID) + sizeof(Utilz::GUID);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				Utilz::GUID* shader;
				Utilz::GUID* material;
			};
			

			std::map<Utilz::GUID, ShaderData, Utilz::GUID::Compare> guidToShader;
			std::map<Utilz::GUID, TextureData, Utilz::GUID::Compare> guidToTexture;
			std::map<Utilz::GUID, matDataInfo, Utilz::GUID::Compare> guidToMaterial;

			InitializationInfo initInfo;
			std::default_random_engine generator;

			MaterialData materialInfo;
			std::unordered_map<Entity, size_t, EntityHasher> entityToMaterialInfo;

			Utilz::GUID defaultPixelShader;
			Utilz::GUID defaultTexture;
			Utilz::GUID defaultTextureBinding;
			Utilz::GUID defaultSampler;
			Utilz::GUID defaultMaterial;

			struct toUpdateStruct
			{
				Utilz::GUID material;
			};
			Utilz::CircularFiFo<toUpdateStruct, 10> toUpdate;
		};
	}
}


#endif //SE_CORE_MATERIAL_MANAGER_H_