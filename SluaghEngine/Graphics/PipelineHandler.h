#ifndef SE_GRAPHICS_PIPELINEHANDLER_H
#define SE_GRAPHICS_PIPELINEHANDLER_H
#include "IPipelineHandler.h"
#include <d3d11.h>
#include <map>
#include <unordered_map>
namespace SE
{
	namespace Graphics
	{
		class PipelineHandler : public IPipelineHandler
		{
		public:
			PipelineHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11RenderTargetView* backbuffer);
			~PipelineHandler();
			PipelineHandler(const PipelineHandler& other) = delete;
			PipelineHandler(PipelineHandler&& other) = delete;
			PipelineHandler& operator=(const PipelineHandler& other) = delete;

			void CreateVertexBuffer(const Utilz::GUID& id, void* data, size_t vertexCount, size_t stride, bool dynamic = false) override;
			void CreateIndexBuffer(const Utilz::GUID& id, void* data, size_t indexCount, size_t indexSize) override;
			void CreateBuffer(const Utilz::GUID& id, void* data, size_t elementCount, size_t elementStride, uint32_t flags) override;
			void DestroyIndexBuffer(const Utilz::GUID& id) override;
			void DestroyVertexBuffer(const Utilz::GUID& id) override;

			void CreateViewport(const Utilz::GUID& id, const Viewport& viewport);

			void CreateVertexShader(const Utilz::GUID& id, void* data, size_t size) override;
			void CreateGeometryShader(const Utilz::GUID& id, void* data, size_t size) override;
			void CreatePixelShader(const Utilz::GUID& id, void* data, size_t size) override;
			void CreateComputeShader(const Utilz::GUID& id, void* data, size_t size) override;
			void DestroyVertexShader(const Utilz::GUID& id) override;
			void DestroyGeometryShader(const Utilz::GUID& id) override;
			void DestroyPixelShader(const Utilz::GUID& id) override;
			void DestroyComputeShader(const Utilz::GUID& id) override;

			void CreateConstantBuffer(const Utilz::GUID& id, size_t size, void* initialData = nullptr) override;
			void UpdateConstantBuffer(const Utilz::GUID& id, void* data, size_t size) override;
			void DestroyConstantBuffer(const Utilz::GUID& id) override;

			void CreateTexture(const Utilz::GUID& id, void* data, size_t width, size_t height) override;
			void DestroyTexture(const Utilz::GUID& id) override;

			void CreateRasterizerState(const Utilz::GUID& id, const RasterizerState& state) override;
			void DestroyRasterizerState(const Utilz::GUID& id) override;

			void CreateBlendState(const Utilz::GUID& id, const BlendState& state) override;
			void DestroyBlendState(const Utilz::GUID& id) override;

			void CreateDepthStencilState(const Utilz::GUID& id, const DepthStencilState& state) override;
			void DestroyDepthStencilState(const Utilz::GUID& id) override;

			void CreateSamplerState(const Utilz::GUID& id, const SamplerState& state) override;
			void DestroySamplerState(const Utilz::GUID& id) override;

			void CreateRenderTarget(const Utilz::GUID& id, const RenderTarget& target) override;
			void DestroyRenderTarget(const Utilz::GUID& id) override;

			void CreateDepthStencilView(const Utilz::GUID& id, size_t width, size_t height, bool bindAsTexture = false) override;
			void DestroyDepthStencilView(const Utilz::GUID& id) override;

			void SetPipeline(const Pipeline& pipeline) override;
		private:

			void SetInputAssemblerStage(const InputAssemblerStage& pIA);
			void SetVertexShaderStage(const VertexShaderStage& vss);
			void SetGeometryShaderStage(const GeometryShaderStage& gss);
			void SetRasterizerStage(const RasterizerStage& rs);
			void SetPixelShaderStage(const PixelShaderStage& pss);
			void SetOutputMergerStage(const OutputMergerStage& oms);

			ID3D11Device* device;
			ID3D11DeviceContext* deviceContext;

			struct VertexBuffer
			{
				ID3D11Buffer* buffer;
				uint32_t stride;
			};
			struct IndexBuffer
			{
				ID3D11Buffer* buffer;
				uint32_t stride;
			};
			Pipeline currentPipeline;

			std::unordered_map<Utilz::GUID, VertexBuffer, Utilz::GUID::Hasher> vertexBuffers;
			std::unordered_map<Utilz::GUID, IndexBuffer, Utilz::GUID::Hasher> indexBuffers;
			std::unordered_map<Utilz::GUID, ID3D11InputLayout*, Utilz::GUID::Hasher> inputLayouts;
			std::unordered_map<Utilz::GUID, ID3D11VertexShader*, Utilz::GUID::Hasher> vertexShaders;
			std::unordered_map<Utilz::GUID, ID3D11GeometryShader*, Utilz::GUID::Hasher> geometryShaders;
			std::unordered_map<Utilz::GUID, ID3D11PixelShader*, Utilz::GUID::Hasher> pixelShaders;
			std::unordered_map<Utilz::GUID, ID3D11ComputeShader*, Utilz::GUID::Hasher> computeShaders;
			std::unordered_map<Utilz::GUID, ID3D11Buffer*, Utilz::GUID::Hasher> constantBuffers;
			std::unordered_map<Utilz::GUID, ID3D11ShaderResourceView*, Utilz::GUID::Hasher> shaderResourceViews;
			std::unordered_map<Utilz::GUID, ID3D11RenderTargetView*, Utilz::GUID::Hasher> renderTargetViews;
			std::unordered_map<Utilz::GUID, ID3D11DepthStencilView*, Utilz::GUID::Hasher> depthStencilViews;
			std::unordered_map<Utilz::GUID, ID3D11SamplerState*, Utilz::GUID::Hasher> samplerStates;
			std::unordered_map<Utilz::GUID, ID3D11BlendState*, Utilz::GUID::Hasher> blendStates;
			std::unordered_map<Utilz::GUID, ID3D11RasterizerState*, Utilz::GUID::Hasher> rasterizerStates;
			std::unordered_map<Utilz::GUID, D3D11_VIEWPORT, Utilz::GUID::Hasher> viewports;
			std::unordered_map<Utilz::GUID, ID3D11DepthStencilState*, Utilz::GUID::Hasher> depthStencilStates;
			/**<Key is evaluated by (GUID(shader) + GUID(resourceBindingName))*/
			std::unordered_map<Utilz::GUID, int, Utilz::GUID::Hasher> shaderAndResourceNameToBindSlot;

		};
	}
}


#endif
