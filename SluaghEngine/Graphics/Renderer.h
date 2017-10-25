#ifndef SE_GRAPHICS_RENDERER_H_
#define SE_GRAPHICS_RENDERER_H_
#include <IRenderer.h>
#include "DeviceManager.h"
#include "GraphicResourceHandler.h"

#include "AnimationSystem.h"
#include <mutex>
#include "MemoryMeasuring.h"
#include <Utilz\CircularFiFo.h>
#include <thread>
#include "GPUTimeCluster.h"
#include <Utilz\CPUTimeCluster.h>


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
			int Initialize(const InitializationInfo& initInfo) override;

			/**
			* @brief Shutdown the renderer
			* @endcode
			*/
			void Shutdown() override;


			IPipelineHandler* GetPipelineHandler() override
			{
				return pipelineHandler;
			};
			IPipelineHandler* GetSecondaryPipelineHandler() override
			{
				return secPipelineHandler;
			}
			/**
			* @brief Adds a renderjob to be rendered, is rendered until RemoveRenderJob is called
			* @param[in] job Struct containing all information required to render.
			* @param[in] group The group (enum) the job should belong to.
			* @retval Returns a handle to the job on success.
			* @retval -1 on failure.
			* @sa RenderJob, RenderGroup
			*/
			uint32_t AddRenderJob(const RenderJob& job, RenderGroup group) override;

			/**
			* @brief Removes a renderjob that has been added by AddRenderJob
			* @param[in] jobID The ID retrieved from AddRenderJob
			* @sa AddRenderJob
			*/
			void RemoveRenderJob(uint32_t jobID) override;

			/**
			* @brief Replaces an existing renderjob with a new one. The job must have been added by AddRenderJob.
			* @param[in] jobID The ID retrieved from AddRenderJob
			* @param[in] newJob The job to replace the existing job with.
			* @warning Performance heavy if used excessively.
			* @sa AddRenderJob
			*/
			void ChangeRenderJob(uint32_t jobID, const RenderJob& newJob) override;

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
			* @brief    Changes vertex buffer handle for render job
			* @param[in] jobID The ID of the job, gotten through EnableRendering
			* @param[in] handles The RenderObjectInfo to change to
			* @retval 0 On success.
			* @sa EnableRendering
			*/
			int UpdateRenderingBuffer(uint32_t jobID, const RenderObjectInfo& handles) override;

			/**
			* @brief    Sets Text render jobs
			* @param[in] handles The handles struct
			* @retval jobID On success.
			* @endcode
			*/
			size_t EnableTextRendering(const TextGUI & handles) override;

			/**
			* @brief    Removes a Text render job.
			* @param[in] handles The handles struct
			* @retval jobID On success.
			* @endcode
			*/
			size_t DisableTextRendering(const size_t & jobID) override;

			/**
			* @brief    Sets Text render jobs
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			size_t EnableTextureRendering(const GUITextureInfo & handles) override;

			/**
			* @brief    Removes a Text render job.
			* @param[in] handles The handles struct
			* @retval jobID On success.
			* @endcode
			*/
			size_t DisableTextureRendering(const size_t & jobID) override;

			/**
			* @brief    Sets Light render jobs
			* @param[in] handles The handles struct
			* @retval jobID On success.
			* @endcode
			*/
			int EnableLightRendering(const LightData & handles) override;

			/**
			* @brief    Removes a Light render job.
			* @param[in] handles The handles struct
			* @retval 0 On success.
			* @endcode
			*/
			int DisableLightRendering(size_t ID) override;

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
			* @brief Updates the lightPos used for rendering
			* @param[in] pos The pos to use.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int UpdateLightPos(const DirectX::XMFLOAT3& pos, size_t ID) override;

			/**
			* @brief Updates the view matrix used for rendering
			* @param[in] viewMatrix The view matrix to use, an array of 16 floats in row major format.
			* @retval return_value_0 Returns 0 on success.
			* @endcode
			*/
			int UpdateView(float* viewMatrix, const DirectX::XMFLOAT4& cameraPos) override;

			/**
			* @brief Renders the scene
			* @retval return_value_0 Returns 0 On success.
			* @endcode
			*/
			int Render() override;

			/**
			* @brief Begins the frame. Clears the render target view.
			* @retval 0 On success.
			*/
			int BeginFrame() override;

			/*
			* @brief Ends the frame. Presents the rendered scene to the screen.
			* @retval 0 On success.
			*/
			int EndFrame() override;


			/*
			* @brief If the implementation of the renderer is DirectX11, destination points to a struct with a member for a pointer to the ID3D11Device as well as a pointer to the ID3D11DeviceContext
			* @details This method is needed to instanciate third party libraries such as Imgui which needs to access the device and device context. Not very pretty but we need it...
			* @param[in] destination A pointer to a struct that can hold whatever info Imgui needs. In the case of DX11, the struct has the structure {ID3D11Device*, ID3D11DeviceContext*}
			* @param[in] size The size of the struct, used to validate that the void ptr points to a struct of sufficient size.
			*/
			void GetDeviceInfo(void* destination, size_t size) override;

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
			* @brief Destroy a vertex buffer.
			* @param[in] handle The handle to the buffer to destroy.
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
			* @brief Updates the transformation of a render job.
			* @param[in] jobID The ID of the job to update.
			* @param[in] transform The transfrom to apply to the job, an array of 16 floats in row major format.
			* @retval 0 On success.
			* @endcode
			*/
			int UpdateTransform(uint32_t jobID, float* transform) override;

			//int UpdateBoneTransform(uint32_t jobID, float* transforms, size_t nrOfJoints);

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
			* @brief Create a compute shader from raw data
			* @param[in] data A pointer to shader blob.
			* @param[in] size The size of the shader blob.
			* @retval handle On success.
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int CreateComputeShader(void* data, size_t size) override;


			/**
			* @brief Create a new fomt
			* @retval 0+ Font ID
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int CreateTextFont(void * data, size_t size) override;
			/**
			* @brief Resizes the swapchain
			* @param[in] windowHandle A window handle.
			* @endcode
			*/
			void ResizeSwapChain(void* windowHandle) override;

			/**
			* @brief Create a skeleton
			* @param[in] jointData The joint data.
			* @param[in] nrOfJoints The number of joints.
			* @endcode
			*/
			int CreateSkeleton(JointAttributes* jointData, size_t nrOfJoints)override;

			/**
			* @brief Create an animation
			* @param[in] matrices The animation keyframes
			* @param[in] nrOfKeyframes The number of keyframes.
			* @param[in] nrOfJoints The number of joints.
			* @endcode
			*/
			int CreateAnimation(DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints) override;

			/**
			* @brief Start a new animation job
			* @param[in] info Animation info
			* @sa AnimationJobInfo
			* @retval -1 On fail.
			* @retval handle The job id.
			* @endcode
			*/
			int StartAnimation(const AnimationJobInfo& info) override;

			/**
			* @brief Stop an animation (This removes the job)
			* @param[in] job The job top stop
			* @endcode
			*/
			void StopAnimation(int job)override;

			/**
			* @brief Update an animation job
			* @param[in] job Which animation job to update
			* @param[in] info Animation info
			* @sa AnimationJobInfo
			* @endcode
			*/
			void UpdateAnimation(int job, const AnimationJobInfo& info)override;

			/**
			* @brief Set the speed of an animation job
			* @param[in] job Which animation job to update
			* @param[in] speed The speed
			* @sa AnimationJobInfo
			* @endcode
			*/
			void SetAnimationSpeed(int job, float speed) override;

			/**
			* @brief Set the speed of an animation job
			* @param[in] job Which animation job to update
			* @param[in] keyframe The keyframe
			* @endcode
			*/
			void SetKeyFrame(int job, float keyframe);

			/**
			* @brief Start an animation job
			* @param[in] job Which animation job to Start
			* @sa AnimationJobInfo
			* @endcode
			*/
			void StartAnimation(int job) override;

			/**
			* @brief Pause an animation job
			* @param[in] job Which animation job to pause
			* @sa AnimationJobInfo
			* @endcode
			*/
			void PauseAnimation(int job) override;

			/**
			* @brief	The amount of VRam currently used.
			*
			* @retval size_t The amount of VRam used in bytes.
			*
			*/
			inline size_t GetVRam() override {
				return memMeasure.GetVRam();
			};

			/**
			* @brief Checks if there is enough free VRAM to allocate a resource.
			* @param sizeToAdd The size of the new resource.
			*
			* @retval true If there is enough VRAM to allocate this resource.
			* @retval true If there is not enough VRAM to allocate this resource.
			*
			*/
			bool IsUnderLimit(size_t sizeToAdd)override;

			/**
			* @brief Checks if there is enough free VRAM to allocate a resource with a potential unallocated amount.
			* @param potential The size of resources that can be unloaded to make room.
			* @param sizeToAdd The size of the new resource.
			*
			* @retval true If there is enough VRAM to allocate this resource.
			* @retval true If there is not enough VRAM to allocate this resource.
			*
			*/
			bool IsUnderLimit(size_t potential, size_t sizeToAdd)override;

			/*
			* @brief Saves the current error log in the parameter. 
			* @warning The vector can only be used until the next call to BeginFrame if it is stored as a reference.
			*/
			inline std::vector<std::string>& GetErrorLog() override{
				return errorLog;
			}

			/**
			* @brief	Return a map of with profiling information.
			*
			*/
			inline void GetProfilingInformation(Utilz::TimeMap& map)override
			{
				cpuTimer.GetMap(map);
				gpuTimer->GetMap(map);
			}

			int EnableBloom(int handleHorizontal, int handleVertical);

			int DisableBloom();

		private:
			Renderer(const Renderer& other) = delete;
			Renderer(const Renderer&& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;

			InitializationInfo initInfo;

			IPipelineHandler* pipelineHandler;
			IPipelineHandler* secPipelineHandler;

			/**<Is cleared at the start at each frame, contents can be fetched by GetErrorLogs*/
			std::vector<std::string> errorLog;

			DirectX::XMFLOAT4X4 newViewProjTransposed;

			struct OncePerFrameConstantBuffer
			{
				DirectX::XMFLOAT4X4 viewproj;
			};

			struct LightDataBuffer
			{
				DirectX::XMFLOAT4 size;
				LightData data[20];
			};

			int oncePerFrameBufferID;
			int lightBufferID = -1;
			int materialBufferID = -1;
			int cameraBufferID = -1;

			DeviceManager* device;
			ID3D11Device* dev;
			ID3D11DeviceContext* devContext;
			GraphicResourceHandler* graphicResourceHandler;
			MemoryMeasuring memMeasure;

			/***********General render jobs ***************/
			
			struct InternalRenderJob
			{
				RenderJob job;
				uint32_t jobID;
			};
			uint32_t jobIDCounter = 0;
			static const uint32_t JOB_GROUP_BITS = 4;
			static const uint32_t JOB_ID_BITS = 28;
			static const uint32_t JOB_ID_MASK = (1 << JOB_ID_BITS) - 1;
			static const uint32_t JOB_GROUP_MASK = (1 << JOB_GROUP_BITS) - 1;

			std::unordered_map<uint32_t, uint32_t> jobIDToIndex;
			std::map<uint8_t, std::vector<InternalRenderJob>> jobGroups;
			



			/***********End General Render Jobs************/

			/******** Instanced render job members ********/
			static const uint32_t maxDrawInstances = 256;
			int instancedTransformsConstantBufferHandle = -1;
			struct RenderBucket
			{
				RenderObjectInfo stateInfo;
				std::vector<DirectX::XMFLOAT4X4> transforms;
				/**<Whenever a job is removed the transform vector replaces the removed job's transform with the last added job's transform, as such we need a reverse lookup instead of iterating over all the jobs to find who had the bucket and transform index of the moved transform. The same index is used for this vector as for the transforms vector*/
				std::vector<uint32_t> jobsInBucket;
				std::vector<int> animationJob;
			};
			std::vector<RenderBucket> renderBuckets;
			struct BucketAndTransformIndex
			{
				uint32_t bucketIndex;
				uint32_t transformIndex;
				uint32_t animationIndex;
			};
			std::vector<BucketAndTransformIndex> jobIDToBucketAndTransformIndex;
			std::stack<uint32_t> freeJobIndices;
			void RenderABucket(const RenderBucket& bucket, const RenderObjectInfo& previousJob);
			/******** END Instanced render job members ********/

			/*********** Line render job members **************/
			std::vector<LineRenderJob> lineRenderJobs;
			std::stack<uint32_t> freeLineRenderJobIndices;
			int singleTransformConstantBuffer = -1;

			/*********** END Line render job members **********/
			

			/*********** Renderjob Queues **************/
			struct RenderJobStruct
			{
				uint32_t jobID;
				RenderObjectInfo handles;
			};
			struct UpdateTransformStruct
			{
				uint32_t jobID;
				DirectX::XMFLOAT4X4 transform;
			};

			std::mutex renderJobLock;
			Utilz::CircularFiFo<RenderJobStruct, 1024> newJobs;
			Utilz::CircularFiFo<uint32_t, 1024 > removeJobs;
			Utilz::CircularFiFo<RenderJobStruct, 1024> updateJobs;
			Utilz::CircularFiFo<UpdateTransformStruct, 1024> updateTransforms;

		
			void AddNewRenderJobs();		
			void UpdateRenderJobs();
			void UpdateTransforms();
			void RemoveRenderJobs();
		
			/*********** END Renderjob Queues **************/


			/*********** Lightjob **************/

			std::mutex lightLock;
			std::vector<LightData> renderLightJobs;
			DirectX::XMFLOAT4 newCameraPos = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 0.0);

			/*********** END Renderjob Queues **************/

			std::vector<RenderObjectInfo> renderJobs;
			std::vector<TextGUI> renderTextJobs;
			std::vector<GUITextureInfo> renderTextureJobs;
		
			//std::map<size_t, size_t> lightID;

			// fonts
			std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
			std::vector<DirectX::SpriteFont> fonts;

			static const int lightBufferSize = 20;


			/*********** Animation System **************/

			AnimationSystem* animationSystem;
			std::stack<int> freeAnimationJobIndicies;
			std::vector<AnimationJobInfo> jobIDToAnimationJob;


			/*********** End Animation System **************/

			bool running = false;
			std::thread myThread;

			void Frame();

			/*********** END Threading **************/

			/*********** Bloom **************/

			bool bloom = false;

			int bloomHorizontalHandle, bloomVerticalHandle;
			int bloomShaderResourceViewHandles[3];
			int bloomUnorderedAccessViewHandles[2];

			/********* END Bloom ************/

			Utilz::CPUTimeCluster cpuTimer;
			GPUTimeCluster* gpuTimer;
		};

	}
}
#endif //SE_GRAPHICS_RENDERER_H_