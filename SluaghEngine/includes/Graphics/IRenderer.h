#ifndef _IRENDERER_H_
#define _IRENDERER_H_

#include <cstdint>
#include "TextureDesc.h"
#include <Graphics\GUIInfo.h>
#include <Utilz\TimeCluster.h>
#include "IPipelineHandler.h"
#include "RenderJob.h"
#include <functional>

#if defined DLL_EXPORT_RENDERER
#define DECLDIR_R __declspec(dllexport)
#else
#define DECLDIR_R __declspec(dllimport)
#endif


namespace SE
{
	namespace Graphics
	{
		struct InitializationInfo
		{
			void* window;
			size_t maxVRAMUsage = 256u*1024u*1024u;
		};
		class IRenderer
		{
		public:
			virtual ~IRenderer() {};

			/**
			* @brief Initialize the renderer
			* @param[in] initInfo Initalization info container.
			* @retval 0 On success.
			* @retval other See HRESULT
			*/
			virtual int Initialize(const InitializationInfo& initInfo) = 0;

			/**
			* @brief Shutdown the renderer
			*/
			virtual void Shutdown() = 0;

			virtual IPipelineHandler* GetPipelineHandler() = 0;
			virtual IPipelineHandler* GetSecondaryPipelineHandler() = 0;

			/**
			* @brief Adds a renderjob to be rendered, is rendered until RemoveRenderJob is called
			* @param[in] job Struct containing all information required to render.
			* @param[in] group The RenderGroup (enum) the job should belong to.
			* @retval Returns a handle to the job on success.
			* @retval -1 on failure.
			* @sa RenderJob, RenderGroup
			*/
			virtual uint32_t AddRenderJob(const RenderJob& job, RenderGroup group) = 0;

			/**
			* @brief Removes a renderjob that has been added by AddRenderJob
			* @param[in] jobID The ID retrieved from AddRenderJob
			* @sa AddRenderJob
			*/
			virtual void RemoveRenderJob(uint32_t jobID) = 0;

			/**
			* @brief Replaces an existing renderjob with a new one. The job must have been added by AddRenderJob.
			* @param[in] jobID The ID retrieved from AddRenderJob
			* @param[in] newJob The job to replace the existing job with.
			* @sa AddRenderJob
			*/
			virtual void ChangeRenderJob(uint32_t jobID, const RenderJob& newJob) = 0;

			/**
			* @brief Allow for modifying an existing renderjob. The job must have been added by AddRenderJob.
			* @param[in] jobID The ID retrieved from AddRenderJob
			* @param[in] callback The callback function where you can change the job.
			* @sa AddRenderJob
			*/
			virtual void ChangeRenderJob(uint32_t jobID, const std::function<void(RenderJob& job)>& callback) = 0;

			/**
			* @brief    Sets a Text render jobs
			* @param[in] handles The handles struct
			* @retval jobID On success.
			*/
			virtual size_t EnableTextRendering(const TextGUI & handles) = 0;

			/**
			* @brief    Removes a Text render job.
			* @param[in] jobID The ID of the job to disable.
			* @retval jobID On success.
			*/
			virtual size_t DisableTextRendering(const size_t & jobID) = 0;

			/**
			* @brief    Sets Text render jobs
			* @param[in] handles The handles struct
			* @retval jobID On success.
			*/
			virtual size_t EnableTextureRendering(const GUIJob & handles) = 0;
			
			/**
			* @brief    Removes a Text render job.
			* @param[in] jobID The ID of the job to disable.
			* @retval jobID On success.
			*/
			virtual size_t DisableTextureRendering(const size_t & jobID) = 0;

			/**
			* @brief Renders the scene
			* @retval 0 On success.
			*/
			virtual int Render() = 0;

			/**
			* @brief Begins the frame. Clears the render target view.
			* @retval 0 On success.
			*/
			virtual int BeginFrame() = 0;

			/*
			* @brief Ends the frame. Presents the rendered scene to the screen.
			* @retval 0 On success.
			*/
			virtual int EndFrame() = 0;

			/*
			* @brief If the implementation of the renderer is DirectX11, destination points to a struct with a member for a pointer to the ID3D11Device as well as a pointer to the ID3D11DeviceContext
			* @details This method is needed to instanciate third party libraries such as Imgui which needs to access the device and device context
			* @param[in] destination A pointer to a struct that can hold whatever info Imgui needs. In the case of DX11, the struct has the structure {ID3D11Device*, ID3D11DeviceContext*}
			* @param[in] size The size of the struct, used to validate that the void ptr points to a struct of sufficient size.
			*/
			virtual void GetDeviceInfo(void* destination, size_t size) = 0;

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
			* @brief Create a new font
			* @retval 0+ Font ID
			* @retval -1 Something went wrong.
			*/
			virtual int CreateTextFont(void * data, size_t size) = 0;
			/**
			* @brief Resizes the swapchain
			* @param[in] windowHandle A window handle.
			*/
			virtual void ResizeSwapChain(void* windowHandle) = 0;
			/**
			* @brief	The amount of VRam currently used.
			* @retval size_t The amount of VRam used in bytes.
			*/
			virtual size_t GetVRam() = 0;

			/**
			* @brief Checks if there is enough free VRAM to allocate a resource.
			* @param sizeToAdd The size of the new resource.
			*
			* @retval true If there is enough VRAM to allocate this resource.
			* @retval true If there is not enough VRAM to allocate this resource.
			*
			*/
			virtual bool IsUnderLimit(size_t sizeToAdd) = 0;

			/**
			* @brief Checks if there is enough free VRAM to allocate a resource with a potential unallocated amount.
			* @param potential The size of resources that can be unloaded to make room.
			* @param sizeToAdd The size of the new resource.
			*
			* @retval true If there is enough VRAM to allocate this resource.
			* @retval true If there is not enough VRAM to allocate this resource.
			*
			*/
			virtual bool IsUnderLimit(size_t potential, size_t sizeToAdd) = 0;

			/*
			 * @brief Saves the current error log in the parameter. The vector can only be used until the next call to BeginFrame if it is stored as a reference.
			 */
			virtual std::vector<std::string>& GetErrorLog() = 0;

			/**
			* @brief	Return a map of with profiling information.
			*
			*/
			virtual void GetProfilingInformation(Utilz::TimeMap& map) = 0;

		protected:
			IRenderer() {};
			IRenderer(const IRenderer& other) = delete;
			IRenderer(const IRenderer&& other) = delete;
			IRenderer& operator=(const IRenderer& rhs) = delete;
		};

		DECLDIR_R IRenderer* CreateRenderer();

	}
}
#endif
