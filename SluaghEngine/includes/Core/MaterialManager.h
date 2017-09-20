#ifndef SE_CORE_MATERIAL_MANAGER_H_
#define SE_CORE_MATERIAL_MANAGER_H_
#include <Utilz\GUID.h>
#include "EntityManager.h"
#include <unordered_map>
#include <map>
#include <random>
namespace SE
{
	namespace Core
	{
		class MaterialManager
		{
		public:
			struct CreateInfo
			{
				Utilz::GUID* textures;
				size_t textureCount;
				Utilz::GUID* shader;
				size_t shaderCount;
			};

			MaterialManager(const EntityManager& entityManager);
			~MaterialManager();
			MaterialManager(const MaterialManager& other) = delete;
			MaterialManager(const MaterialManager&& other) = delete;
			MaterialManager& operator=(const MaterialManager& other) = delete;

			/**
			* @brief	Create a texture for the entity. This is for mesh with no submeshes.
			* @param [in] entity The entity to bind the texture to.
			* @param [in] guid The guid of the texture to use.
			*/
			void Create(const Entity& entity, const CreateInfo& info);


			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame();
		private:
			/**
			* @brief	Allocate more memory
			*/
			void Allocate(size_t size);
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index);
			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection();

			int LoadTexture(const Utilz::GUID& guid, void*data, size_t size);
			int LoadShader(const Utilz::GUID& guid, void*data, size_t size);

			struct TextureData
			{
				int textureHandle;
				size_t refCount;
			};
			struct ShaderData
			{
				int shaderHandle;
				size_t refCount;
			};
			struct MaterialData
			{
				static const size_t size = sizeof(Entity) + sizeof(size_t) + sizeof(size_t);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				size_t* textureIndex;
				size_t* shaderIndex;
			};

			const EntityManager& entityManager;
			std::default_random_engine generator;

			MaterialData materialInfo;
			std::unordered_map<Entity, size_t, EntityHasher> entityToMaterialInfo;

			int defaultTextureHandle;
			int defaultShaderHandle;
			std::vector<TextureData> textureInfo;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToTextureInfo;

			std::vector<ShaderData> shaderInfo;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToShaderInfo;
		};
	}
}


#endif //SE_CORE_MATERIAL_MANAGER_H_