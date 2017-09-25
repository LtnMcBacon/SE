#ifndef SE_GRAPHICS_RENDEROBJECTINFO_H_
#define SE_GRAPHICS_RENDEROBJECTINFO_H_
#include <cstdint>
#include <bitset>
#include <algorithm>
#undef min //F you microsoft...
namespace SE
{
	namespace Graphics
	{
		struct RenderObjectInfo
		{
			enum class PrimitiveTopology : uint8_t
			{
				POINT_LIST,
				LINE_LIST,
				LINE_STRIP,
				TRIANGLE_LIST,
				TRIANGLE_STRIP
			};
			static const uint32_t maxTextureBinds = 4;
			int bufferHandle;
			int transformHandle;
			int pixelShader;
			int vertexShader;
			uint8_t textureBindings[maxTextureBinds];
			uint8_t textureHandles[maxTextureBinds];
			uint8_t textureCount;
			PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;
			/**
			* @brief Computes the difference between two RenderObjectInfo structs in terms of how many attributes are different. As such, a - b == b - a.
			* @details This operator is used to determine which render job to complete next, i.e. the render job that requires the least amount of state changes from the current one.
			* @param[in] rhs The other RenderObjectInfo to compare against
			* @retval Returns the number of differing attributes between the objects
			*/
			inline size_t operator-(const RenderObjectInfo& rhs) const
			{
				uint32_t stateChanges = 0;
				stateChanges = (stateChanges << 1) | (bufferHandle != rhs.bufferHandle);
				stateChanges = (stateChanges << 1) | (transformHandle != rhs.transformHandle);
				stateChanges = (stateChanges << 1) | (pixelShader != rhs.pixelShader);
				stateChanges = (stateChanges << 1) | (vertexShader != rhs.vertexShader);
				const uint32_t loopTo = std::min(textureCount, rhs.textureCount);
				for (uint32_t i = 0; i < loopTo; ++i)
				{
					stateChanges = (stateChanges << 1) | (textureBindings[i] != rhs.textureBindings[i]);
					stateChanges = (stateChanges << 1) | (textureHandles[i] != rhs.textureHandles[i]);
				}
				stateChanges = (stateChanges << 1) | (topology != rhs.topology);
				std::bitset<32> bits(stateChanges);
				return bits.count() + std::abs((int)textureCount - (int)rhs.textureCount);
			}
		};
	}
}

#endif

