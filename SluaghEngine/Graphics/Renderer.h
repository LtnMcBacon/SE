#ifndef SE_GRAPHICS_RENDERER_H_
#define SE_GRAPHICS_RENDERER_H_
#include <IRenderer.h>
#include "DeviceManager.h"
#include "MemoryMeasuring.h"
#include "GPUTimeCluster.h"
#include "GraphicResourceHandler.h"
#include <Utilz\CPUTimeCluster.h>
#include <ToolKit\SpriteFont.h>
#include <ToolKit\SpriteBatch.h>


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
			* @brief Allow for modifying an existing renderjob. The job must have been added by AddRenderJob.
			* @param[in] jobID The ID retrieved from AddRenderJob
			* @param[in] callback The callback function where you can change the job.
			* @sa AddRenderJob
			*/
			void ChangeRenderJob(uint32_t jobID, const std::function<void(RenderJob& job)>& callback) override;

			/**
			* @brief    Sets Text render jobs
			* @param[in] handles The handles struct
			* @retval jobID On success.
			* @endcode
			*/
			size_t EnableTextRendering(const TextJob & handles) override;

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
			size_t EnableTextureRendering(const GUIJob & handles) override;

			/**
			* @brief    Removes a Text render job.
			* @param[in] handles The handles struct
			* @retval jobID On success.
			* @endcode
			*/
			size_t DisableTextureRendering(const size_t & jobID) override;


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
			* @brief Creates a texture (SRV)
			* @param[in] data Raw image data
			* @param[in] description Texture description
			* @sa TextureDesc
			*
			*/
			int CreateTexture(void* data, const TextureDesc& description) override;
		
			/**
			* @brief Create a new fomt
			* @retval 0+ Font ID
			* @retval -1 Something went wrong.
			* @endcode
			*/
			int CreateTextFont(void * data, size_t size) override;/**
			* @brief Resizes the swapchain
			* @param[in] windowHandle A window handle.
			* @endcode
			*/
			void ResizeSwapChain(void* windowHandle) override;
			/*
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

		private:
			Renderer(const Renderer& other) = delete;
			Renderer(const Renderer&& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;

			InitializationInfo initInfo;

			IPipelineHandler* pipelineHandler;
			IPipelineHandler* secPipelineHandler;

			/**<Is cleared at the start at each frame, contents can be fetched by GetErrorLogs*/
			std::vector<std::string> errorLog;

		
			DeviceManager* device;
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


			struct InternalTextJob
			{
				TextJob job;
				RECT rect;
				DirectX::XMFLOAT2 origin;
				DirectX::XMFLOAT2 scale;
				size_t index;
			};
			std::vector<InternalTextJob> renderTextJobs;
			std::vector<size_t> textJobToIndex;
			std::stack<size_t> freeTextJobs;

			std::vector<GUIJob> renderTextureJobs;
	
			// fonts
			DirectX::SpriteBatch* spriteBatch;
			std::vector<DirectX::SpriteFont> fonts;

			///*********** Bloom **************/

			//bool bloom = false;

			//int bloomHorizontalHandle = -1;
			//int bloomVerticalHandle = -1;
			//int bloomShaderResourceViewHandles[3] = { -1, -1, -1 };
			//int bloomUnorderedAccessViewHandles[2] = { -1, -1 };

			/********* END Bloom ************/

			Utilz::CPUTimeCluster cpuTimer;
			GPUTimeCluster* gpuTimer;
		};

	}
}
#endif //SE_GRAPHICS_RENDERER_H_