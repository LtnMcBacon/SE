#ifndef SE_CORE_COLLISION_MANAGER_H_
#define SE_CORE_COLLISION_MANAGER_H_
#include "EntityManager.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <random>
#include <Utilz\GUID.h>
#include <Utilz\Delegate.h>
#include <ResourceHandler\IResourceHandler.h>
#include "TransformManager.h"
#include <unordered_map>
#include <map>
#include <Utilz\CircularFiFo.h>

namespace SE
{
	namespace Core
	{
		/**
		*
		* @brief This class stores bounding boxes and will update you if something collides.
		* @details To function properly the entity need a boundingbox created and a transform. And a function provided to be called on collision.
		*
		**/
		typedef Utilz::Delegate<void(const Entity& hit, const Entity& hitter)> CollideCallback;

		class CollisionManager
		{
		public:

			CollisionManager(ResourceHandler::IResourceHandler* resourceHandler, const EntityManager& entityManager, TransformManager* transformManager);
			~CollisionManager();
			CollisionManager(const CollisionManager& other) = delete;
			CollisionManager(const CollisionManager&& other) = delete;
			CollisionManager& operator=(const CollisionManager& other) = delete;

			/**
			* @brief	Create a bounding Hierarchy for the entity.
			*
			* @details	The Hierarchy will be created from the entities renderable object data. If no data is found a default boundingbox and sphere will be created of size (1,1,1)
			* @param[in] entity The entity to create the Hierarchy for.
			*/
			//void CreateBoundingHierarchy(const Entity& entity);
			/**
			* @brief	Create a bounding Hierarchy for the entity.
			*
			* @details	The bounding Hierarchy will be created from the points given.
			* @param[in] entity The entity to create the bounding Hierarchy for.
			* @param[in] p1 The lower front left corner.
			* @param[in] p2 The upper back right corner.
			*/
			void CreateBoundingHierarchy(const Entity& entity, const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2, bool async, ResourceHandler::Behavior behavior);
			/**
			* @brief	Create a bounding Hierarchy for the entity.
			*
			* @details	The bounding Hierarchy will be created from mesh asset given.
			* @param[in] entity The entity to create the bounding Hierarchy for.
			* @param[in] mesh Guid of the mesh to create the bounding Hierarchy from.
			*/
			void CreateBoundingHierarchy(const Entity& entity, const Utilz::GUID& mesh, bool async = false, ResourceHandler::Behavior behavior = ResourceHandler::Behavior::QUICK);

			/**
			* @brief	Bind a callback that will be called if the given entity collides with any other entity.
			* @param[in] entity The entity to create the bind the callback to.
			* @param[in] callback The callback.
			* @sa CollideCallbackDelegate
			*/
			void BindOnCollideWithAny(const Entity& entity);

			/**
			* @brief	Get the bounding Hierarchy of an entity
			* @param[in] entity The entity from whom to fetch the boundingbox from.
			* @param[out] boundingBox The returned boundingbox.
			*/
			//void GetBoudningHierarchy(const Entity& entity/*, DirectX::BoundingOrientedBox* boundingBox*/);

			/**
			* @brief	Check if the entity is hit by a ray.
			* @param[in] entity The entity to check with
			* @param[in] pickingRay The ray to do the picking with.
			* @param[out] distance The distance to the entity.
			*/
			bool PickEntity(const Entity & entity, const DirectX::XMVECTOR & rayO, const DirectX::XMVECTOR & rayD, float * distance);

			/**
			* @brief	Check if the entity is hit by a ray.
			* @param[in] rayO The origin of the ray.
			* @param[in] rayD The direction of the ray.
			* @param[out] collidedEntity The entity that the ray collided with.
			* @retval true Returns true if an entity was written to collidedEntity
			* @retval false Returns false if the ray did not collide with an entity
			*/
			bool Pick(const DirectX::XMVECTOR& rayO, const DirectX::XMVECTOR& rayD, Entity& collidedEntity) const;


			inline void RegisterCollideWithAnyCallback(const CollideCallback& callback)
			{
				collideWithAny = callback;
			}

			/**
			* @brief	Called each frame, to update the state.
			* @details The frame will keep the bounding objects up to date with the entities transform.
			*/
			void Frame();

			/*
			 * @brief Gets the local bounding box for an entity. 
			 * @param[in] entity The entity to retrive the bounding box from
			 * @param[out] bb A pointer to where to store the bounding box.
			 * @retval false The entity does not have a bounding box or nullptr was passed to bb.
			 * @retval true The entity has a bounding box and it was stored in bb.
			 *
			 */
			bool GetLocalBoundingBox(const Entity& entity, DirectX::BoundingBox* bb = nullptr);
		private:
			void SetDirty(const Entity& entity, size_t index);

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

			/**
			* @brief	Allocate more memory for bounding Hierarchy 
			*/
			void AllocateBH(size_t size);
			/**
			* @brief	Remove an bounding Hierarchy
			*/
			void DestroyBH(size_t index);

			struct BoundingHierarchyInfo
			{
				size_t index;
				std::list<Entity> entities;
			};
			struct BoundingHierarchy
			{
				static const size_t size = /* sizeof(DirectX::BoundingOrientedBox) + */  sizeof(DirectX::BoundingSphere) + sizeof(DirectX::BoundingBox);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				//DirectX::BoundingOrientedBox* box;
				DirectX::BoundingSphere* sphere;
				DirectX::BoundingBox* AABB;
			};
			struct CollisionData
			{
				static const size_t size = sizeof(Entity) + sizeof(size_t) + sizeof(DirectX::BoundingSphere) + sizeof(DirectX::BoundingBox) + sizeof(uint8_t);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				size_t* boundingIndex;
				DirectX::BoundingSphere* sphereWorld;
				DirectX::BoundingBox* AABBWorld;
				uint8_t* collisionWithAny;
			};

			ResourceHandler::IResourceHandler* resourceHandler;
			const EntityManager& entityManager;
			TransformManager* transformManager;

			struct DirtyEntityInfo
			{
				size_t transformIndex;
				size_t myIndex;
			};
			std::vector<DirtyEntityInfo> dirtyEntites;

			std::default_random_engine generator;
			CollisionData collisionData;
			std::unordered_map<Core::Entity, size_t, Core::EntityHasher> entityToCollisionData;

			size_t defaultHierarchy;
			BoundingHierarchy boundingHierarchy;
			std::vector<BoundingHierarchyInfo> boundingInfo;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToBoudningIndex;


			int LoadMesh(size_t newHI, void * data, size_t size);
			void CreateBoundingHierarchy(size_t index, void*data, size_t numVertices, size_t stride);

			CollideCallback collideWithAny;


			struct ToUpdate
			{
				size_t boundingHierarchyIndex;
				size_t index;
			};

			Utilz::CircularFiFo<ToUpdate> toUpdate;
		};
	}
}

#endif // SE_CORE_COLLISION_MANAGER_H_