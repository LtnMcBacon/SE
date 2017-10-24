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
			PipelineHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11RenderTargetView* backbuffer, ID3D11DepthStencilView* dsv);
			~PipelineHandler();
			PipelineHandler(const PipelineHandler& other) = delete;
			PipelineHandler(PipelineHandler&& other) = delete;
			PipelineHandler& operator=(const PipelineHandler& other) = delete;

			int CreateVertexBuffer(const Utilz::GUID& id, void* data, size_t vertexCount, size_t stride, bool dynamic = false) override;
			int UpdateDynamicVertexBuffer(const Utilz::GUID& id, void* data, size_t size) override;
			int CreateIndexBuffer(const Utilz::GUID& id, void* data, size_t indexCount, size_t indexSize) override;
			int CreateBuffer(const Utilz::GUID& id, void* data, size_t elementCount, size_t elementStride, size_t maxElements, uint32_t flags) override;
			int DestroyIndexBuffer(const Utilz::GUID& id) override;
			int DestroyVertexBuffer(const Utilz::GUID& id) override;

			int CreateViewport(const Utilz::GUID& id, const Viewport& viewport);

			int CreateVertexShader(const Utilz::GUID& id, void* data, size_t size) override;
			int CreateGeometryShader(const Utilz::GUID& id, void* data, size_t size) override;
			int CreateGeometryShaderStreamOut(const Utilz::GUID& id, void* data, size_t size) override;
			int CreatePixelShader(const Utilz::GUID& id, void* data, size_t size) override;
			int CreateComputeShader(const Utilz::GUID& id, void* data, size_t size) override;
			int DestroyVertexShader(const Utilz::GUID& id) override;
			int DestroyGeometryShader(const Utilz::GUID& id) override;
			int DestroyPixelShader(const Utilz::GUID& id) override;
			int DestroyComputeShader(const Utilz::GUID& id) override;

			int CreateConstantBuffer(const Utilz::GUID& id, size_t size, void* initialData = nullptr) override;
			int UpdateConstantBuffer(const Utilz::GUID& id, void* data, size_t size) override;
			int DestroyConstantBuffer(const Utilz::GUID& id) override;

			int CreateTexture(const Utilz::GUID& id, void* data, size_t width, size_t height) override;
			int DestroyTexture(const Utilz::GUID& id) override;

			int CreateRasterizerState(const Utilz::GUID& id, const RasterizerState& state) override;
			int DestroyRasterizerState(const Utilz::GUID& id) override;

			int CreateBlendState(const Utilz::GUID& id, const BlendState& state) override;
			int DestroyBlendState(const Utilz::GUID& id) override;

			int CreateDepthStencilState(const Utilz::GUID& id, const DepthStencilState& state) override;
			int DestroyDepthStencilState(const Utilz::GUID& id) override;

			int CreateSamplerState(const Utilz::GUID& id, const SamplerState& state) override;
			int DestroySamplerState(const Utilz::GUID& id) override;

			int CreateRenderTarget(const Utilz::GUID& id, const RenderTarget& target) override;
			int DestroyRenderTarget(const Utilz::GUID& id) override;

			int CreateDepthStencilView(const Utilz::GUID& id, size_t width, size_t height, bool bindAsTexture = false) override;
			int DestroyDepthStencilView(const Utilz::GUID& id) override;

			void SetPipeline(const Pipeline& pipeline) override;
			void SetPipelineForced(const Pipeline& pipeline) override;
		private:

			void SetInputAssemblerStage(const InputAssemblerStage& pIA);
			void SetVertexShaderStage(const ShaderStage& vss);
			void SetGeometryShaderStage(const ShaderStage& gss);
			void SetRasterizerStage(const RasterizerStage& rs);
			void SetPixelShaderStage(const ShaderStage& pss);
			void SetOutputMergerStage(const OutputMergerStage& oms);

			void ForcedSetInputAssemblerStage(const InputAssemblerStage& pIA);
			void ForcedSetVertexShaderStage(const ShaderStage& vss);
			void ForcedSetGeometryShaderStage(const ShaderStage& gss);
			void ForcedSetRasterizerStage(const RasterizerStage& rs);
			void ForcedSetPixelShaderStage(const ShaderStage& pss);
			void ForcedSetOutputMergerStage(const OutputMergerStage& oms);

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
