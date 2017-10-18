#ifndef SE_GRAPHICS_PIPELINE_H_
#define SE_GRAPHICS_PIPELINE_H_
#include <cstdint>
#include <Utilz/GUID.h>
namespace SE
{
	namespace Graphics
	{
		enum class PrimitiveTopology : uint8_t
		{
			POINT_LIST,
			LINE_LIST,
			LINE_STRIP,
			TRIANGLE_LIST,
			TRIANGLE_STRIP,
			NONE
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
			uint8_t constantBufferCount;
		};

		struct GeometryShaderStage
		{
			static const size_t maxConstantBuffers = 8;
			Utilz::GUID shader;
			Utilz::GUID constantBuffers[maxConstantBuffers];
			uint8_t constantBufferCount;
		};

		struct PixelShaderStage
		{
			static const size_t maxConstantBuffers = 8;
			static const size_t maxRenderTargets = 8;
			static const size_t maxTextures = 8;
			static const size_t maxSamplers = 8;
			Utilz::GUID shader;
			Utilz::GUID constantBuffers[maxConstantBuffers];
			Utilz::GUID textures[maxTextures]; //bindslots correspond to the index in the array
			Utilz::GUID samplers[maxSamplers];
			Utilz::GUID renderTargets[maxRenderTargets];
			uint8_t constantBufferCount;
			uint8_t textureCount;
			uint8_t samplerCount;
			uint8_t renderTargetCount;	
		};

		struct OutputMergerStage
		{
			Utilz::GUID rasterizerState;
			Utilz::GUID blendState;
			Utilz::GUID depthStencilState;
		};

		struct Pipeline
		{
			InputAssemblerStage IAStage;
			VertexShaderStage	VSStage;
			GeometryShaderStage	GSStage;
			PixelShaderStage	PSStage;
			OutputMergerStage	OMStage;
		};
	}
}


#endif
