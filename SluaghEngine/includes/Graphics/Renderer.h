#ifndef SE_GRAPHICS_RENDERER_H_
#define SE_GRAPHICS_RENDERER_H_
#include "IRenderer.h"
#include "DeviceManager.h"
#include "GraphicResourceHandler.h"
#include <Graphics\GUIInfo.h>
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
			int Initialize(void* window) override;

			/**
			* @brief Shutdown the renderer
			* @endcode
			*/
			void Shutdown() override;

			/**
			* @brief    Sets a render job
			* @param[in] handles The handles struct
			* @retval Returns a handle to the job on success.
			* @retval -1 on failure.
			* @sa RenderObjectInfo
			*/
			int EnableRendering(const RenderObjectInfo& handles) override;

			/**
			* @brief    Removes a render job.
			* @param[in] jobID The ID of the job, gotten through EnableRendering
			* @retval 0 On success.
			* @sa EnableRendering
			*/
			int DisableRendering(uint32_t jobID) override;


			/**
			* @brief    Sets Text render jobs
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			int EnableTextRendering(const TextGUI & handles) override;

			/**
			* @brief    Removes a Text render job.
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			int DisableTextRendering(const TextGUI& handles) override;

			/**
			* @brief    Sets Text render jobs
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			int EnableTextureRendering(const GUITextureInfo & handles) override;

			/**
			* @brief    Removes a Text render job.
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			int DisableTextureRendering(const GUITextureInfo& handles) override;

			/**
			* @brief    Sets a render job
			* @param[in] lineJob The job containing information about the job.
			* @retval Returns a handle to the job on success.
			* @retval -1 on failure.
			* @sa LineRenderJob
			*/
			int AddLineRenderJob(const LineRenderJob& lineJob) override;

			/**
			* @brief    Removes a line render job.
			* @param[in] lineJobID The ID of the job, gotten through return value of AddLineRenderJob
			* @retval 0 On success.
			* @sa EnableRendering
			*/
			int RemoveLineRenderJob(uint32_t lineJobID) override;

			/**
			* @brief Updates the transformation of a line render job.
			* @param[in] lineJobID The ID of the job to update.
			* @param[in] transform The transfrom to apply to the job, an array of 16 floats in row major format.
			* @retval 0 On success.
			*/
			int UpdateLineRenderJobTransform(uint32_t lineJobID, float* transform) override;

			/**
			* @brief Updates the range of the line render job. (Which range of the dynamic vertex buffer to grab vertices from.)
			* @param[in] lineJobID The ID of the job to update.
			* @param[in] startVertex The first vertex to draw
			* @param[in] vertexCount The number of vertices to draw.
			* @retval 0 On success.
			*/
			int UpdateLineRenderJobRange(uint32_t lineJobID, uint32_t startVertex, uint32_t vertexCount) override;

			/**
			* @brief Updates the view matrix used for rendering
			* @param[in] viewMatrix The view matrix to use, an array of 16 floats in row major format.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int UpdateView(float* viewMatrix) override;

			/**
			* @brief Renders the scene
			* @retval return_value_0 Returns 0 On success.
			* @endcode
			*/
			int Render() override;


			/**
			* @brief Creates a vertex buffer.
			* @param[in] data The vertex data.
			* @param[in] vertexCount Number of vertices
			* @param[in] stride The size of one vertex
			* @retval BufferHandle Returns a handle to the created buffer.
			* @retval -1 If something went wrong
			* @endcode
			*/
			int CreateVertexBuffer(void*data, size_t vertexCount, size_t stride) override;

			/**
			* @brief Destroys a buffer.
			* @param[in] bufferHandle The handle to the buffer to destroy
			* @endcode
			*/
			void DestroyVertexBuffer(int bufferHandle) override;
			
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
			int CreateTransform() override;
			/**
			* @brief Destroy a transform
			* @param[in] transformHandle Handle to the transform to destroy.
			* @endcode
			*/
			void DestroyTransform(int transformHandle) override;
			/**
			* @brief Updates the transformation of a render job.
			* @param[in] jobID The ID of the job to update.
			* @param[in] transform The transfrom to apply to the job, an array of 16 floats in row major format.
			* @retval 0 On success.
			* @endcode
			*/
			int UpdateTransform(uint32_t jobID, float* transform) override;



			/**
			* @brief Create a pixel shader from raw data
			* @param[in] data A pointer to shader blob.
			* @param[in] size The size of the shader blob.
			* @param[out] reflection Stores information from a shader reflection at the address pointed to by reflection if not nullptr.
			* @retval handle On success.
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int CreatePixelShader(void* data, size_t size, ShaderSettings* reflection = nullptr) override;

			/**
			* @brief Create a vertex buffer with CPU write access
			* @param[in] bytewidth The size of the buffer
			* @param[in] initialData The data to create the buffer with
			* @param[in] vertexByteSize The size of the vertex type in bytes
			* @param[in] initialDataSize The size in bytes of the initial data
			* @retval handle On success.
			* @retval -1 Something went wrong.
			*/
			int CreateDynamicVertexBuffer(size_t bytewidth, size_t vertexByteSize, void* initialData = nullptr, size_t initialDataSize = 0) override;

			/**
			* @brief Updates a dynamic vertex buffer. Replaces the existing data.
			* @param[in] handle The handle of the vertex buffer to update
			* @param[in] data The data to put in the vertex buffer
			* @param[in] totalSize The total size in bytes of the data to put in the buffer
			* @param[in] sizePerElement The size per vertex in bytes
			* @retval 0 On success.
			* @retval -1 Something went wrong.
			*/
			int UpdateDynamicVertexBuffer(int handle, void* data, size_t totalSize, size_t sizePerElement) override;

			/**
			* @brief Create a vertex shader from raw data
			* @param[in] data A pointer to shader blob.
			* @param[in] size The size of the shader blob.
			* @retval handle On success.
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int CreateVertexShader(void* data, size_t size) override;

			/**
			* @brief Create a new fomt
			* @retval 0+ Font ID
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int CreateTextFont(Utilz::GUID fontFile, ResourceHandler::IResourceHandler* resourceHandler) override;
			/**
			* @brief Resizes the swapchain
			* @param[in] windowHandle A window handle.
			* @endcode
			*/
			void ResizeSwapChain(void* windowHandle) override;
		private:
			Renderer(const Renderer& other) = delete;
			Renderer(const Renderer&& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;


			struct OncePerFrameConstantBuffer
			{
				DirectX::XMFLOAT4X4 viewproj;
			};

			int oncePerFrameBufferID;

			DeviceManager* device;

			GraphicResourceHandler* graphicResourceHandler;

			/******** Instanced render job members ********/
			static const uint32_t maxDrawInstances = 256;
			int instancedTransformsConstantBufferHandle = -1;
			struct RenderBucket
			{
				RenderObjectInfo stateInfo;
				std::vector<DirectX::XMFLOAT4X4> transforms;
				/**<Whenever a job is removed the transform vector replaces the removed job's transform with the last added job's transform, as such we need a reverse lookup instead of iterating over all the jobs to find who had the bucket and transform index of the moved transform. The same index is used for this vector as for the transforms vector*/
				std::vector<uint32_t> jobsInBucket;
			};
			std::vector<RenderBucket> renderBuckets;
			struct BucketAndTransformIndex
			{
				uint32_t bucketIndex;
				uint32_t transformIndex;
			};
			std::vector<BucketAndTransformIndex> jobIDToBucketAndTransformIndex;
			std::stack<uint32_t> freeJobIndices;
			/******** END Instanced render job members ********/

			/*********** Line render job members **************/
			std::vector<LineRenderJob> lineRenderJobs;
			std::stack<uint32_t> freeLineRenderJobIndices;
			int singleTransformConstantBuffer = -1;

			/*********** END Line render job members **********/

			std::vector<RenderObjectInfo> renderJobs;
			std::vector<TextGUI> renderTextJobs;
			std::vector<GUITextureInfo> renderTextureJobs;

			// fonts
			std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
			std::vector<DirectX::SpriteFont> fonts;
			int RetFontData(const Utilz::GUID & guid, void * data, size_t size);
		};

	}
}
#endif //SE_GRAPHICS_RENDERER_H_