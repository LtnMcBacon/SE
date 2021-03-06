#ifndef SE_CORE_DEBUG_RENDER_MANAGER_H_
#define SE_CORE_DEBUG_RENDER_MANAGER_H_


#include <IDebugRenderManager.h>
#include <unordered_map>
#include <vector>
#include <random>
#include <unordered_set>

namespace SE
{
	namespace Core
	{

		/**
		*
		* @brief This manager is used to render debug-related elements using line lists. Lazy creation is employed which means that if an entity does not have a DebugRender-component when any of the methods in this class are called, the component will be created without having to explicitly create a component.
		*
		* @details Is dependent on a transform component being bound to the same entity for all methods.
		*
		**/

		class DebugRenderManager : public IDebugRenderManager
		{
		public:
			DebugRenderManager( const IDebugRenderManager::InitializationInfo& initInfo);
			~DebugRenderManager();
			DebugRenderManager(const DebugRenderManager& other) = delete;
			DebugRenderManager(const DebugRenderManager&& other) = delete;
			DebugRenderManager& operator=(const DebugRenderManager& other) = delete;


			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;
			/**
			* @brief	Enable/disable debug rendering of the object. If the object has a bounding box it will also be rendered. Additional debug rendering can also be added to the entity such as crosses and lines.
			* @param[in] entity Which entity.
			* @param[in] enable True to enable, false to disable
			* @retval true Success.
			* @retval false The bounding box was not found or is already being rendererd.
			*/
			bool ToggleDebugRendering(const Entity& entity, bool enable)override;

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
			void DrawCross(const Entity& entity,float scale = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f)override;

			/**
			* @brief	Draws a line between two points.
			* @details  The positions of the two points are given in local space. When rendered they will be transformed based on the transformation stored in the transform manager.
			*			
			* @param[in] entity Which entity.
			* @param[in] a The first point
			* @param[in] b The second point
			*/
			void DrawLine(const Entity& entity, const Point3D& a, const Point3D& b)override;
			void Destroy(const Entity& e)override;
		private:
			Graphics::Pipeline pipeline;
			Utilz::GUID vertexBufferID;
			Utilz::GUID vertexShaderID;
			Utilz::GUID pixelShaderID;
			Utilz::GUID transformBufferID;
			struct LineSegment
			{
				Point3D a;
				Point3D b;
			};
			InitializationInfo initInfo;

			/**<The collision manager loads in the bounding boxes asynchronously. If ToggleDebugRendering is called before the BB is loaded, the entity gets put in this vector for the time being.*/
			std::vector<Entity> awaitingBoundingBoxes;
			void CreateBoundingBoxes();

			static const size_t maximumLinesToRender = 16384;
			static const size_t dynamicVertexBufferSize = sizeof(LineSegment) * maximumLinesToRender;
			bool dirty = false;
			size_t lineCount = 0;
			std::unordered_map<Entity, std::vector<LineSegment>, EntityHasher> entityToLineList;
			std::unordered_map<Entity, uint32_t, EntityHasher> entityToJobID;
			std::unordered_map<Entity, DirectX::XMFLOAT4X4, EntityHasher> cachedTransforms;
			//In case we don't leave it up to the caller to not enable the same entity twice
			std::unordered_set<Entity, EntityHasher> entityRendersBoundingVolume;

			void SetDirty(const Entity & entity, size_t index);

			std::default_random_engine generator;
			void GarbageCollection()override;
			void Destroy(size_t index)override;

		};
	}
}




#endif
