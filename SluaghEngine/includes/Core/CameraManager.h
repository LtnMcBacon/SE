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
		struct CameraBindInfoStruct
		{
			float aspectRatio = (800.0f / 640.0f);
			float fov = 1.570796f;
			float nearPlane = 0.01f;
			float farPlance = 100.0f;
			DirectX::XMFLOAT3 posistion = { 0.f, 0.f, 0.f };
			DirectX::XMFLOAT3 rotation = { 0.f, 0.f, 0.f };
		};
		class CameraManager
		{
		public:
			CameraManager(Graphics::IRenderer* renderer, const EntityManager& entityManager, TransformManager* transformManager);
			~CameraManager();


			/**
			* @brief	Create a camera for the given entity.
			*/
			void Bind(const Entity& entity, CameraBindInfoStruct& info = CameraBindInfoStruct());

			/*
			* @brief Updates the camera
			* @param[in] entity The entity to update the camera for.
			* @param[in] info The new settings for the camera.
			*/
			void UpdateCamera(const Entity& entity, const CameraBindInfoStruct& info);

			/**
			* @brief	Update a camera for the given entity.
			*/
			void UpdateCamera(const Entity& entity, CameraBindInfoStruct& info);

			/**
			* @brief Retrieves the  view matrix of the camera bound to entity in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the view matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			DirectX::XMFLOAT4X4 GetView(const Entity& entity);

			/**
			* @brief Retrieves the inverse view matrix of the camera bound to entity in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the inverse view matrix if the entity has a camera component. Returns an identity matrix otherwise.
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