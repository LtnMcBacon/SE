#ifndef _IRENDERER_H_
#define _IRENDERER_H_
#include <Utilz/GUID.h>
#include <cstdint>
#include "TextureDesc.h"
#include "RenderObjectInfo.h"
#include <Graphics\GUIInfo.h>
#include <Graphics\LightInfo.h>
#include "ShaderSettings.h"
#include "LineRenderJob.h"
#include <ResourceHandler\IResourceHandler.h>
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
			* @retval Returns a handle to the job on success.
			* @retval -1 on failure.
			* @sa RenderObjectInfo
			* @endcode
			*/
			virtual int EnableRendering(const RenderObjectInfo& handles) = 0;

			/**
			* @brief    Removes a render job.
			* @param[in] jobID The ID of the job, gotten through EnableRendering
			* @retval 0 On success.
			* @sa EnableRendering
			*/
			virtual int DisableRendering(uint32_t jobID) = 0;

			

			/**
			* @brief    Sets a Text render jobs
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			virtual int EnableTextRendering(const TextGUI & handles) = 0;

			/**
			* @brief    Removes a Text render job.
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			virtual int DisableTextRendering(const TextGUI& handles) = 0;

			/**
			* @brief    Sets Text render jobs
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			virtual int EnableTextureRendering(const GUITextureInfo & handles) = 0;
			
			/**
			* @brief    Removes a Text render job.
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			virtual int DisableTextureRendering(const GUITextureInfo& handles) = 0;

			/**
			* @brief    Sets Light render jobs
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			virtual int EnableLightRendering(const LightData & handles) = 0;

			/**
			* @brief    Removes a Light render job.
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			virtual int DisableLightRendering(const LightData& handles) = 0;

			/**
			* @brief    Sets a render job
			* @param[in] lineJob The job containing information about the job.
			* @retval Returns a handle to the job on success.
			* @retval -1 on failure.
			* @sa LineRenderJob
			*/
			virtual int AddLineRenderJob(const LineRenderJob& lineJob) = 0;
			
			/**
			* @brief    Removes a line render job.
			* @param[in] lineJobID The ID of the job, gotten through return value of AddLineRenderJob
			* @retval 0 On success.
			* @sa EnableRendering
			*/
			virtual int RemoveLineRenderJob(uint32_t lineJobID) = 0;

			/**
			* @brief Updates the transformation of a line render job.
			* @param[in] lineJobID The ID of the job to update.
			* @param[in] transform The transfrom to apply to the job, an array of 16 floats in row major format.
			* @retval 0 On success.
			*/
			virtual int UpdateLineRenderJobTransform(uint32_t lineJobID, float* transform) = 0;

			/**
			* @brief Updates the range of the line render job. (Which range of the dynamic vertex buffer to grab vertices from.)
			* @param[in] lineJobID The ID of the job to update.
			* @param[in] startVertex The first vertex to draw
			* @param[in] vertexCount The number of vertices to draw.
			* @retval 0 On success.
			*/
			virtual int UpdateLineRenderJobRange(uint32_t lineJobID, uint32_t startVertex, uint32_t vertexCount) = 0;

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
			* @brief Updates the transformation of a render job.
			* @param[in] jobID The ID of the job to update.
			* @param[in] transform The transfrom to apply to the job, an array of 16 floats in row major format.
			* @retval 0 On success.
			* @endcode
			*/
			virtual int UpdateTransform(uint32_t jobID, float* transform) = 0;


			/**
			* @brief Create a pixel shader from raw data
			* @param[in] data A pointer to shader blob.
			* @param[in] size The size of the shader blob.
			* @param[out] reflection Stores information from a shader reflection at the address pointed to by reflection if not nullptr.
			* @retval handle On success.
			* @retval -1 Something went wrong.
			* @endcode
			*/
			virtual int CreatePixelShader(void* data, size_t size, ShaderSettings* reflection = nullptr) = 0;

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
			* @brief Create a vertex buffer with CPU write access
			* @param[in] bytewidth The size of the buffer
			* @param[in] initialData The data to create the buffer with
			* @param[in] vertexByteSize The size of the vertex type in bytes
			* @param[in] initialDataSize The size in bytes of the initial data
			* @retval handle On success.
			* @retval -1 Something went wrong.
			*/
			virtual int CreateDynamicVertexBuffer(size_t bytewidth, size_t vertexByteSize, void* initialData = nullptr, size_t initialDataSize = 0) = 0;

			/**
			* @brief Updates a dynamic vertex buffer. Replaces the existing data.
			* @param[in] handle The handle of the vertex buffer to update
			* @param[in] data The data to put in the vertex buffer
			* @param[in] totalSize The total size in bytes of the data to put in the buffer
			* @param[in] sizePerElement The size per vertex in bytes
			* @retval 0 On success.
			* @retval -1 Something went wrong.
			*/
			virtual int UpdateDynamicVertexBuffer(int handle, void* data, size_t totalSize, size_t sizePerElement) = 0;


			/**
			* @brief Create a new font
			* @retval 0+ Font ID
			* @retval -1 Something went wrong.
			* @endcode
			*/
			virtual int CreateTextFont(Utilz::GUID fontFile, ResourceHandler::IResourceHandler* resourceHandler) = 0;

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
