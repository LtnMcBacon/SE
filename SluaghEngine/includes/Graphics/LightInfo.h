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
		};	
	}
}
#endif