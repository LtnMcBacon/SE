#ifndef SE_INTERFACE_TRANSFORM_MANAGER_H_
#define SE_INTERFACE_TRANSFORM_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include "IManager.h"
#include "IEntityManager.h"
#include <DirectXMath.h>

namespace SE
{
	namespace Core
	{
		/**
		*
		* @brief The transform manager is responsible for handling all transforms for entities. Create must be called for an entity before any other method that accepts entities are called for that entity.
		*
		* @details Keeps a pointer to the entity manager for cleaning out transforms from entities that have been destroyed in the entity manager. A garbage collection method is used and should be called once per frame.
		*
		*
		* @sa EntityManager
		*
		**/
		class ITransformManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				IEntityManager* entityManager;
			};

			virtual ~ITransformManager() {};

			/**
			* @brief    Creates a transform component for an entity. Must be called before any other methods for a given entity. There is no destroy method, this is handled internally.
			* @param[in] e The entity to bind a transform component to.
			* @param[in] pos The worldspace position.
			* @param[in] rotation The  pitch, yaw and roll of the entity as the x, y and z component of the vec3.
			* @param[in] scale The scale.
			* @warning MUST BE CALLED BEFORE ANY OTHER METHOD FOR A GIVEN ENTITY.
			*/
			virtual void Create(const Entity& e, const DirectX::XMFLOAT3& pos = { 0.0f,0.0f,0.0f }, const DirectX::XMFLOAT3& rotation = { 0.0f,0.0f,0.0f }, const DirectX::XMFLOAT3& scale = { 1.0f, 1.0f, 1.0f }) = 0;

			/**
			* @brief    Bind an entity as a child to another entity. This will update the transform of the child when the parent change.
			* @param[in] parent The parent entity
			* @param[in] child The child entity.
			* @param[in] rotation Wether or not rotation should be inherited.
			* @param[in] translateToParent Whether or not the child should move towards the parent upon binding.
			*/
			virtual void BindChild(const Entity& parent, const Entity& child, bool rotation = true, bool translateToParent = false) = 0;



			/**
			* @brief    Sets the entity as dirty.
			* @param[in] e The entity to move
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void SetAsDirty(const Entity& e) = 0;

			/**
			* @brief    Moves an entity along a vector.
			* @param[in] e The entity to move
			* @param[in] dir The direction to move the entity along. For example {0,5,0} will move the entity 5 units along the y-axis.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void Move(const Entity& e, const DirectX::XMFLOAT3& dir) = 0;

			/**
			* @brief    Moves an entity along a vector.
			* @param[in] e The entity to move
			* @param[in] dir The direction to move the entity along. For example {0,5,0} will move the entity 5 units along the y-axis.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void Move(const Entity& e, const DirectX::XMVECTOR& dir) = 0;
			/**
			* @brief    Relative rotation. Adds roll, pitch, and yaw to the current rotation. Uses radians and not degrees.
			* @param[in] e The entity to rotate
			* @param[in] pitch The pitch in radians. (x-axis)
			* @param[in] yaw The yaw in radians. (y-axis)
			* @param[in] roll The roll in radians. (z-axis)
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void Rotate(const Entity& e, float pitch, float yaw, float roll) = 0;
			/**
			* @brief    Relative scaling. For example, if the scale is 2 and Scale(entity, 4) is called, the entity will be scaled to 8.
			* @param[in] e The entity to scale
			* @param[in] scale The value to multiply the scale with.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void Scale(const Entity& e, float scale) = 0;
			/**
			* @brief    Relative scaling. For example, if the scale is 2 and Scale(entity, 4) is called, the entity will be scaled to 8.
			* @param[in] e The entity to scale
			* @param[in] scale The value to multiply the scale with.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void Scale(const Entity& e, const DirectX::XMFLOAT3& scale) = 0;
			/**
			* @brief    Sets the position in world space for a given entity.
			* @param[in] e The entity to scale
			* @param[in] pos The position.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void SetPosition(const Entity& e, const DirectX::XMFLOAT3& pos) = 0;
			/**
			* @brief    Absolute rotation. Sets the rotation to the roll, pitch and yaw supplied in the parameter list.
			* @param[in] e The entity to rotate
			* @param[in] pitch The pitch in radians. (x-axis)
			* @param[in] yaw The yaw in radians. (y-axis)
			* @param[in] roll The roll in radians. (z-axis)
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void SetRotation(const Entity& e, float pitch, float yaw, float roll) = 0;
			/**
			* @brief    Sets the scale of an entity.
			* @param[in] e The entity to scale
			* @param[in] scale The scale.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void SetScale(const Entity& e, float scale) = 0;
			/**
			* @brief    Sets the scale of an entity.
			* @param[in] e The entity to scale
			* @param[in] scale The scale.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual void SetScale(const Entity& e, const DirectX::XMFLOAT3& scale) = 0;
			/**
			* @brief    Returns the world position of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual const DirectX::XMFLOAT3& GetPosition(const Entity& e) const = 0;
			/**
			* @brief    Returns the roll (x-component of vector), pitch (y-component of vector) and yaw (z-component of vector) of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual const DirectX::XMFLOAT3& GetRotation(const Entity& e) const = 0;
			/**
			* @brief    Returns the scale of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity.
			*/
			virtual const DirectX::XMFLOAT3& GetScale(const Entity& e) const = 0;

			/**
			* @brief    Returns the transform of an entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive use sparingly.
			*/
			virtual const DirectX::XMFLOAT4X4 GetTransform(const Entity& e) const = 0;

			/**
			* @brief    Returns the forward vector of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive use sparingly.
			*/
			virtual const DirectX::XMFLOAT3 GetForward(const Entity& e)const = 0;

			/**
			* @brief    Returns the rightward pointing vector of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive use sparingly.
			*/
			virtual DirectX::XMFLOAT3 GetRight(const Entity& e) const = 0;

			/**
			* @brief    Sets the forward vector of the entity.
			* @param[in] e The entity.
			* @param[in] forward The forward vector to set.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive; use sparingly.
			*/
			virtual const void SetForward(const Entity& e, const DirectX::XMFLOAT3& forward) = 0;

			/**
			* @brief    Sets the forward vector of the entity.
			* @param[in] e The entity.
			* @param[in] forward The forward vector to set.
			* @warning Create must be called before this method for a given entity. Also this operation is very expensive; use sparingly.
			*/
			virtual const void SetForward(const Entity& e, const DirectX::XMVECTOR& forward) = 0;

			/**
			* @brief     Is called once per frame by the engine. Do not call this from outside the engine as it is unnecessary.
			* @details removes up to 4 dead entities per call. Looks at a subset of its registered entities and asks the entity manager if they are dead. If they are, they are removed.
			*/
		//	virtual int GarbageCollection();

			/**
			* @brief     Returns the number of registered entities, aka the number of transform components in the system.
			*/
			virtual uint32_t ActiveTransforms() const = 0;

			virtual  void RegisterSetDirty(const Utilz::Delegate<void(const Entity& entity, size_t index)>& callback) = 0;

			virtual inline const DirectX::XMFLOAT4X4* GetCleanedTransforms()const = 0;
		protected:
			ITransformManager() {};
	
			ITransformManager(const ITransformManager& other) = delete;
			ITransformManager(const ITransformManager&& other) = delete;
			ITransformManager& operator=(const ITransformManager& other) = delete;
			ITransformManager& operator=(const ITransformManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the TransformManager
		**/
		DECLDIR_CORE ITransformManager* CreateTransformManager(const ITransformManager::InitializationInfo&);
	}
}

#endif //SE_INTERFACE_TRANSFORM_MANAGER_H_