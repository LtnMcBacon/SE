#ifndef _IRENDERER_H_
#define _IRENDERER_H_
#include <cstdint>
namespace SE
{
	namespace Graphics
	{
		class Entity;
		struct RenderObjectInfo
		{
			int32_t meshGUID;
			int32_t* textureGUIDPtr;
			int32_t textureGUIDCount;
		};
		class IRenderer
		{
		public:
			/**
			* @brief    Associates an entity with a renderable object. This function must be called
			*  before an entity can be rendered. Every call to this must be met with a matching
			*  DestroyRenderObject
			* @param[in] entity The entity to bind.
			* @param[in] info Information about the renderable object that is created.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			virtual int CreateRenderObject(const Entity& entity, const RenderObjectInfo& info) = 0;

			/**
			* @brief    Destroys the renderable object that is associated with the entity.
			* @param[in] entity The entity that is bound to the renderable object.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			virtual int DestroyRenderObject(const Entity& entity) = 0;

			/**
			* @brief    Enables rendering for the renderable object bound to the entity.
			* @param[in] entity The entity that is bound to the renderable object.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			virtual int EnableRendering(const Entity& entity) = 0;

			/**
			* @brief    Disables rendering for the renderable object bound to the entity.
			* @param[in] entity The entity that is bound to the renderable object.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			virtual int DisableRendering(const Entity& entity) = 0;

			/**
			* @brief Updates the transformation for an entity that is bound to rendering.
			* @param[in] entity The entity that is bound to the renderable object.
			* @param[in] transform The transfrom to apply to the renderable object, an array of 16 floats in row major format.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			virtual int UpdateTranslation(const Entity& entity, float* transform) = 0;

			/**
			* @brief Updates the view matrix used for rendering
			* @param[in] viewMatrix The view matrix to use, an array of 16 floats in row major format.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			virtual int UpdateView(float* viewMatrix) = 0;

		private:
			IRenderer();
			virtual ~IRenderer() = delete;
			IRenderer(const IRenderer& other) = delete;
			IRenderer(const IRenderer&& other) = delete;
			IRenderer& operator=(const IRenderer& rhs) = delete;
		};
	}
}
#endif
