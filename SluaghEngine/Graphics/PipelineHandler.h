#ifndef SE_GRAPHICS_PIPELINEHANDLER_H
#define SE_GRAPHICS_PIPELINEHANDLER_H
#include "IPipelineHandler.h"
#include <d3d11.h>
#include <map>
namespace SE
{
	namespace Graphics
	{
		class PipelineHandler : public IPipelineHandler
		{
		public:
			PipelineHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
			~PipelineHandler();
			PipelineHandler(const PipelineHandler& other) = delete;
			PipelineHandler(PipelineHandler&& other) = delete;
			PipelineHandler& operator=(const PipelineHandler& other) = delete;

			void CreateVertexBuffer(const Utilz::GUID& id, void* data, size_t vertexCount, size_t stride, bool dynamic = false) override;
			void DestroyVertexBuffer(const Utilz::GUID& id) override;
			void CreateIndexBuffer(const Utilz::GUID& id, void* data, size_t indexCount, size_t indexSize) override;
			void DestroyIndexBuffer(const Utilz::GUID& id) override;

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

			void SetPipeline(const Pipeline& pipeline) override;
		private:
			ID3D11Device* device;
			ID3D11DeviceContext* deviceContext;

			struct ConstantBuffer
			{
				ID3D11Buffer* buffer;
				int bindSlot;
			};
			

			std::map<Utilz::GUID, ID3D11Buffer*, Utilz::GUID::Compare> vertexBuffers;
			std::map<Utilz::GUID, ID3D11Buffer*, Utilz::GUID::Compare> indexBuffers;
			std::map<Utilz::GUID, ID3D11InputLayout*, Utilz::GUID::Compare> inputLayouts;
			std::map<Utilz::GUID, ID3D11VertexShader*, Utilz::GUID::Compare> vertexShaders;
			std::map<Utilz::GUID, ID3D11GeometryShader*, Utilz::GUID::Compare> geometryShaders;
			std::map<Utilz::GUID, ID3D11PixelShader*, Utilz::GUID::Compare> pixelShaders;
			std::map<Utilz::GUID, ID3D11ComputeShader*, Utilz::GUID::Compare> computeShaders;
			std::map<Utilz::GUID, ConstantBuffer, Utilz::GUID::Compare> constantBuffers;
			std::map<Utilz::GUID, ID3D11ShaderResourceView*, Utilz::GUID::Compare> shaderResourceViews;
			std::map<Utilz::GUID, ID3D11RenderTargetView*, Utilz::GUID::Compare> renderTargetViews;
			std::map<Utilz::GUID, ID3D11SamplerState*, Utilz::GUID::Compare> samplerStates;
			std::map<Utilz::GUID, ID3D11BlendState*, Utilz::GUID::Compare> blendStates;
			std::map<Utilz::GUID, ID3D11RasterizerState*, Utilz::GUID::Compare> rasterizerStates;
			std::map<Utilz::GUID, ID3D11DepthStencilState*, Utilz::GUID::Compare> depthStencilStates;
			/**<Key is evaluated by (GUID(pixelShader) + GUID(resourceBindingName))*/
			std::map<Utilz::GUID, int, Utilz::GUID::Compare> pixelShaderAndResourceNameToBindSlot;


		};
	}
}


#endif
