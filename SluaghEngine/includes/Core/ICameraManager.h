#ifndef SE_INTERFACE_CAMERA_MANAGER_H_
#define SE_INTERFACE_CAMERA_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include <Graphics\IRenderer.h>
#include "IManager.h"
#include "IEntityManager.h"
#include "ITransformManager.h"
#include <DirectXMath.h>

namespace SE
{
	namespace Core
	{
		class ICameraManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				Graphics::IRenderer* renderer;  
				IEntityManager* entityManager;
				ITransformManager* transformManager;
			};
			struct CreateInfo
			{
				float aspectRatio = (800.0f / 640.0f);
				float fov = 1.570796f;
				float nearPlane = 0.01f;
				float farPlance = 100.0f;
				DirectX::XMFLOAT3 posistion = { 0.f, 0.f, 0.f };
				DirectX::XMFLOAT3 rotation = { 0.f, 0.f, 0.f };
			};

			virtual ~ICameraManager() {};

			/**
			* @brief	Create a camera for the given entity.
			*/
			virtual void Create(const Entity& entity, CreateInfo& info = CreateInfo()) = 0;

			/*
			* @brief Updates the camera
			* @param[in] entity The entity to update the camera for.
			* @param[in] info The new settings for the camera.
			*/
			virtual void UpdateCamera(const Entity& entity, const CreateInfo& info) = 0;


			/**
			* @brief	Update the active camera.
			*/
			virtual void UpdateCamera(const CreateInfo& info) = 0;

			/**
			* @brief Retrieves the  view matrix of the camera bound to entity in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the view matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			virtual DirectX::XMFLOAT4X4 GetView(const Entity& entity) = 0;

			/**
			* @brief Retrieves the inverse view matrix of the camera bound to entity in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the inverse view matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			virtual DirectX::XMFLOAT4X4 GetViewInv(const Entity& entity) = 0;

			/**
			* @brief Retrieves the projection matrix in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the projection matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			virtual DirectX::XMFLOAT4X4 GetProjection(const Entity& entity) = 0;

			/**
			* @brief Retrieves the combined view/projection matrix in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the combined view/projection matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			virtual DirectX::XMFLOAT4X4 GetViewProjection(const Entity& entity) = 0;

			virtual void WorldSpaceRayFromScreenPos(int x, int y, int screenWidth, int screenHeight, DirectX::XMVECTOR& origin, DirectX::XMVECTOR& direction) const = 0;
			/**
			* @brief	Set the camera as the active camera.
			*/
			virtual void SetActive(const Entity& entity) = 0;

			/**
			*	@brief	Return the entity to the active camera (to allow usage of the default camera).
			*/
			virtual Entity GetActive() = 0;
		protected:
			ICameraManager() {};
	
			ICameraManager(const ICameraManager& other) = delete;
			ICameraManager(const ICameraManager&& other) = delete;
			ICameraManager& operator=(const ICameraManager& other) = delete;
			ICameraManager& operator=(const ICameraManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the CameraManager
		**/
		DECLDIR_CORE ICameraManager* CreateCameraManager(const ICameraManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_CAMERA_MANAGER_H_