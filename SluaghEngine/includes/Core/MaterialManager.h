#ifndef SE_CORE_MATERIAL_MANAGER_H_
#define SE_CORE_MATERIAL_MANAGER_H_
#include <Utilz\GUID.h>
#include "EntityManager.h"
#include <ResourceHandler\IResourceHandler.h>
#include <unordered_map>
#include <Graphics\IRenderer.h>
#include <map>
#include <random>
namespace SE
{
	namespace Core
	{
		class MaterialManager
		{
			friend class RenderableManager;
		public:
			struct CreateInfo
			{
				Utilz::GUID shader;
				Utilz::GUID* shaderResourceNames;
				Utilz::GUID* textureFileNames;
				uint32_t textureCount;
			};

			MaterialManager(ResourceHandler::IResourceHandler* resourceHandler, Graphics::IRenderer* renderer, const EntityManager& entityManager);
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

			void SetTexture(const Entity& entity, Utilz::GUID shaderResourceName,
				Utilz::GUID textureFileName);

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

			int LoadDefaultShader(const Utilz::GUID& guid, void*data, size_t size);
			int LoadTexture(const Utilz::GUID& guid, void*data, size_t size);
			int LoadShader(const Utilz::GUID& guid, void*data, size_t size);
			static const uint32_t maxTextureBinds = 4;
			struct TextureBindings
			{
				uint8_t bindings[maxTextureBinds];
			};
			struct TextureIndices
			{
				uint8_t indices[maxTextureBinds];
			};

			struct TextureData
			{
				int textureHandle;
				size_t refCount;
			};
			struct ShaderData
			{
				int shaderHandle;
				size_t refCount;
				Graphics::ShaderSettings shaderReflection;
			};
			struct MaterialData
			{
				static const size_t size = sizeof(Entity) + sizeof(TextureBindings) + sizeof(TextureIndices) + sizeof(uint32_t);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				TextureBindings* textureBindings;
				TextureIndices* textureIndices;
				uint32_t* shaderIndex;
			};


			std::vector<ShaderData> shaders;
			std::vector<TextureData> textures;
			std::map<Utilz::GUID, int, Utilz::GUID::Compare> guidToShaderIndex;
			std::map<Utilz::GUID, int, Utilz::GUID::Compare> guidToTextureIndex;

			ResourceHandler::IResourceHandler* resourceHandler;
			Graphics::IRenderer* renderer;
			const EntityManager& entityManager;
			std::default_random_engine generator;

			MaterialData materialInfo;
			std::unordered_map<Entity, size_t, EntityHasher> entityToMaterialInfo;

			int defaultTextureHandle;
			int defaultShaderHandle;
			Graphics::ShaderSettings defaultShaderReflection;

		};
	}
}


#endif //SE_CORE_MATERIAL_MANAGER_H_