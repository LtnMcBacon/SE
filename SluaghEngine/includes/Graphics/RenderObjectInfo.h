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
				TRIANGLE_STRIP,
				NONE
			};

			enum class JobType : uint32_t 
			{
				STATIC = 1,
				SKINNED = 1 << 1
			};

			static const uint32_t maxTextureBinds = 4;
			int bufferHandle;
			int pixelShader;
			int vertexShader;
			int8_t textureBindings[maxTextureBinds];
			int8_t textureHandles[maxTextureBinds];
			uint8_t textureCount;
			PrimitiveTopology topology = PrimitiveTopology::TRIANGLE_LIST;
			JobType type;
			uint8_t skeletonHandle;
			uint8_t fillSolid;
			uint8_t transparency;
			RenderObjectInfo()
			{
				bufferHandle = -1;
				pixelShader = -1;
				vertexShader = -1;
				type = JobType::STATIC;
				skeletonHandle = 0;
				for (int i = 0; i < maxTextureBinds; ++i)
				{
					textureBindings[i] = -1;
					textureHandles[i] = -1;
				}
				textureCount = 0;
				topology = PrimitiveTopology::TRIANGLE_LIST;
				fillSolid = 1;
				transparency = 0;
			}
			inline RenderObjectInfo& operator=(const RenderObjectInfo& rhs)
			{
				bufferHandle = rhs.bufferHandle;
				pixelShader = rhs.pixelShader;
				vertexShader = rhs.vertexShader;
				type = rhs.type;
				skeletonHandle = rhs.skeletonHandle;
				for(int i = 0; i < maxTextureBinds; ++i)
				{
					textureBindings[i] = rhs.textureBindings[i];
					textureHandles[i] = rhs.textureHandles[i];
				}
				textureCount = rhs.textureCount;
				topology = rhs.topology;
				fillSolid = rhs.fillSolid;
				transparency = rhs.transparency;
				return *this;
			}
			RenderObjectInfo(const RenderObjectInfo& rhs)
			{
				bufferHandle = rhs.bufferHandle;
				pixelShader = rhs.pixelShader;
				vertexShader = rhs.vertexShader;
				type = rhs.type;
				skeletonHandle = rhs.skeletonHandle;
				for (int i = 0; i < maxTextureBinds; ++i)
				{
					textureBindings[i] = rhs.textureBindings[i];
					textureHandles[i] = rhs.textureHandles[i];
				}
				textureCount = rhs.textureCount;
				topology = rhs.topology;
				fillSolid = rhs.fillSolid;
				transparency = rhs.transparency;
			}
			RenderObjectInfo(RenderObjectInfo&& rhs)
			{
				bufferHandle = rhs.bufferHandle;
				pixelShader = rhs.pixelShader;
				vertexShader = rhs.vertexShader;
				type = rhs.type;
				skeletonHandle = rhs.skeletonHandle;
				for (int i = 0; i < maxTextureBinds; ++i)
				{
					textureBindings[i] = rhs.textureBindings[i];
					textureHandles[i] = rhs.textureHandles[i];
				}
				textureCount = rhs.textureCount;
				topology = rhs.topology;
				fillSolid = rhs.fillSolid;
				transparency = rhs.transparency;
			}
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
				stateChanges = (stateChanges << 1) | (pixelShader != rhs.pixelShader);
				stateChanges = (stateChanges << 1) | (vertexShader != rhs.vertexShader);
				const uint32_t loopTo = std::min(textureCount, rhs.textureCount);
				for (uint32_t i = 0; i < loopTo; ++i)
				{
					stateChanges = (stateChanges << 1) | (textureBindings[i] != rhs.textureBindings[i]);
					stateChanges = (stateChanges << 1) | (textureHandles[i] != rhs.textureHandles[i]);
				}
				stateChanges = (stateChanges << 1) | (topology != rhs.topology);
				stateChanges = (stateChanges << 1) | (type != rhs.type);
				stateChanges = (stateChanges << 1) | (skeletonHandle != rhs.skeletonHandle);
				stateChanges = (stateChanges << 1) | (fillSolid != rhs.fillSolid);
				stateChanges = (stateChanges << 1) | (transparency != rhs.transparency);
				std::bitset<32> bits(stateChanges);
				return bits.count() + std::abs((int)textureCount - (int)rhs.textureCount);
			}
		};
	}
}

#endif

