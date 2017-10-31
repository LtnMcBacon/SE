#ifndef SE_CORE_TRANSFORMMANAGER_H_
#define SE_CORE_TRANSFORMMANAGER_H_

#include <ITransformManager.h>
#include <mutex>
#include <random>

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
		class TransformManager : public ITransformManager
		{
		public:
			TransformManager(const InitializationInfo& initInfo);
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
			void Create(const Entity& e, const DirectX::XMFLOAT3& pos = { 0.0f,0.0f,0.0f }, const DirectX::XMFLOAT3& rotation = { 0.0f,0.0f,0.0f }, const DirectX::XMFLOAT3& scale = { 1.0f, 1.0f, 1.0f })override;

			/**
			* @brief    Bind an entity as a child to another entity. This will update the transform of the child when the parent change.
			* @param[in] parent The parent entity
			* @param[in] child The child entity.
			* @param[in] rotation Wether or not rotation should be inherited.
			* @param[in] translateToParent Whether or not the child should move towards the parent upon binding.
			*/
			void BindChild(const Entity& parent, const Entity& child, bool rotation = true, bool translateToParent = false)override;



			/**
			* @brief    Sets the entity as dirty.
			* @param[in] e The entity to move
			* @warning Create must be called before this method for a given entity.
			*/
			void SetAsDirty(const Entity& e)override;

			/**
			* @brief    Moves an entity along a vector.
			* @param[in] e The entity to move
			* @param[in] dir The direction to move the entity along. For example {0,5,0} will move the entity 5 units along the y-axis.
			* @warning Create must be called before this method for a given entity.
			*/
			void Move(const Entity& e, const DirectX::XMFLOAT3& dir)override;

			/**
			* @brief    Moves an entity along a vector.
			* @param[in] e The entity to move
			* @param[in] dir The direction to move the entity along. For example {0,5,0} will move the entity 5 units along the y-axis.
			* @warning Create must be called before this method for a given entity.
			*/
			void Move(const Entity& e, const DirectX::XMVECTOR& dir)override;
			/**
			* @brief    Relative rotation. Adds roll, pitch, and yaw to the current rotation. Uses radians and not degrees.
			* @param[in] e The entity to rotate
			* @param[in] pitch The pitch in radians. (x-axis)
			* @param[in] yaw The yaw in radians. (y-axis)
			* @param[in] roll The roll in radians. (z-axis)
			* @warning Create must be called before this method for a given entity.
			*/
			void Rotate(const Entity& e, float pitch, float yaw, float roll)override;
			/**
			* @brief    Relative scaling. For example, if the scale is 2 and Scale(entity, 4) is called, the entity will be scaled to 8.
			* @param[in] e The entity to scale
			* @param[in] scale The value to multiply the scale with.
			* @warning Create must be called before this method for a given entity.
			*/
			void Scale(const Entity& e, float scale)override;
			/**
			* @brief    Relative scaling. For example, if the scale is 2 and Scale(entity, 4) is called, the entity will be scaled to 8.
			* @param[in] e The entity to scale
			* @param[in] scale The value to multiply the scale with.
			* @warning Create must be called before this method for a given entity.
			*/
			void Scale(const Entity& e, const DirectX::XMFLOAT3& scale)override;
			/**
			* @brief    Sets the position in world space for a given entity.
			* @param[in] e The entity to scale
			* @param[in] pos The position.
			* @warning Create must be called before this method for a given entity.
			*/
			void SetPosition(const Entity& e, const DirectX::XMFLOAT3& pos)override;
			/**
			* @brief    Absolute rotation. Sets the rotation to the roll, pitch and yaw supplied in the parameter list.
			* @param[in] e The entity to rotate
			* @param[in] pitch The pitch in radians. (x-axis)
			* @param[in] yaw The yaw in radians. (y-axis)
			* @param[in] roll The roll in radians. (z-axis)
			* @warning Create must be called before this method for a given entity.
			*/
			void SetRotation(const Entity& e, float pitch, float yaw, float roll)override;
			/**
			* @brief    Sets the scale of an entity.
			* @param[in] e The entity to scale
			* @param[in] scale The scale.
			* @warning Create must be called before this method for a given entity.
			*/
			void SetScale(const Entity& e, float scale)override;
			/**
			* @brief    Sets the scale of an entity.
			* @param[in] e The entity to scale
			* @param[in] scale The scale.
			* @warning Create must be called before this method for a given entity.
			*/
			void SetScale(const Entity& e, const DirectX::XMFLOAT3& scale)override;
			/**
			* @brief    Returns the world position of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity.
			*/
			const DirectX::XMFLOAT3& GetPosition(const Entity& e) const override;
			/**
			* @brief    Returns the roll (x-component of vector), pitch (y-component of vector) and yaw (z-component of vector) of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity.
			*/
			const DirectX::XMFLOAT3& GetRotation(const Entity& e) const override;
			/**
			* @brief    Returns the scale of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity.
			*/
			const DirectX::XMFLOAT3& GetScale(const Entity& e) const override;

			/**
			* @brief    Returns the transform of an entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive use sparingly.
			*/
			const DirectX::XMFLOAT4X4 GetTransform(const Entity& e) const override;

			/**
			* @brief    Returns the forward vector of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive use sparingly.
			*/
			const DirectX::XMFLOAT3 GetForward(const Entity& e)const override;

			/**
			* @brief    Returns the rightward pointing vector of the entity.
			* @param[in] e The entity.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive use sparingly.
			*/
			DirectX::XMFLOAT3 GetRight(const Entity& e) const override;

			/**
			* @brief    Sets the forward vector of the entity.
			* @param[in] e The entity.
			* @param[in] forward The forward vector to set.
			* @warning Create must be called before this method for a given entity. Also this operation is expensive; use sparingly.
			*/
			const void SetForward(const Entity& e, const DirectX::XMFLOAT3& forward) override;

			/**
			* @brief    Sets the forward vector of the entity.
			* @param[in] e The entity.
			* @param[in] forward The forward vector to set.
			* @warning Create must be called before this method for a given entity. Also this operation is very expensive; use sparingly.
			*/
			const void SetForward(const Entity& e, const DirectX::XMVECTOR& forward) override;

			/**
			* @brief     Returns the number of registered entities, aka the number of transform components in the system.
			*/
			uint32_t ActiveTransforms() const override;

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;

			inline void RegisterSetDirty(const Utilz::Delegate<void(const Entity& entity, size_t index)>& callback) override
			{
				SetDirty += callback;
			}
		
			inline const DirectX::XMFLOAT4X4* GetCleanedTransforms()const override
			{
				return dirtyTransforms.data();
			}
		private:


			/**
			* @brief     Is called once per frame by the engine. Do not call this from outside the engine as it is unnecessary.
			* @details removes up to 4 dead entities per call. Looks at a subset of its registered entities and asks the entity manager if they are dead. If they are, they are removed.
			*/
			void GarbageCollection()override;
			void Allocate(size_t count);
			void Destroy(const size_t index)override;
			void Destroy(const Entity& e)override;

			void UpdateTransform(size_t index);

			std::vector<DirectX::XMFLOAT4X4> dirtyTransforms;

			enum TransformFlags : uint16_t
			{
				INHERIT_TRANSLATION = 1 << 0,
				INHERIT_SCALE = 1 << 1,
				INHERIT_ROTATION = 1 << 2,
				INHERIT_ALL = 7U,
				DIRTY = 1 << 3
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
	
			std::default_random_engine generator;
			InitializationInfo initInfo;
			Utilz::Event<void(const Entity& entity, size_t index)> SetDirty;

		
			
			std::vector<Entity> entityStack;
			std::mutex queueLock;
		};
	}
}


#endif //SE_CORE_TRANSFORMMANAGER_H_
