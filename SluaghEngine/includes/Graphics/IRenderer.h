#ifndef _IRENDERER_H_
#define _IRENDERER_H_
#include <Utilz/GUID.h>
#include <cstdint>
namespace SE
{
	namespace Graphics
	{
		class Entity;
		struct RenderObjectInfo
		{
			Utilz::GUID meshGUID;
			Utilz::GUID* textureGUIDPtr = nullptr;
			uint32_t textureGUIDCount = 0;
		};
		class IRenderer
		{
		public:
			virtual ~IRenderer() {};

			/**
			* @brief Initialize the renderer
			* @param[in] window A pointer to the window.
			* @retval 0 On success.
			* @retval other See HRESULT
			* @endcode
			*/
			virtual int Initialize(void* window) = 0;

			/**
			* @brief Shutdown the renderer
			* @endcode
			*/
			virtual void Shutdown() = 0;

			/**
			* @brief    Associates an entity with a renderable object. This function must be called
			*  before an entity can be rendered. Every call to this must be met with a matching
			*  DestroyRenderObject
			* @param[in] entity The entity to bind.
			* @param[in] info Information about the renderable object that is created.
			* @retval 0 On success.
			* @endcode
			*/
			virtual int CreateRenderObject(const Entity& entity, const RenderObjectInfo& info) = 0;

			/**
			* @brief    Destroys the renderable object that is associated with the entity.
			* @param[in] entity The entity that is bound to the renderable object.
			* @retval 0 On success.
			* @endcode
			*/
			virtual int DestroyRenderObject(const Entity& entity) = 0;

			/**
			* @brief    Enables rendering for the renderable object bound to the entity.
			* @param[in] entity The entity that is bound to the renderable object.
			* @retval 0 On success.
			* @endcode
			*/
			virtual int EnableRendering(const Entity& entity) = 0;

			/**
			* @brief    Disables rendering for the renderable object bound to the entity.
			* @param[in] entity The entity that is bound to the renderable object.
			* @retval 0 On success.
			* @endcode
			*/
			virtual int DisableRendering(const Entity& entity) = 0;

			/**
			* @brief Updates the transformation for an entity that is bound to rendering.
			* @param[in] entity The entity that is bound to the renderable object.
			* @param[in] transform The transfrom to apply to the renderable object, an array of 16 floats in row major format.
			* @retval 0 On success.
			* @endcode
			*/
			virtual int UpdateTranslation(const Entity& entity, float* transform) = 0;

			/**
			* @brief Updates the view matrix used for rendering
			* @param[in] viewMatrix The view matrix to use, an array of 16 floats in row major format.
			* @retval 0 On success.
			* @endcode
			*/
			virtual int UpdateView(float* viewMatrix) = 0;

			/**
			* @brief Renders the scene
			* @retval 0 On success.
			* @endcode
			*/
			virtual int Render() = 0;


			virtual int CreateVertexBuffer(void*data, size_t vertexCount, size_t stride) = 0;
		protected:
			IRenderer() {};
			IRenderer(const IRenderer& other) = delete;
			IRenderer(const IRenderer&& other) = delete;
			IRenderer& operator=(const IRenderer& rhs) = delete;
		};
	}
}
#endif
