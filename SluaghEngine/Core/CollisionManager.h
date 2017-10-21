#ifndef SE_CORE_COLLISION_MANAGER_H_
#define SE_CORE_COLLISION_MANAGER_H_

#include <ICollisionManager.h>
#include <random>
#include <unordered_map>
#include <list>
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
		class CollisionManager : public ICollisionManager
		{
		public:
			CollisionManager(const InitializationInfo& initInfo);
			~CollisionManager();
			CollisionManager(const CollisionManager& other) = delete;
			CollisionManager(const CollisionManager&& other) = delete;
			CollisionManager& operator=(const CollisionManager& other) = delete;

			/**
			* @brief	Create a bounding Hierarchy for the entity.
			*
			* @details	The bounding Hierarchy will be created from the points given.
			* @param[in] entity The entity to create the bounding Hierarchy for.
			* @param[in] p1 The lower front left corner.
			* @param[in] p2 The upper back right corner.
			*/
			void CreateBoundingHierarchy(const Entity& entity, const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2)override;
			/**
			* @brief	Create a bounding Hierarchy for the entity.
			*
			* @details	The bounding Hierarchy will be created from mesh asset given.
			* @param[in] entity The entity to create the bounding Hierarchy for.
			* @param[in] mesh Guid of the mesh to create the bounding Hierarchy from.
			*/
			void CreateBoundingHierarchy(const Entity& entity, const Utilz::GUID& mesh, bool async = false, ResourceHandler::Behavior behavior = ResourceHandler::Behavior::QUICK)override;

			/**
			* @brief	Bind a callback that will be called if the given entity collides with any other entity.
			* @param[in] entity The entity to create the bind the callback to.
			* @param[in] callback The callback.
			* @sa CollideCallbackDelegate
			*/
			void BindOnCollideWithAny(const Entity& entity)override;

			/**
			* @brief	Check if the entity is hit by a ray.
			* @param[in] rayO The origin of the ray.
			* @param[in] rayD The direction of the ray.
			* @param[out] collidedEntity The entity that the ray collided with.
			* @retval true Returns true if an entity was written to collidedEntity
			* @retval false Returns false if the ray did not collide with an entity
			*/
			bool Pick(const DirectX::XMVECTOR& rayO, const DirectX::XMVECTOR& rayD, Entity& collidedEntity) const override;


			inline void SetCollideWithAnyCallback(const Utilz::Delegate<void(const Entity& hit, const Entity& hitter)>& callback)override
			{
				collideWithAny = callback;
			}

			/**
			* @brief	Called each frame, to update the state.
			* @details The frame will keep the bounding objects up to date with the entities transform.
			*/
			void Frame(Utilz::TimeCluster* timer)override;

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
			void Destroy(size_t index)override;
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(const Entity& entity)override;
			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection()override;

			/**
			* @brief	Allocate more memory for bounding Hierarchy 
			*/
			void AllocateBH(size_t size);
			/**
			* @brief	Remove an bounding Hierarchy
			*/
			void DestroyBH(size_t index);


			InitializationInfo initInfo;

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

			Utilz::Delegate<void(const Entity& hit, const Entity& hitter)> collideWithAny;


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