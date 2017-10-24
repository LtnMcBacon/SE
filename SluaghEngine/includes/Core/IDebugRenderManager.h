#ifndef SE_INTERFACE_DEBUG_RENDER_MANAGER_H_
#define SE_INTERFACE_DEBUG_RENDER_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif


#include <Utilz\GUID.h>
#include <Graphics\IRenderer.h>
#include <Utilz\StackAllocator.h>
#include <ResourceHandler\IResourceHandler.h>

#include "IManager.h"
#include "IEntityManager.h"
#include "ITransformManager.h"
#include "ICollisionManager.h"


namespace SE
{
	namespace Core
	{
		class IDebugRenderManager : public IManager
		{
		public:
			struct Point3D
			{
				float x, y, z;
			};
			struct InitializationInfo
			{
				ResourceHandler::IResourceHandler* resourceHandler;
				Graphics::IRenderer* renderer;
				IEntityManager* entityManager;
				ITransformManager* transformManager;
				ICollisionManager* collisionManager;
				Utilz::StackAllocator* perFrameStackAllocator;
			};

			virtual ~IDebugRenderManager() {};


			/**
			* @brief	Enable/disable debug rendering of the object. If the object has a bounding box it will also be rendered. Additional debug rendering can also be added to the entity such as crosses and lines.
			* @param[in] entity Which entity.
			* @param[in] enable True to enable, false to disable
			* @retval true Success.
			* @retval false The bounding box was not found or is already being rendererd.
			*/
			virtual bool ToggleDebugRendering(const Entity& entity, bool enable) = 0;

			/**
			* @brief	Draws a cross on the entity.
			* @details The scale, x, y, and z parameters can be omitted and the cross will be drawn at the position and scale owned by the transform manager.
			*			If the parameters are submitted they will be used in addition to the transform stored in the transform manager. For example if the
			*			transform stored in the transform manager is {1, 1, 2} and the parameters {0, 2, 0} are supplied, the cross will be drawn at the
			*			position {1, 3, 2}.
			* @param[in] entity Which entity.
			* @param[in] scale The size of the cross, defaults to 1.0f.
			* @param[in] x The x-coordinate of the center of the cross.
			* @param[in] y The y-coordinate of the center of the cross.
			* @param[in] z The z-coordinate of the center of the cross.
			*/
			virtual void DrawCross(const Entity& entity, float scale = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f) = 0;

			/**
			* @brief	Draws a line between two points.
			* @details  The positions of the two points are given in local space. When rendered they will be transformed based on the transformation stored in the transform manager.
			*
			* @param[in] entity Which entity.
			* @param[in] a The first point
			* @param[in] b The second point
			*/
			virtual void DrawLine(const Entity& entity, const Point3D& a, const Point3D& b) = 0;
		protected:
			IDebugRenderManager() {};
	
			IDebugRenderManager(const IDebugRenderManager& other) = delete;
			IDebugRenderManager(const IDebugRenderManager&& other) = delete;
			IDebugRenderManager& operator=(const IDebugRenderManager& other) = delete;
			IDebugRenderManager& operator=(const IDebugRenderManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the DebugRenderManager
		**/
		DECLDIR_CORE IDebugRenderManager* CreateDebugRenderManager(const IDebugRenderManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_DEBUG_RENDER_MANAGER_H_