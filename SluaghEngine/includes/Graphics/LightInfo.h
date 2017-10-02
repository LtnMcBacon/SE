#pragma once
#ifndef SE_GRAPHICS_LIGHT_INFO_H_
#define SE_GRAPHICS_LIGHT_INFO_H_
#include <DirectXMath.h>

namespace SE
{
	namespace Graphics
	{
		struct LightData
		{
			DirectX::XMFLOAT4 colour;	//colour (rgba)
			DirectX::XMFLOAT4 pos;	//pos (pos + range)

			/*inline*/ size_t operator==(const LightData& rhs) const
			{
				uint32_t stateChanges = 0;
				stateChanges = (stateChanges << 1) | (pos.x == rhs.pos.x);
				stateChanges = (stateChanges << 1) | (pos.y == rhs.pos.y);
				stateChanges = (stateChanges << 1) | (pos.z == rhs.pos.z);
				stateChanges = (stateChanges << 1) | (pos.w == rhs.pos.w);
				if (stateChanges != 0)
					return false;
				stateChanges = (stateChanges << 1) | (colour.x != rhs.colour.x);
				stateChanges = (stateChanges << 1) | (colour.y != rhs.colour.y);
				stateChanges = (stateChanges << 1) | (colour.z != rhs.colour.z);
				stateChanges = (stateChanges << 1) | (colour.w != rhs.colour.w);
				if (stateChanges != 0)
					return false;
				return true;
			}
		};	
	}
}
#endif