#ifndef SE_CORE_CAMERA_MANAGER_H_
#define SE_CORE_CAMERA_MANAGER_H_
#include <ICameraManager.h>

#include <unordered_map>
#include <random>
namespace SE
{
	namespace Core
	{
		class CameraManager : public ICameraManager
		{
		public:
			CameraManager(const InitializationInfo& initInfo);
			~CameraManager();


			/**
			* @brief	Create a camera for the given entity.
			*/
			void Create(const Entity& entity, CreateInfo& info = CreateInfo())override;

			/*
			* @brief Updates the camera
			* @param[in] entity The entity to update the camera for.
			* @param[in] info The new settings for the camera.
			*/
			void UpdateCamera(const Entity& entity, const CreateInfo& info)override;


			/**
			* @brief	Update the active camera.
			*/
			void UpdateCamera(const CreateInfo& info)override;

			/**
			* @brief Retrieves the  view matrix of the camera bound to entity in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the view matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			DirectX::XMFLOAT4X4 GetView(const Entity& entity)override;

			/**
			* @brief Retrieves the inverse view matrix of the camera bound to entity in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the inverse view matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			DirectX::XMFLOAT4X4 GetViewInv(const Entity& entity)override;

			/**
			* @brief Retrieves the projection matrix in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the projection matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			DirectX::XMFLOAT4X4 GetProjection(const Entity& entity)override;

			/**
			* @brief Retrieves the combined view/projection matrix in a row major format.
			* @param[in] entity The entity which the camera is to be gotten from.
			* @retval Returns the combined view/projection matrix if the entity has a camera component. Returns an identity matrix otherwise.
			*/
			DirectX::XMFLOAT4X4 GetViewProjection(const Entity& entity)override;

			void WorldSpaceRayFromScreenPos(int x, int y, int screenWidth, int screenHeight, DirectX::XMVECTOR& origin, DirectX::XMVECTOR& direction) const override;
			/**
			* @brief	Set the camera as the active camera.
			*/
			void SetActive(const Entity& entity)override;

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;

			/**
			*	@brief	Return the entity to the active camera (to allow usage of the default camera).
			*/
			Entity GetActive()override {
				return currentActive.entity;
			}
		private:
			InitializationInfo initInfo;
			std::default_random_engine generator;

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

			struct ActiveCam
			{
				size_t activeCamera;
				Entity entity;
			};
			ActiveCam currentActive;
			CameraData cameraData;
			std::unordered_map<Entity, size_t, EntityHasher> entityToIndex;

		};
	}
}


#endif