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

			/*inline*/ size_t operator==(const GUITextureInfo& rhs) const
			{
				uint32_t stateChanges = 0;
				stateChanges = (stateChanges << 1) | (pos.x != rhs.pos.x);
				stateChanges = (stateChanges << 1) | (pos.y != rhs.pos.y);
				if (stateChanges != 0)
					return false;
				stateChanges = (stateChanges << 1) | (rotation != rhs.rotation);
				stateChanges = (stateChanges << 1) | (layerDepth != rhs.layerDepth);
				stateChanges = (stateChanges << 1) | (rect != rhs.rect);
				stateChanges = (stateChanges << 1) | (origin.x != rhs.origin.x);
				stateChanges = (stateChanges << 1) | (origin.y != rhs.origin.y);
				if (stateChanges != 0)
					return false;
				stateChanges = (stateChanges << 1) | (scale.x != rhs.scale.x);
				stateChanges = (stateChanges << 1) | (scale.y != rhs.scale.y);
				stateChanges = (stateChanges << 1) | (colour.x != rhs.colour.x);
				stateChanges = (stateChanges << 1) | (colour.y != rhs.colour.y);
				stateChanges = (stateChanges << 1) | (colour.z != rhs.colour.z);
				stateChanges = (stateChanges << 1) | (colour.w != rhs.colour.w);
				stateChanges = (stateChanges << 1) | (effect != rhs.effect);
				stateChanges = (stateChanges << 1) | (anchor != rhs.anchor);
				if (stateChanges != 0)
					return false;
				return true;
			}
		};

		struct TexUsage
		{
			int textureHandle;
			size_t refCount;
		};
	}
}
#endif