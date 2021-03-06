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
			size_t hashString;
			std::wstring text;		
			long posX = 0;
			long posY = 0;
			long width = -1;
			long height = -1;
			DirectX::XMFLOAT4 colour = { 1.0f, 1.0f, 1.0f, 1.0f };
			DirectX::XMFLOAT2 anchor = { 0.0f, 0.0f };
			DirectX::XMFLOAT2 scale = { 1.0f, 1.0f };
			DirectX::XMFLOAT2 screenAnchor = { 0.0f, 0.0f };

			Effect effect = Effect::NoEffect;
			float rotation = 0.0f;
			float layerDepth = 0.0f;
			bool absolute = false;
		};
		struct TextJob
		{
			size_t fontID;
			long originalScreenWidth;
			long originalScreenHeight;
			TextGUI info;
		};
		struct GUITextureInfo
		{
			long posX = 0;
			long posY = 0;
			long width = -1;
			long height = -1;
			DirectX::XMFLOAT4 colour = { 1.0f, 1.0f, 1.0f, 1.0f };
			DirectX::XMFLOAT2 anchor = { 0.0f, 0.0f };
			DirectX::XMFLOAT2 scale = { 1.0f, 1.0f };
			DirectX::XMFLOAT2 screenAnchor = { 0.0f, 0.0f };	

			Effect effect = Effect::NoEffect;
			float rotation = 0.0f;
			float layerDepth = 0.0f;
			bool absolute = false;
		};
		struct GUIJob
		{
			int textureID;
			long originalScreenWidth;
			long originalScreenHeight;
			GUITextureInfo info;
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