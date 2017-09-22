#ifndef _IRENDERER_H_
#define _IRENDERER_H_
#include <Utilz/GUID.h>
#include <cstdint>
#include "TextureDesc.h"
#include "RenderObjectInfo.h"
namespace SE
{
	namespace Graphics
	{
		class Entity;

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
			* @brief    Sets a render job
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @sa RenderObjectInfo
			* @endcode
			*/
			virtual int EnableRendering(const RenderObjectInfo& handles) = 0;

			/**
			* @brief    Removes a render job.
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			virtual int DisableRendering(const RenderObjectInfo& handles) = 0;



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

			/**
			* @brief Creates a vertex buffer.
			* @param[in] data The vertex data.
			* @param[in] vertexCount Number of vertices
			* @param[in] stride The size of one vertex
			* @retval BufferHandle Returns a handle to the created buffer.
			* @retval -1 If something went wrong
			* @endcode
			*/
			virtual int CreateVertexBuffer(void*data, size_t vertexCount, size_t stride) = 0;

			/**
			* @brief Destroys a buffer.
			* @param[in] bufferHandle The handle to the buffer to destroy
			* @endcode
			*/
			virtual void DestroyVertexBuffer(int bufferHandle) = 0;

			/**
			* @brief Creates a texture (SRV)
			* @param[in] data Raw image data
			* @param[in] description Texture description
			* @retval handle on success
			* @retval -1 on failure.
			* @sa TextureDesc
			* 
			*/
			virtual int CreateTexture(void* data, const TextureDesc& description) = 0;

			/**
			* @brief Create a transform.
			* @retval transformHandle Returns a handle to the created transform.
			* @retval -1 If something went wrong
			* @endcode
			*/
			virtual int CreateTransform() = 0;
			/**
			* @brief Destroy a transform
			* @param[in] transformHandle Handle to the transform to destroy.
			* @endcode
			*/
			virtual void DestroyTransform(int transformHandle) = 0;
			/**
			* @brief Updates the transformation for an entity that is bound to rendering.
			* @param[in] transformHandle The transform handle that is bound to the renderable object.
			* @param[in] transform The transfrom to apply to the renderable object, an array of 16 floats in row major format.
			* @retval 0 On success.
			* @endcode
			*/
			virtual int UpdateTransform(int transformHandle, float* transform) = 0;



			/**
			* @brief Create a pixel shader from raw data
			* @param[in] data A pointer to shader blob.
			* @param[in] size The size of the shader blob.
			* @retval handle On success.
			* @retval -1 Something went wrong.
			* @endcode
			*/
			virtual int CreatePixelShader(void* data, size_t size) = 0;

			/**
			* @brief Create a vertex shader from raw data
			* @param[in] data A pointer to shader blob.
			* @param[in] size The size of the shader blob.
			* @retval handle On success.
			* @retval -1 Something went wrong.
			* @endcode
			*/
			virtual int CreateVertexShader(void* data, size_t size) = 0;

			/**
			* @brief Resizes the swapchain
			* @param[in] windowHandle A window handle.
			* @endcode
			*/
			virtual void ResizeSwapChain(void* windowHandle) = 0;
		protected:
			IRenderer() {};
			IRenderer(const IRenderer& other) = delete;
			IRenderer(const IRenderer&& other) = delete;
			IRenderer& operator=(const IRenderer& rhs) = delete;
		};
	}
}
#endif
