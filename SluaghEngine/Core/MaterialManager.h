#ifndef SE_CORE_MATERIAL_MANAGER_H_
#define SE_CORE_MATERIAL_MANAGER_H_


#include <IMaterialManager.h>

#include <unordered_map>
#include <map>
#include <random>
#include <vector>
#include <Utilz\CircularFiFo.h>
#include <Graphics\FileHeaders.h>

namespace SE
{
	namespace Core
	{
		class MaterialManager : public IMaterialManager
		{
		public:
			MaterialManager(const IMaterialManager::InitializationInfo& initInfo);
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
			void Create(const Entity& entity, const CreateInfo& info)override;


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

			struct TextureInfo
			{
				uint32_t numTextures;
				Utilz::GUID bindings[Graphics::ShaderStage::maxTextures];
				Utilz::GUID textures[Graphics::ShaderStage::maxTextures];
			};

			struct MaterialFileData
			{
				Graphics::MaterialAttributes attrib;
				TextureInfo textureInfo;
			};

			struct MaterialData
			{
				static const size_t size = sizeof(Entity) + sizeof(Utilz::GUID) + sizeof(Utilz::GUID) + sizeof(MaterialFileData*) + sizeof(uint8_t);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				Utilz::GUID* shader;
				Utilz::GUID* materialGUID;
				MaterialFileData** material;
				uint8_t* bloom;
			};

			InitializationInfo initInfo;
			std::default_random_engine generator;

			MaterialFileData* defaultMaterialInfo = nullptr;
			MaterialData materialInfo;
			std::unordered_map<Entity, size_t, EntityHasher> entityToMaterialInfo;

			std::function<ResourceHandler::LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)> shaderLoadCallback;
			std::function<void(const Utilz::GUID&, void*, size_t)> shaderDestroyCallback;

			std::function<ResourceHandler::LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)> materialLoadCallback;
			std::function<void(const Utilz::GUID&, void*, size_t)> materialDestroyCallback;

			std::function<ResourceHandler::LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)> textureLoadCallback;
			std::function<void(const Utilz::GUID&, void*, size_t)> textureDestroyCallback;

			struct toUpdateStruct
			{
				Utilz::GUID shader;
				Utilz::GUID mat;
				MaterialFileData* material;
				Entity entity;
			};
			Utilz::CircularFiFo<toUpdateStruct> toUpdate;
		};
	}
}


#endif //SE_CORE_MATERIAL_MANAGER_H_