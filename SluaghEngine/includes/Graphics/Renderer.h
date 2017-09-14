#ifndef SE_GRAPHICS_RENDERER_H_
#define SE_GRAPHICS_RENDERER_H_
#include "IRenderer.h"
#include "DeviceManager.h"
#include "MaterialHandler.h"

namespace SE
{
	namespace Graphics
	{

		class Renderer : public IRenderer
		{
		public:
			Renderer();
			~Renderer();

			/**
			* @brief Initialize the renderer
			* @param[in] window A pointer to the window.
			* @retval 0 On success.
			* @endcode
			*/
			long Initialize(void* window);

			/**
			* @brief Shutdown the renderer
			* @endcode
			*/
			void Shutdown();

			/**
			* @brief    Associates an entity with a renderable object. This function must be called
			*  before an entity can be rendered. Every call to this must be met with a matching
			*  DestroyRenderObject
			* @param[in] entity The entity to bind.
			* @param[in] info Information about the renderable object that is created.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int CreateRenderObject(const Entity& entity, const RenderObjectInfo& info);

			/**
			* @brief    Destroys the renderable object that is associated with the entity.
			* @param[in] entity The entity that is bound to the renderable object.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int DestroyRenderObject(const Entity& entity);

			/**
			* @brief    Enables rendering for the renderable object bound to the entity.
			* @param[in] entity The entity that is bound to the renderable object.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int EnableRendering(const Entity& entity);

			/**
			* @brief    Disables rendering for the renderable object bound to the entity.
			* @param[in] entity The entity that is bound to the renderable object.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int DisableRendering(const Entity& entity);

			/**
			* @brief Updates the transformation for an entity that is bound to rendering.
			* @param[in] entity The entity that is bound to the renderable object.
			* @param[in] transform The transfrom to apply to the renderable object, an array of 16 floats in row major format.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int UpdateTranslation(const Entity& entity, float* transform);

			/**
			* @brief Updates the view matrix used for rendering
			* @param[in] viewMatrix The view matrix to use, an array of 16 floats in row major format.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int UpdateView(float* viewMatrix);
		private:
			Renderer(const Renderer& other) = delete;
			Renderer(const Renderer&& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;

			MaterialHandler* materialHandler;
			DeviceManager* device;

		};

	}
}
#endif //SE_GRAPHICS_RENDERER_H_