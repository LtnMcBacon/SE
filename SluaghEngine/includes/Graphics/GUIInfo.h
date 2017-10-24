#pragma once
#pragma once
#ifndef SE_GRAPHICS_GUI_INFO_H_
#define SE_GRAPHICS_GUI_INFO_H_
#include <DirectXMath.h>
#include <Utilz\GUID.h>
#include <ToolKit\SpriteFont.h>
#include <ToolKit\SpriteBatch.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTKD.lib")
#else
#pragma comment(lib, "DirectXTK.lib")
#endif

namespace SE
{
	namespace Graphics
	{
		struct TextGUI
		{
			size_t fontID;
			int hashString;
			std::wstring text;		
			DirectX::XMFLOAT4 colour;
			DirectX::XMFLOAT2 pos;
			DirectX::XMFLOAT2 origin;			
			DirectX::XMFLOAT2 scale;
			DirectX::SpriteEffects effect;
			float rotation;		
			float layerDepth;
			bool anchor = false;
		};

		struct GUITextureInfo
		{
			int textureID = 0;
			DirectX::XMFLOAT2 pos;
			DirectX::XMFLOAT4 colour;
			DirectX::XMFLOAT2 origin;
			DirectX::XMFLOAT2 scale;
			DirectX::SpriteEffects effect;
			RECT* rect = nullptr;
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