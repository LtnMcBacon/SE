#ifndef SE_CORE_MATERIAL_MANAGER_H_
#define SE_CORE_MATERIAL_MANAGER_H_
#include <Utilz\GUID.h>
#include "EntityManager.h"
#include <ResourceHandler\IResourceHandler.h>
#include <unordered_map>
#include <Graphics\IRenderer.h>
#include <map>
#include <random>
#include "RenderableManager.h"

namespace SE
{
	namespace Core
	{
		class MaterialManager
		{
		public:
			struct CreateInfo
			{
				Utilz::GUID shader;
				Utilz::GUID* shaderResourceNames;
				Utilz::GUID* textureFileNames;
				uint32_t textureCount;
			};

			MaterialManager(ResourceHandler::IResourceHandler* resourceHandler, Graphics::IRenderer* renderer, const EntityManager& entityManager, RenderableManager* renderableManager);
			~MaterialManager();
			MaterialManager(const MaterialManager& other) = delete;
			MaterialManager(const MaterialManager&& other) = delete;
			MaterialManager& operator=(const MaterialManager& other) = delete;

			/**
			* @brief	Create a texture for the entity. This is for mesh with no submeshes.
			* @param [in] entity The entity to bind the texture to.
			* @param [in] guid The guid of the texture to use.
			* @sa CreateInfo
			*/
			void Create(const Entity& entity, const CreateInfo& info, bool async = false, ResourceHandler::Behavior behavior = ResourceHandler::Behavior::QUICK);


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

			void SetRenderObjectInfo(const Entity& entity, Graphics::RenderObjectInfo* info);

			ResourceHandler::InvokeReturn LoadDefaultShader(const Utilz::GUID& guid, void*data, size_t size);
			int LoadTexture(void*data, size_t size);
			ResourceHandler::InvokeReturn LoadShader(const Utilz::GUID& guid, void*data, size_t size);
			
			struct TextureBindings
			{
				int8_t bindings[Graphics::RenderObjectInfo::maxTextureBinds];
			};
			struct TextureIndices
			{
				int8_t indices[Graphics::RenderObjectInfo::maxTextureBinds];
			};

			struct TextureData
			{
				int textureHandle;
				std::list<Entity> entities;
			};
			struct ShaderData
			{
				int shaderHandle;
				size_t refCount;
				Graphics::ShaderSettings shaderReflection;
			};
			struct MaterialData
			{
				static const size_t size = sizeof(Entity) + sizeof(TextureBindings) + sizeof(TextureIndices) + sizeof(size_t);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				TextureBindings* textureBindings;
				TextureIndices* textureIndices;
				size_t* shaderIndex;
			};


			std::vector<ShaderData> shaders;
			std::vector<TextureData> textures;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToShaderIndex;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToTextureIndex;

			ResourceHandler::IResourceHandler* resourceHandler;
			Graphics::IRenderer* renderer;
			const EntityManager& entityManager;
			RenderableManager* renderableManager;
			std::default_random_engine generator;

			MaterialData materialInfo;
			std::unordered_map<Entity, size_t, EntityHasher> entityToMaterialInfo;

			int defaultTextureHandle;
			int defaultShaderHandle;
			Graphics::ShaderSettings defaultShaderReflection;

			struct toUpdateStruct
			{
				size_t textureIndex;
				int newHandle;
			};
			Utilz::CircularFiFo<toUpdateStruct, 10> toUpdate;
		};
	}
}


#endif //SE_CORE_MATERIAL_MANAGER_H_