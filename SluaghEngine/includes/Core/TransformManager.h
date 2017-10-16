#ifndef SE_CORE_TRANSFORMMANAGER_H_
#define SE_CORE_TRANSFORMMANAGER_H_
#include <DirectXMath.h>
#include <unordered_map>
#include "EntityManager.h"
#include <Utilz\Event.h>
#include <mutex>

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
		class TransformManager
		{
			friend class RenderableManager;
			friend class CameraManager;
			friend class DebugRenderManager;
			friend class LightManager;
		public:
			TransformManager(EntityManager* em);
			~TransformManager();
			TransformManager(const TransformManager& other) = delete;
			TransformManager(const TransformManager&& other) = delete;
			TransformManager& operator=(const TransformManager& other) = delete;

			/**
			* @brief    Creates a transform component for an entity. Must be called before any other methods for a given entity. There is no destroy method, this is handled internally.
			* @param[in] e The entity to bind a transform component to.
			* @param[in] pos The worldspace position.
			* @param[in] rotation The  pitch, yaw and roll of the entity as the x, y and z component of the vec3.
			* @param[in] scale The scale.
			* @warning MUST BE CALLED BEFORE ANY OTHER METHOD FOR A GIVEN ENTITY.
			*/
			void Create(const Entity& e, const DirectX::XMFLOAT3& pos = { 0.0f,0.0f,0.0f }, const DirectX::XMFLOAT3& rotation = { 0.0f,0.0f,0.0f }, const DirectX::XMFLOAT3& scale = { 1.0f, 1.0f, 1.0f });

			/**
			* @brief    Bind an entity as a child to another entity. This will update the transform of the child when the parent change.
			* @param[in] parent The parent entity
			* @param[in] child The child entity.
			* @param[in] rotation Wether or not rotation should be inherited.
			* @warning A parent can only have one child. An entity with a child can not have a parent. An entity with a parent can not have a child.
			*/
			void BindChild(const Entity& parent, const Entity& child, bool rotation = true);



			/**
			* @brief    Sets the entity as dirty.
			* @param[in] e The entity to move
			* @warning Create must be called before this method for a given entity.
			*/
			void SetAsDirty(const Entity& e);

			/**
			* @brief    Moves an entity along a vector.
			* @param[in] e The entity to move
			* @param[in] dir The direction to move the entity along. For example {0,5,0} will move the entity 5 units along the y-axis.
			* @warning Create must be called before this method for a given entity.
			*/
			void Move(const Entity& e, const DirectX::XMFLOAT3& dir);

			/**
			* @brief    Moves an entity along a vector.
			* @param[in] e The entity to move
			* @param[in] dir The direction to move the entity along. For example {0,5,0} will move the entity 5 units along the y-axis.
			* @warning Create must be called before this method for a given entity.
			*/
			void Move(const Entity& e, const DirectX::XMVECTOR& dir);
			/**
			* @brief    Relative rotation. Adds roll, pitch, and yaw to the current rotation. Uses radians and not degrees.
			* @param[in] e The entity to rotate
			* @param[in] pitch The pitch in radians. (x-axis)
			* @param[in] yaw The yaw in radians. (y-axis)
			* @param[in] roll The roll in radians. (z-axis)
			* @warning Create must be called before this method for a given entity.
			*/
			void Rotate(const Entity& e, float pitch, float yaw, float roll);
			/**
			* @brief    Relative scaling. For example, if the scale is 2 and Scale(entity, 4) is called, the entity will be scaled to 8.
			* @param[in] e The entity to scale
			* @param[in] scale The value to multiply the scale with.
			* @warning Create must be called before this method for a given entity.
			*/
			void Scale(const Entity& e, float scale);
			/**
			* @brief    Relative scaling. For example, if the scale is 2 and Scale(entity, 4) is called, the entity will be scaled to 8.
			* @param[in] e The entity to scale
			* @param[in] scale The value to multiply the scale with.
			* @warning Create must be called before this method for a given entity.
			*/
			void Scale(const Entity& e, const DirectX::XMFLOAT3& scale);
			/**
			* @brief    Sets the position in world space for a given entity.
			* @param[in] e The entity to scale
			* @param[in] pos The position.
			* @warning Create must be called before this method for a given entity.
			*/
			void SetPosition(const Entity& e, const DirectX::XMFLOAT3& pos);
			/**
			* @brief    Absolute rotation. Sets the rotation to the roll, pitch and yaw supplied in the parameter list.
			* @param[in] e The entity to rotate
			* @param[in] pitch The pitch in radians. (x-axis)
			* @param[in] yaw The yaw in radians. (y-axis)
			* @param[in] roll The roll in radians. (z-axis)
			* @warning Create must be called before this method for a given entity.
			*/
			void SetRotation(const Entity& e, float pitch, float yaw, float roll);
			/**
			* @brief    Sets the scale of an entity.
			* @param[in] e The entity to scale
			* @param[in] scale The scale.
			* @warning Create must be called before this method for a given entity.
			*/
			void SetScale(const Entity& e, float scale);
			/**
			* @brief    Sets the scale of an entity.
			* @param[in] e The entity to scale
			* @param[in] scale The scale.
			* @warning Create must be called before this method for a given entity.
			*/
			void SetScale(const Entity& e, const DirectX::XMFLOAT3& scale);
			/**
			* @brief    Returns the world position of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity.
			*/
			const DirectX::XMFLOAT3& GetPosition(const Entity& e) const;
			/**
			* @brief    Returns the roll (x-component of vector), pitch (y-component of vector) and yaw (z-component of vector) of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity.
			*/
			const DirectX::XMFLOAT3& GetRotation(const Entity& e) const;
			/**
			* @brief    Returns the scale of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity.
			*/
			const DirectX::XMFLOAT3& GetScale(const Entity& e) const;

			/**
			* @brief    Returns the transform of an entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive use sparingly.
			*/
			const DirectX::XMFLOAT4X4 GetTransform(const Entity& e) const;

			/**
			* @brief    Returns the forward vector of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive use sparingly.
			*/
			const DirectX::XMFLOAT3 GetForward(const Entity& e)const;

			/**
			* @brief    Sets the forward vector of the entity.
			* @param[in] e The entity.
			* @param[in] forward The forward vector to set.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive; use sparingly.
			*/
			const void SetForward(const Entity& e, const DirectX::XMFLOAT3& forward);

			/**
			* @brief    Sets the forward vector of the entity.
			* @param[in] e The entity.
			* @param[in] forward The forward vector to set.
			* @warning Create must be called before this method for a given entity. Also this operation is very expensive; use sparingly.
			*/
			const void SetForward(const Entity& e, const DirectX::XMVECTOR& forward);

			/**
			* @brief     Is called once per frame by the engine. Do not call this from outside the engine as it is unnecessary.
			* @details removes up to 4 dead entities per call. Looks at a subset of its registered entities and asks the entity manager if they are dead. If they are, they are removed.
			*/
			int GarbageCollection();

			/**
			* @brief     Returns the number of registered entities, aka the number of transform components in the system.
			*/
			uint32_t ActiveTransforms() const;

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame();

			inline void RegisterSetDirty(const Utilz::Delegate<void(const Entity& entity, size_t index)>& callback)
			{
				SetDirty += callback;
			}
		
			inline const DirectX::XMFLOAT4X4* GetCleanedTransforms()const
			{
				return &dirtyTransforms[0];
			}
		private:
			void SetAsDirty(size_t index);

			void UpdateTransform(size_t index);

			std::unordered_map<Entity, uint32_t, EntityHasher> entityToIndex;
			struct PD
			{
				size_t Index;
				size_t parentIndex;
			};
			std::vector<PD> parentDeferred;
			std::vector<DirectX::XMFLOAT4X4> dirtyTransforms;

			Entity* entities;
			DirectX::XMFLOAT3* positions;
			DirectX::XMFLOAT3* rotations;
			DirectX::XMFLOAT3* scalings;
			uint8_t* dirty;
			size_t *Parent; // Parent transform (whose reference system we are relative to).
			size_t *Child;
			size_t *DirtyTransform; // Use this to get other children
			uint8_t* inheritRotation;

			uint32_t transformCount;
			uint32_t transformCapacity;
			uint32_t garbageCollectionIndex;
			static const size_t transformCapacityIncrement = 512;
			static const size_t sizePerEntity = sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(uint8_t) + sizeof(Entity) + sizeof(size_t)*2 + sizeof(uint8_t);

			enum TransformFlags : uint16_t
			{
				DIRTY = 1 << 0,
				INHERIT_TRANSLATION = 1 << 1,
				INHERIT_SCALE = 1 << 2,
				INHERIT_ROTATION = 1 << 3,
				INHERIT_ALL = 7U
			};


			struct TransformData
			{
				static const size_t size = sizeof(Entity) + sizeof(DirectX::XMFLOAT3) * 3U + sizeof(int32_t) * 3 + sizeof(int16_t);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entities = nullptr;
				DirectX::XMFLOAT3* positions = nullptr;
				DirectX::XMFLOAT3* rotations = nullptr;
				DirectX::XMFLOAT3* scalings = nullptr;
				int32_t* childIndex = nullptr;
				int32_t* siblingIndex = nullptr;
				int32_t* parentIndex = nullptr;
				int16_t* flags = nullptr;
			};
			
			TransformData data;
			int32_t* lookUpTable;
			size_t lookUpTableSize;
			void Allocate(size_t count);
			void Destroy(const Entity& entity);
			
			EntityManager* entityManager; /**<The transform manager needs a reference to the entity manager in order to find which entities have been destroyed and can be removed.*/

			Utilz::Event<void(const Entity& entity, size_t index)> SetDirty;

		
			
			std::vector<Entity> entityStack;
			std::mutex queueLock;
		};
	}
}


#endif //SE_CORE_TRANSFORMMANAGER_H_
