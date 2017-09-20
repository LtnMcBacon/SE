#ifndef SE_CORE_RENDERABLE_MANAGER_H_
#define SE_CORE_RENDERABLE_MANAGER_H_
#include "EntityManager.h"
#include <Utilz\GUID.h>
#include <unordered_map>
#include <random>
#include <Graphics\IRenderer.h>
#include <map>
#include <ResourceHandler\IResourceHandler.h>

namespace SE
{
	namespace Core
	{
		/**
		*
		* @brief This class is used to bind renderable objects to an entity.
		*
		* @details Use this class when you want an entity to be rendered as a object in the world.
		*
		**/
		class RenderableManager
		{
		public:
			RenderableManager(const EntityManager& entityManager);
			~RenderableManager();
			RenderableManager(const RenderableManager& other) = delete;
			RenderableManager(const RenderableManager&& other) = delete;
			RenderableManager& operator=(const RenderableManager& other) = delete;

			/**
			* @brief	Bind a renderable object to and entity
			*
			* @param[in] entity The entity to bind the renderable object to.
			* @param[in] meshGUID The guid of the mesh to be used. 
			*
			*/
			void CreateRenderableObject(const Entity& entity,const Utilz::GUID& meshGUID);

			/**
			* @brief	Hide/Show the renderable object
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			void ToggleRenderableObject(const Entity& entity, bool show);
			/**
			* @brief	Hide the renderable object
			*/
			void HideRenderableObject(const Entity& entity);

			/**
			* @brief	Show the renderable object
			*/
			void ShowRenderableObject(const Entity& entity);

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

			void UpdateDirtyTransforms();


			int LoadModel(const Utilz::GUID& guid, void* data, size_t size);
			void SetDirty(const Entity& entity, size_t index);
			struct DirtyEntityInfo
			{
				size_t transformIndex;
				size_t renderableIndex;
			};
			std::vector<DirtyEntityInfo> dirtyEntites;

			struct RenderableObjectData
			{
				static const size_t size = sizeof(Entity) + sizeof(size_t) + sizeof(int);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				size_t* bufferIndex;
				int* transformHandle;
			};
			const EntityManager& entityManager;
			std::default_random_engine generator;	
			RenderableObjectData renderableObjectInfo;
			std::unordered_map<Entity, size_t, EntityHasher> entityToRenderableObjectInfoIndex;

			int defaultMeshHandle;

			struct BufferInfo
			{
				int bufferHandle;
				uint32_t refCount;		
			};

			std::vector<BufferInfo> bufferInfo;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToBufferInfoIndex;
		};
	}
}
#endif