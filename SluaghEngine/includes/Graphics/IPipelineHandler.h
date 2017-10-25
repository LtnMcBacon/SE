#ifndef SE_GRAPHICS_IPIPELINEHANDLER_H_
#define SE_GRAPHICS_IPIPELINEHANDLER_H_
#include "Pipeline.h"

namespace SE
{
	namespace Graphics
	{
		class IPipelineHandler
		{
		public:
			virtual ~IPipelineHandler() = 0 {};

			virtual int CreateVertexBuffer(const Utilz::GUID& id, void* data, size_t vertexCount, size_t stride, bool dynamic = false) = 0;
			virtual int UpdateDynamicVertexBuffer(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual int CreateIndexBuffer(const Utilz::GUID& id, void* data, size_t indexCount, size_t indexSize) = 0;
			virtual int CreateBuffer(const Utilz::GUID& id, void* data, size_t elementCount, size_t elementStride, size_t maxElements, uint32_t flags) = 0;
			virtual int DestroyIndexBuffer(const Utilz::GUID& id) = 0;
			virtual int DestroyVertexBuffer(const Utilz::GUID& id) = 0;

			virtual int CreateVertexShader(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual int CreateGeometryShader(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual int CreateGeometryShaderStreamOut(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual int CreatePixelShader(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual int CreateComputeShader(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual int DestroyVertexShader(const Utilz::GUID& id) = 0;
			virtual int DestroyGeometryShader(const Utilz::GUID& id) = 0;
			virtual int DestroyPixelShader(const Utilz::GUID& id) = 0;
			virtual int DestroyComputeShader(const Utilz::GUID& id) = 0;

			virtual int CreateConstantBuffer(const Utilz::GUID& id, size_t size, void* initialData = nullptr) = 0;
			virtual int UpdateConstantBuffer(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual int DestroyConstantBuffer(const Utilz::GUID& id) = 0;

			virtual int CreateTexture(const Utilz::GUID& id, void* data, size_t width, size_t height) = 0;
			virtual int DestroyTexture(const Utilz::GUID& id) = 0;

			virtual int CreateRasterizerState(const Utilz::GUID& id, const RasterizerState& state) = 0;
			virtual int DestroyRasterizerState(const Utilz::GUID& id) = 0;

			virtual int CreateBlendState(const Utilz::GUID& id, const BlendState& state) = 0;
			virtual int DestroyBlendState(const Utilz::GUID& id) = 0;

			virtual int CreateDepthStencilState(const Utilz::GUID& id, const DepthStencilState& state) = 0;
			virtual int DestroyDepthStencilState(const Utilz::GUID& id) = 0;

			virtual int CreateSamplerState(const Utilz::GUID& id, const SamplerState& state) = 0;
			virtual int DestroySamplerState(const Utilz::GUID& id) = 0;

			virtual int CreateRenderTarget(const Utilz::GUID& id, const RenderTarget& target) = 0;
			virtual int DestroyRenderTarget(const Utilz::GUID& id) = 0;

			virtual int CreateDepthStencilView(const Utilz::GUID& id, size_t width, size_t height, bool bindAsTexture = false) = 0;
			virtual int DestroyDepthStencilView(const Utilz::GUID& id) = 0;

			virtual void SetPipeline(const Pipeline& pipeline) = 0;
			/*
			 * @brief Does not compare the current pipeline with pipeline. Sets all states even if those states might already be set. Only use this for the first pipeline to set.
			 * @param[in] pipeline The pipeline to set.
			 */
			virtual void SetPipelineForced(const Pipeline& pipeline) = 0;

		};
	}
}


#endif