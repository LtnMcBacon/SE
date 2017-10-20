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
			RenderableManager(const InitializationInfo& initInfo);
			~RenderableManager();
			RenderableManager(const RenderableManager& other) = delete;
			RenderableManager(const RenderableManager&& other) = delete;
			RenderableManager& operator=(const RenderableManager& other) = delete;
			RenderableManager& operator=(const RenderableManager&& other) = delete;

			/**
			* @brief	Bind a renderable object to and entity
			*
			* @param[in] entity The entity to bind the renderable object to.
			* @param[in] meshGUID The guid of the mesh to be used.
			*
			*/
			void CreateRenderableObject(const Entity& entity, const CreateInfo& meshGUID, bool async = false, ResourceHandler::Behavior behavior = ResourceHandler::Behavior::QUICK)override;

			/**
			* @brief	Hide/Show the renderable object
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			void ToggleRenderableObject(const Entity& entity, bool show)override;

			inline void RegisterToSetRenderObjectInfo(const SetRenderObjectInfoDelegate&& callback)override
			{
				SetRenderObjectInfoEvent += callback;
			}
			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;

			void UpdateRenderableObject(const Entity& entity)override;

			void ToggleWireframe(const Entity& entity, bool wireFrame) override;

			void ToggleTransparency(const Entity& entity, bool transparency) override;

		private:
		
			void CreateRenderObjectInfo(size_t index, Graphics::RenderObjectInfo * info);
			Utilz::Event<void(const Entity& entity, Graphics::RenderObjectInfo* info)> SetRenderObjectInfoEvent;

			
			void LinearUnload(size_t sizeToAdd);

			typedef void(RenderableManager::*UnloadingStrategy)(size_t sizeToAdd);

			UnloadingStrategy Unload;


			void SetDirty(const Entity& entity, size_t index);

			/**
			* @brief	Allocate more memory
			*/
			void Allocate(size_t size);
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index)override;
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

			int LoadModel(void* data, size_t size);
			
			void LoadResource(const Utilz::GUID& meshGUID, size_t index, bool async, ResourceHandler::Behavior behavior);
		
			struct RenderableObjectData
			{
				static const size_t size = sizeof(Entity) + sizeof(size_t) + sizeof(Graphics::RenderObjectInfo::PrimitiveTopology) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				size_t* bufferIndex;
				Graphics::RenderObjectInfo::PrimitiveTopology* topology;
				uint8_t* visible;
				uint32_t* jobID;
				uint8_t* wireframe;
				uint8_t* transparency;
			};
			InitializationInfo initInfo;
			std::default_random_engine generator;	

			struct DirtyEntityInfo
			{
				size_t transformIndex;
				Entity entity;
			};
			std::vector<DirtyEntityInfo> dirtyEntites;



			RenderableObjectData renderableObjectInfo;
			std::unordered_map<Entity, size_t, EntityHasher> entityToRenderableObjectInfoIndex;

			int defaultShader;


			enum class BufferState
			{
				Loaded,
				Loading,
				Dead
			};

			struct BufferInfo
			{
				int bufferHandle;
				BufferState state;
				size_t size;
				std::list<Entity> entities;
			};

			std::vector<BufferInfo> bufferInfo;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToBufferInfoIndex;
			std::mutex bufferLock;


			struct toUpdateStruct
			{
				size_t bufferIndex;
				int newHandle;
				size_t size;
			};
			Utilz::CircularFiFo<toUpdateStruct, 10> toUpdate;
		};
	}
}
#endif