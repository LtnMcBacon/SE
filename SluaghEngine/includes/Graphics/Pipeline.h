#ifndef SE_GRAPHICS_PIPELINE_H_
#define SE_GRAPHICS_PIPELINE_H_
#include <cstdint>
#include <Utilz/GUID.h>
namespace SE
{
	namespace Graphics
	{
		enum class CullMode
		{
			CULL_FRONT,
			CULL_BACK,
			CULL_NONE
		};
		enum class WindingOrder
		{
			CLOCKWISE,
			COUNTERCLOCKWISE
		};
		enum class FillMode
		{
			FILL_SOLID,
			FILL_WIREFRAME
		};
		struct RasterizerState
		{
			CullMode cullMode;
			WindingOrder windingOrder;
			FillMode fillMode;
		};

		enum class BlendOperation
		{
			ADD,
			SUB,
			MAX,
			MIN
		};

		enum class Blend
		{
			ZERO,
			ONE,
			SRC_COLOR,
			INV_SRC_COLOR,
			SRC_ALPHA,
			INV_SRC_ALPHA,
			DEST_ALPHA,
			INV_DEST_ALPHA,
			DEST_COLOR,
			INV_DEST_COLOR,
			BLEND_FACTOR
		};

		struct BlendState
		{
			bool enable;
			BlendOperation blendOperation;
			BlendOperation blendOperationAlpha;
			Blend srcBlend;
			Blend dstBlend;
			Blend srcBlendAlpha;
			Blend dstBlendAlpha;
		};

		enum class ComparisonOperation
		{
			LESS,
			LESS_EQUAL,
			EQUAL,
			GREATER,
			GREATER_EQUAL,
			NO_COMPARISON
		};

		struct DepthStencilState
		{
			bool enableDepth;
			bool writeDepth;
			ComparisonOperation comparisonOperation;
		};
		enum class AddressingMode
		{
			WRAP,
			MIRROR,
			CLAMP
		};
		enum class Filter
		{
			ANISOTROPIC,
			LINEAR,
			POINT
		};
		struct SamplerState
		{
			AddressingMode addressU;
			AddressingMode addressV;
			AddressingMode addressW;
			Filter filter;
			int maxAnisotropy;
		};

		enum class TextureFormat
		{
			R32G32B32A32_FLOAT,
			R8G8B8A8_UNORM
		};

		struct RenderTarget
		{
			bool bindAsShaderResource;
			int width;
			int height;
			TextureFormat format;

		};
		enum class PrimitiveTopology : uint8_t
		{
			NONE,
			POINT_LIST,
			LINE_LIST,
			LINE_STRIP,
			TRIANGLE_LIST,
			TRIANGLE_STRIP
		};
		enum BufferFlags
		{
			BIND_VERTEX = 1 << 0,
			BIND_INDEX = 1 << 1,
			BIND_CONSTANT = 1 << 2,
			BIND_STREAMOUT = 1 << 3,
			CPU_READ = 1 << 5,
			CPU_WRITE = 1 << 6
		};
		struct InputAssemblerStage
		{
			Utilz::GUID vertexBuffer;
			Utilz::GUID indexBuffer;
			PrimitiveTopology topology;
			Utilz::GUID inputLayout;
		};

		struct VertexShaderStage
		{
			static const size_t maxConstantBuffers = 8;
			Utilz::GUID shader;
			Utilz::GUID constantBuffers[maxConstantBuffers];
			uint8_t constantBufferCount = 0;
		};

		struct GeometryShaderStage
		{
			static const size_t maxConstantBuffers = 8;
			Utilz::GUID shader;
			Utilz::GUID constantBuffers[maxConstantBuffers];
			uint8_t constantBufferCount = 0;
		};

		struct StreamOutStage
		{
			Utilz::GUID streamOutTarget;
		};

		struct PixelShaderStage
		{
			static const size_t maxConstantBuffers = 8;
			static const size_t maxTextures = 8;
			static const size_t maxSamplers = 8;
			Utilz::GUID shader;
			Utilz::GUID constantBuffers[maxConstantBuffers];
			Utilz::GUID textures[maxTextures];
			Utilz::GUID textureBindings[maxTextures];
			Utilz::GUID samplers[maxSamplers];			
			uint8_t constantBufferCount = 0;
			uint8_t textureCount = 0;
			uint8_t samplerCount = 0;
			
		};

		struct OutputMergerStage
		{
			static const size_t maxRenderTargets = 8;
			Utilz::GUID blendState;
			Utilz::GUID depthStencilState;
			Utilz::GUID renderTargets[maxRenderTargets];
			Utilz::GUID depthStencilView;
			uint8_t renderTargetCount = 0;
		};
		struct Viewport
		{
			float width;
			float height;
			float maxDepth;
			float minDepth;
			float topLeftX;
			float topLeftY;
		};

		struct RasterizerStage
		{
			Utilz::GUID rasterizerState;
			Utilz::GUID viewport;
		};

		struct Pipeline
		{
			InputAssemblerStage IAStage;
			VertexShaderStage	VSStage;
			GeometryShaderStage	GSStage;
			StreamOutStage		SOStage;
			RasterizerStage		RStage;
			PixelShaderStage	PSStage;
			OutputMergerStage	OMStage;
		};
	}
}


#endif
