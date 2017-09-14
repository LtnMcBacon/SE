#ifndef SE_CORE_TRANSFORMMANAGER_H_
#define SE_CORE_TRANSFORMMANAGER_H_
#include <DirectXMath.h>
#include <unordered_map>
#include "EntityManager.h"

namespace SE
{
	namespace Core
	{
		class TransformManager
		{
		public:
			TransformManager(EntityManager* em);
			~TransformManager();
			TransformManager(const TransformManager& other) = delete;
			TransformManager(const TransformManager&& other) = delete;
			TransformManager& operator=(const TransformManager& other) = delete;

			void Create(const Entity& e, const DirectX::XMFLOAT3& pos = { 0.0f,0.0f,0.0f }, const DirectX::XMFLOAT3& rotation = { 0.0f,0.0f,0.0f }, float scale = 1.0f);
			void Move(const Entity& e, const DirectX::XMFLOAT3& dir);
			void Rotate(const Entity& e, float roll, float pitch, float yaw);
			void Scale(const Entity& e, float scale);
			void SetPosition(const Entity& e, const DirectX::XMFLOAT3& pos);
			void SetRotation(const Entity& e, float roll, float pitch, float yaw);
			void SetScale(const Entity& e, float scale);
			DirectX::XMFLOAT3 GetPosition(const Entity& e) const;
			DirectX::XMFLOAT3 GetRotation(const Entity& e) const;
			float GetScale(const Entity& e) const;

			
			int GarbageCollection();

			uint32_t ActiveTransforms() const;

		private:
			std::unordered_map<Entity, uint32_t, EntityHasher> entityToIndex;
			std::unordered_map<uint32_t, Entity> indexToEntity;
			
			
			DirectX::XMFLOAT3* positions;
			DirectX::XMFLOAT3* rotations;
			float* scalings;

			uint32_t transformCount;
			uint32_t transformCapacity;
			uint32_t garbageCollectionIndex;
			static const size_t transformCapacityIncrement = 512;
			static const size_t sizePerEntity = sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT3) + sizeof(float);

			EntityManager* entityManager; /**<The transform manager needs a reference to the entity manager in order to find which entities have been destroyed and can be removed.*/
			
			void ExpandTransforms();
			
		};
	}
}


#endif //SE_CORE_TRANSFORMMANAGER_H_
