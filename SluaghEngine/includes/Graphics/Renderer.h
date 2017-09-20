#ifndef SE_GRAPHICS_RENDERER_H_
#define SE_GRAPHICS_RENDERER_H_
#include "IRenderer.h"
#include "DeviceManager.h"
#include "GraphicResourceHandler.h"

#include <Utilz\Camera.h>

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
			int Initialize(void* window);

			/**
			* @brief Shutdown the renderer
			* @endcode
			*/
			void Shutdown();

			/**
			* @brief    Sets a render job
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @sa RenderObjectInfo
			* @endcode
			*/
			int EnableRendering(const RenderObjectInfo& handles);

			/**
			* @brief    Removes a render job.
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			int DisableRendering(const RenderObjectInfo& handles);

			/**
			* @brief Updates the view matrix used for rendering
			* @param[in] viewMatrix The view matrix to use, an array of 16 floats in row major format.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int UpdateView(float* viewMatrix);

			/**
			* @brief Renders the scene
			* @retval return_value_0 Returns 0 On success.
			* @endcode
			*/
			int Render();


			/**
			* @brief Creates a vertex buffer.
			* @param[in] data The vertex data.
			* @param[in] vertexCount Number of vertices
			* @param[in] stride The size of one vertex
			* @retval BufferHandle Returns a handle to the created buffer.
			* @retval -1 If something went wrong
			* @endcode
			*/
			int CreateVertexBuffer(void*data, size_t vertexCount, size_t stride);

			/**
			* @brief Destroys a buffer.
			* @param[in] bufferHandle The handle to the buffer to destroy
			* @endcode
			*/
			void DestroyVertexBuffer(int bufferHandle);
			
			/**
			* @brief Creates a texture (SRV)
			* @param[in] data Raw image data
			* @param[in] description Texture description
			* @sa TextureDesc
			*
			*/
			int CreateTexture(void* data, const TextureDesc& description) override;

			/**
			* @brief Create a transform.
			* @param[in] transform Initial transform.
			* @retval transformHandle Returns a handle to the created transform.
			* @retval -1 If something went wrong
			* @endcode
			*/
			int CreateTransform();
			/**
			* @brief Destroy a transform
			* @param[in] transformHandle Handle to the transform to destroy.
			* @endcode
			*/
			void DestroyTransform(int transformHandle);
			/**
			* @brief Updates the transformation for an entity that is bound to rendering.
			* @param[in] transformHandle The transform handle that is bound to the renderable object.
			* @param[in] transform The transfrom to apply to the renderable object, an array of 16 floats in row major format.
			* @retval 0 On success.
			* @endcode
			*/
			int UpdateTransform(int transformHandle, float* transform);

			/**
			* @brief Create a shader resource view (texture)
			* @param[in] data A pointer to the texture data.
			* @param[in] size The size of the texture.
			* @param[in] bytewidth The width of the texture
			* @retval handle On success.
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int CreateTexture(void*data, size_t size, size_t bytewidth);

			/**
			* @brief Create a pixel shader from raw data
			* @param[in] data A pointer to shader blob.
			* @param[in] size The size of the shader blob.
			* @retval handle On success.
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int CreatePixelShader(void* data, size_t size);
		private:
			Renderer(const Renderer& other) = delete;
			Renderer(const Renderer&& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;

			struct OncePerFrameConstantBuffer
			{
				DirectX::XMFLOAT4X4 viewproj;
			};

			struct OncePerObjectConstantBuffer
			{
				DirectX::XMFLOAT4X4 world;
			};
			int oncePerFrameBufferID;

			DeviceManager* device;

			GraphicResourceHandler* graphicResourceHandler;

			Utilz::Camera cam;

			std::vector<RenderObjectInfo> renderJobs;


		};

	}
}
#endif //SE_GRAPHICS_RENDERER_H_