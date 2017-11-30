#ifndef _INTERFACE_DECAL_MANAGER_H_
#define _INTERFACE_DECAL_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include "IManager.h"
#include "IEntityManager.h"
#include "ITransformManager.h"
#include <Graphics/IRenderer.h>
#include <ResourceHandler/IResourceHandler.h>
#include "ICameraManager.h"

namespace SE
{
	namespace Core
	{
		struct DecalCreateInfo
		{
			Utilz::GUID textureName = Utilz::GUID();
			float opacity = 1.0f;
			float transform[16] = {1.0f, 0.0f,0.0f,0.0f,
									0.0f,1.0f,0.0f,0.0f,
									0.0f,0.0f,1.0f,0.0f,
									0.0f,0.0f,0.0f,1.0f};
		};
		class IDecalManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				IEntityManager* entityManager;
				ITransformManager* transformManager;
				ICameraManager* cameraManager;
				Graphics::IRenderer* renderer;
				ResourceHandler::IResourceHandler* resourceHandler;
			};
			virtual ~IDecalManager() {};

			/*
			 * @brief Creates a decal component and attaches it to the entity.
			 * @details If the entity does not have a transform component, one will be created
			 * for it using the default values for ITransformManager::Create.
			 * @param[in] entity The entity to attach the decal to.
			 * @param[in] textureName The filename of the texture to use as a decal.
			 * @retval 0 On success.
			 */
			virtual int Create(const Entity& entity, const DecalCreateInfo& createInfo) = 0;

			/*
			 * @brief sets a local transform that is offset from the transform component.
			 * @details If the translation in the transform manager is {0,1,0} and the
			 * local transform is {2,0,0} the decal will be rendered at {2,1,0}.
			 * @param[in] entity The entity to apply a local transform to.
			 * @param[in] transform16rowMajor A 4x4 transform matrix in row major format.
			 */
			virtual int SetLocalTransform(const Entity& entity, float* transform16rowMajor) = 0;

			/*
			* @brief Sets the opacity for a decal
			* @param[in] entity The entity to set the opacity of
			* @param[in] opacity A value between 0.0f and 1.0f
			*/
			virtual int SetOpacity(const Entity& entity, float opacity) = 0;

			/*
			* @brief Increases/decreases the opacity of a decal
			* @param[in] entity The entity to set the opacity of
			* @param[in] amount How much to change the opacity.
			*/
			virtual int ModifyOpacity(const Entity& entity, float amount) = 0;


			/*
			 * @brief Removes a decal component from an entity.
			 * @param[in] entity The entity to remove the decal component from.
			 */
			virtual int Remove(const Entity& entity) = 0;

		private:

		protected:
			
		};
		DECLDIR_CORE IDecalManager* CreateDecalManager(const IDecalManager::InitializationInfo& initInfo);
	}
}

#endif
