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
			virtual ~IPipelineHandler() = 0;

			virtual void CreateVertexBuffer(const Utilz::GUID& id, void* data, size_t vertexCount, size_t stride) = 0;
			virtual void DestroyVertexBuffer(const Utilz::GUID& id) = 0;
			virtual void CreateIndexBuffer(const Utilz::GUID& id, void* data, size_t indexCount, size_t indexSize) = 0;

			virtual void CreateVertexShader(const Utilz::GUID& id, void* data, size_t size) = 0;
			virtual void DestroyVertexShader(const Utilz::GUID& id) = 0;

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

		};
	}
}


#endif
