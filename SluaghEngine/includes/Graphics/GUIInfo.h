#pragma once
#pragma once
#ifndef SE_GRAPHICS_GUI_INFO_H_
#define SE_GRAPHICS_GUI_INFO_H_
#include <DirectXMath.h>
#include <Utilz\GUID.h>
#include <stdint.h>


namespace SE
{
	namespace Graphics
	{
		enum Effect
		{
			NoEffect = 0,
			HoriFlipEffect = 1,
			VertiFlipEffect = 2,
			BothFlipEffect = HoriFlipEffect | VertiFlipEffect
		};

		struct TextGUI
		{
			size_t fontID;
			int hashString;
			std::wstring text;		
			DirectX::XMFLOAT4 colour;
			DirectX::XMFLOAT2 pos;
			DirectX::XMFLOAT2 origin;			
			DirectX::XMFLOAT2 scale;
			Effect effect;
			float rotation;		
			float layerDepth;
			bool anchor = false;
		};
		struct RECT
		{
			int32_t    left;
			int32_t    top;
			int32_t    right;
			int32_t    bottom;
			RECT& operator=(const RECT& other)
			{
				memcpy(this, &other, sizeof(RECT));
				return *this;
			};
		};
		struct GUITextureInfo
		{
			int textureID = 0;
			DirectX::XMFLOAT2 pos;
			DirectX::XMFLOAT4 colour;
			DirectX::XMFLOAT2 origin;
			DirectX::XMFLOAT2 scale;
			Effect effect;
			RECT rect;
			float rotation;
			float layerDepth;
			bool anchor = false;
		};

		struct TexUsage
		{
			int textureHandle;
			size_t refCount;
			size_t height;
			size_t width;
		};
	}
}
#endif