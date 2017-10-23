#ifndef SE_INTERFACE_COLLISION_MANAGER_H_
#define SE_INTERFACE_COLLISION_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include "IManager.h"
#include <ResourceHandler\IResourceHandler.h>
#include "ITransformManager.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <Utilz\GUID.h>
#include <Utilz\Delegate.h>

namespace SE
{
	namespace Core
	{
		class  ICollisionManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				ResourceHandler::IResourceHandler* resourceHandler;
				IEntityManager* entityManager;
				ITransformManager* transformManager;
			};


			virtual ~ICollisionManager() {};

			/**
			* @brief	Create a bounding Hierarchy for the entity.
			*
			* @details	The bounding Hierarchy will be created from the points given.
			* @param[in] entity The entity to create the bounding Hierarchy for.
			* @param[in] p1 The lower front left corner.
			* @param[in] p2 The upper back right corner.
			*/
			virtual void CreateBoundingHierarchy(const Entity& entity, const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2) = 0;
			/**
			* @brief	Create a bounding Hierarchy for the entity.
			*
			* @details	The bounding Hierarchy will be created from mesh asset given.
			* @param[in] entity The entity to create the bounding Hierarchy for.
			* @param[in] mesh Guid of the mesh to create the bounding Hierarchy from.
			*/
			virtual void CreateBoundingHierarchy(const Entity& entity, const Utilz::GUID& mesh, bool async = false, ResourceHandler::Behavior behavior = ResourceHandler::Behavior::QUICK) = 0;

			/**
			* @brief	Bind a callback that will be called if the given entity collides with any other entity.
			* @param[in] entity The entity to create the bind the callback to.
			* @param[in] callback The callback.
			* @sa CollideCallbackDelegate
			*/
			virtual void BindOnCollideWithAny(const Entity& entity) = 0;

			/**
			* @brief	Check if the entity is hit by a ray.
			* @param[in] rayO The origin of the ray.
			* @param[in] rayD The direction of the ray.
			* @param[out] collidedEntity The entity that the ray collided with.
			* @param[out] distance The distance from the entity that the ray collided with to the ray origin.
			* @retval true Returns true if an entity was written to collidedEntity
			* @retval false Returns false if the ray did not collide with an entity
			*/
			virtual bool Pick(const DirectX::XMVECTOR& rayO, const DirectX::XMVECTOR& rayD, Entity& collidedEntity, float& distance) const = 0;


			virtual void SetCollideWithAnyCallback(const Utilz::Delegate<void(const Entity& hit, const Entity& hitter)>& callback) = 0;

			/*
			* @brief Gets the local bounding box for an entity.
			* @param[in] entity The entity to retrive the bounding box from
			* @param[out] bb A pointer to where to store the bounding box.
			* @retval false The entity does not have a bounding box or nullptr was passed to bb.
			* @retval true The entity has a bounding box and it was stored in bb.
			*
			*/
			virtual bool GetLocalBoundingBox(const Entity& entity, DirectX::BoundingBox* bb = nullptr) = 0;
		protected:
			ICollisionManager() {};
	
			ICollisionManager(const ICollisionManager& other) = delete;
			ICollisionManager(const ICollisionManager&& other) = delete;
			ICollisionManager& operator=(const ICollisionManager& other) = delete;
			ICollisionManager& operator=(const ICollisionManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the CollisionManger
		**/
		DECLDIR_CORE ICollisionManager* CreateCollisionManger(const ICollisionManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_COLLISION_MANAGER_H_