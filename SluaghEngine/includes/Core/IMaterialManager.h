#ifndef SE_INTERFACE_MATERIAL_MANAGER_H_
#define SE_INTERFACE_MATERIAL_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif


#include <Utilz\GUID.h>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>

#include "IManager.h"
#include "IRenderableManager.h"

namespace SE
{
	namespace Core
	{
		class IMaterialManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				ResourceHandler::IResourceHandler* resourceHandler;
				Graphics::IRenderer* renderer;
				IEntityManager* entityManager;
				IRenderableManager* renderableManager;
			};

			struct CreateInfo
			{
				Utilz::GUID meshGUID;
				bool transparent = false;
				bool wireframe = false;
			};


			virtual ~IMaterialManager() {};

		protected:
			IMaterialManager() {};
	
			IMaterialManager(const IMaterialManager& other) = delete;
			IMaterialManager(const IMaterialManager&& other) = delete;
			IMaterialManager& operator=(const IMaterialManager& other) = delete;
			IMaterialManager& operator=(const IMaterialManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the MaterialManager
		**/
		DECLDIR_CORE IMaterialManager* CreateMaterialManager(const IMaterialManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_MATERIAL_MANAGER_H_