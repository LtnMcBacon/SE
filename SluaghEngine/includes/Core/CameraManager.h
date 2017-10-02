#ifndef SE_CORE_CAMERA_MANAGER_H_
#define SE_CORE_CAMERA_MANAGER_H_
#include "EntityManager.h"
#include "TransformManager.h"
#include <Graphics\IRenderer.h>
#include <unordered_map>
#include <random>
namespace SE
{
	namespace Core
	{
		class CameraManager
		{
		public:
			CameraManager(Graphics::IRenderer* renderer, const EntityManager& entityManager, TransformManager* transformManager);
			~CameraManager();


			/**
			* @brief	Create a camera for the given entity.
			*/
			void Bind(const Entity& entity, float fov = 1.570796f, float aspectRatio = (800.0f/640.0f), float nearP = 0.01f, float farP = 100.0f, const DirectX::XMFLOAT3& pos = { 0.0f,0.0f,0.0f }, const DirectX::XMFLOAT3& rotation = { 0.0f,0.0f,0.0f });

			/**
			* @brief	GetViewInv.
			*/
			DirectX::XMFLOAT4X4 GetViewInv(const Entity& entity);

			/**
			* @brief Retrieves the projection matrix in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the projection matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			DirectX::XMFLOAT4X4 GetProjection(const Entity& entity);

			/**
			* @brief Retrieves the combined view/projection matrix in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the combined view/projection matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			DirectX::XMFLOAT4X4 GetViewProjection(const Entity& entity);
			/**
			* @brief	Set the camera as the active camera.
			*/
			void SetActive(const Entity& entity);

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame();
		private:
			Graphics::IRenderer* renderer;
			const EntityManager& entityManager;
			TransformManager* transformManager;
			std::default_random_engine generator;

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


			struct CameraData
			{
				static const size_t size = sizeof(Entity) + sizeof(size_t) + sizeof(float)*4 + sizeof(DirectX::XMFLOAT4X4);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				size_t* dirty;
				float* fov;
				float* aspectRatio;
				float* nearPlane;
				float* farPlane;
				DirectX::XMFLOAT4X4* view;
			};

			size_t activeCamera;
			CameraData cameraData;
			std::unordered_map<Entity, size_t, EntityHasher> entityToIndex;

		};
	}
}


#endif