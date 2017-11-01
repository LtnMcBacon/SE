#ifndef SE_CORE_RENDERABLE_MANAGER_H_
#define SE_CORE_RENDERABLE_MANAGER_H_

#include <IRenderableManager.h>


#include "EntityManager.h"

#include <unordered_map>
#include <random>

#include <map>

#include "TransformManager.h"
#include <list>
#include <mutex>
#include <Utilz\Event.h>
#include <Utilz\CircularFiFo.h>

#include "RenderableManagerInstancing.h"

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
		class RenderableManager : public IRenderableManager
		{
		public:
			RenderableManager(const IRenderableManager::InitializationInfo& initInfo);
			RenderableManager(const IRenderableManager::InitializationInfo& initInfo, size_t allocsize, RenderableManagerInstancing* rmI);
			virtual ~RenderableManager();
			RenderableManager(const RenderableManager& other) = delete;
			RenderableManager(const RenderableManager&& other) = delete;
			RenderableManager& operator=(const RenderableManager& other) = delete;
			RenderableManager& operator=(const RenderableManager&& other) = delete;

			/**
			* @brief	Bind a renderable object to an entity
			*
			* @param[in] entity The entity to bind the renderable object to.
			* @param[in] meshGUID The guid of the mesh to be used.
			*
			*/
			void CreateRenderableObject(const Entity& entity, const CreateInfo& info, bool async = false, ResourceHandler::Behavior behavior = ResourceHandler::Behavior::QUICK)override;

			/**
			* @brief	Hide/Show the renderable object
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			void ToggleRenderableObject(const Entity& entity, bool visible)override;

			/**
			* @brief	Called each frame, to update the state.
			*/
			virtual void Frame(Utilz::TimeCluster* timer)override;

			void ToggleWireframe(const Entity& entity, bool wireFrame) override;

			void ToggleTransparency(const Entity& entity, bool transparency) override;

			bool IsVisible(const Entity& entity)const;

			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index)override;

			void CreateRenderObjectInfo(size_t index, Graphics::RenderJob * info);

			void ToggleShadow(const Entity& entity, bool shadow) override;

		private:
			void LoadResource(const Utilz::GUID& meshGUID, size_t newEntry, bool async, ResourceHandler::Behavior behavior);
			std::function<ResourceHandler::LoadReturn(const Utilz::GUID&, void*, size_t, void**, size_t*)> loadCallback;
			std::function<void(const Utilz::GUID&, void*, size_t)> destroyCallback;

			/**
			* @brief	Allocate more memory
			*/
			void Allocate(size_t size);


			void Init();

			void UpdateRenderableObject(const Entity& entity);
			


			void CreateShadowRenderObjectInfo(size_t index, Graphics::RenderJob* info);
			
		//	void LinearUnload(size_t sizeToAdd);

			//typedef void(RenderableManager::*UnloadingStrategy)(size_t sizeToAdd);

			//UnloadingStrategy Unload;


			void SetDirty(const Entity& entity, size_t index);

			
			/**
			* @brief	Remove an enitity
			*/
			void Destroy(const Entity& entity)override;
			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection()override;

			void UpdateDirtyTransforms();

			ResourceHandler::InvokeReturn LoadDefaultShader(const Utilz::GUID& guid, void* data, size_t size);

			int LoadModel(const Utilz::GUID& meshGUID, void* data, size_t size, size_t& vertexCount);
			

			struct MeshData
			{
				Utilz::GUID mesh;
				size_t vertexCount;
			};

			struct RenderableObjectData
			{
				static const size_t size = sizeof(Entity) + sizeof(MeshData) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				MeshData* mesh;
				uint8_t* visible;
				uint8_t* wireframe;
				uint8_t* transparency;
				uint8_t* shadow;
			};
			InitializationInfo initInfo;
			std::default_random_engine generator;	

			struct DirtyEntityInfo
			{
				size_t transformIndex;
				Entity entity;
			};
			std::vector<DirtyEntityInfo> dirtyEntites;

			RenderableManagerInstancing* rmInstancing;
			RenderableManagerInstancing* shadowInstancing;

			RenderableObjectData renderableObjectInfo;
			std::unordered_map<Entity, size_t, EntityHasher> entityToRenderableObjectInfoIndex;

		
			struct toUpdateStruct
			{
				MeshData data;
				Entity entity;
			};
			Utilz::CircularFiFo<toUpdateStruct, 10> toUpdate;
		};
	}
}
#endif