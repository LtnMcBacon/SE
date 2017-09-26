#pragma once
#pragma once
#ifndef SE_GRAPHICS_GUI_INFO_H_
#define SE_GRAPHICS_GUI_INFO_H_
#include <DirectXMath.h>
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
		//find a better place for me pls
		struct TextGUI
		{
			size_t fontID;
			int hashString;
			std::wstring text;		
			DirectX::XMFLOAT3 colour;
			DirectX::XMFLOAT2 pos;
			DirectX::XMFLOAT2 origin;			
			DirectX::XMFLOAT2 scale;
			DirectX::SpriteEffects effect;
			float rotation;		
			float layerDepth;
		};
	}
}
#endif