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

			virtual void CreateVertexBuffer(const Utilz::GUID& id, void* data, size_t vertexCount, size_t stride, bool dynamic = false) = 0;
			virtual void CreateIndexBuffer(const Utilz::GUID& id, void* data, size_t indexCount, size_t indexSize) = 0;
			virtual void CreateBuffer(const Utilz::GUID& id, void* data, size_t elementCount, size_t elementStride, size_t maxElements, uint32_t flags) = 0;
			virtual void DestroyIndexBuffer(const Utilz::GUID& id) = 0;
			virtual void DestroyVertexBuffer(const Utilz::GUID& id) = 0;

			virtual void CreateVertexShader(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual void CreateGeometryShader(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual void CreateGeometryShaderStreamOut(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual void CreatePixelShader(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual void CreateComputeShader(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual void DestroyVertexShader(const Utilz::GUID& id) = 0;
			virtual void DestroyGeometryShader(const Utilz::GUID& id) = 0;
			virtual void DestroyPixelShader(const Utilz::GUID& id) = 0;
			virtual void DestroyComputeShader(const Utilz::GUID& id) = 0;

			virtual void CreateConstantBuffer(const Utilz::GUID& id, size_t size, void* initialData = nullptr) = 0;
			virtual void UpdateConstantBuffer(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual void DestroyConstantBuffer(const Utilz::GUID& id) = 0;

			virtual void CreateTexture(const Utilz::GUID& id, void* data, size_t width, size_t height) = 0;
			virtual void DestroyTexture(const Utilz::GUID& id) = 0;

			virtual void CreateRasterizerState(const Utilz::GUID& id, const RasterizerState& state) = 0;
			virtual void DestroyRasterizerState(const Utilz::GUID& id) = 0;

			virtual void CreateBlendState(const Utilz::GUID& id, const BlendState& state) = 0;
			virtual void DestroyBlendState(const Utilz::GUID& id) = 0;

			virtual void CreateDepthStencilState(const Utilz::GUID& id, const DepthStencilState& state) = 0;
			virtual void DestroyDepthStencilState(const Utilz::GUID& id) = 0;

			virtual void CreateSamplerState(const Utilz::GUID& id, const SamplerState& state) = 0;
			virtual void DestroySamplerState(const Utilz::GUID& id) = 0;

			virtual void CreateRenderTarget(const Utilz::GUID& id, const RenderTarget& target) = 0;
			virtual void DestroyRenderTarget(const Utilz::GUID& id) = 0;

			virtual void CreateDepthStencilView(const Utilz::GUID& id, size_t width, size_t height, bool bindAsTexture = false) = 0;
			virtual void DestroyDepthStencilView(const Utilz::GUID& id) = 0;

			virtual void SetPipeline(const Pipeline& pipeline) = 0;

		};
	}
}


#endif
