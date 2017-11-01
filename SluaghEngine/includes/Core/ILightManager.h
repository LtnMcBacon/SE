#ifndef SE_INTERFACE_LIGHT_MANAGER_H_
#define SE_INTERFACE_LIGHT_MANAGER_H_

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
		class ILightManager : public IManager
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
				DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
				float radius = 1.0f;
				DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
				DirectX::XMFLOAT3 dir = { 0.0f, 0.0f, 1.0f };
				bool castShadow = false;
				bool isOrtographic = false;
			};


			virtual ~ILightManager() {};

			virtual void Create(const Entity& entity, const CreateInfo& info) = 0;

			virtual void ToggleLight(const Entity& entity, bool show) = 0;

		protected:
			ILightManager() {};
	
			ILightManager(const ILightManager& other) = delete;
			ILightManager(const ILightManager&& other) = delete;
			ILightManager& operator=(const ILightManager& other) = delete;
			ILightManager& operator=(const ILightManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the LightManager
		**/
		DECLDIR_CORE ILightManager* CreateLightManager(const ILightManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_LIGHT_MANAGER_H_