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
				Utilz::GUID materialFile;
			};

			MaterialManager(ResourceHandler::IResourceHandler* resourceHandler, Graphics::IRenderer* renderer, const EntityManager& entityManager, RenderableManager* renderableManager);
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

			struct MatInfo
			{
				uint32_t amountOfTex;
				Utilz::GUID* tex;				
				Utilz::GUID* textureChannel;
			};

			struct matDataInfo
			{
				Graphics::MaterialAttributes attrib;
				MatInfo info;
			};

			ResourceHandler::InvokeReturn LoadDefaultShader(const Utilz::GUID& guid, void*data, size_t size);
			int LoadTexture(void*data, size_t size);
			void LoadMaterialFile(void * data, size_t size, matDataInfo& dataIinfo);
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
				static const size_t size = sizeof(Entity) + sizeof(TextureBindings) + sizeof(TextureIndices) + sizeof(size_t) * 2;
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				TextureBindings* textureBindings;
				TextureIndices* textureIndices;
				size_t* shaderIndex;
				size_t* materialIndex;
			};
			

			std::vector<ShaderData> shaders;
			std::vector<TextureData> textures;
			std::vector<matDataInfo> materials;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToShaderIndex;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToTextureIndex;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToMaterialIndex;

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

			struct toUpdateMatStruct
			{
				size_t materialIndex;
				Graphics::MaterialAttributes attrib;
			};
			Utilz::CircularFiFo<toUpdateMatStruct, 10> toUpdateMat;
		};
	}
}


#endif //SE_CORE_MATERIAL_MANAGER_H_