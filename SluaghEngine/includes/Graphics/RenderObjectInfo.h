#ifndef SE_GRAPHICS_RENDEROBJECTINFO_H_
#define SE_GRAPHICS_RENDEROBJECTINFO_H_

namespace SE
{
	namespace Graphics
	{
		struct RenderObjectInfo
		{
			int bufferHandle;
			int transformHandle;
			int pixelShader;
			int vertexShader;
			int diffuseTexture;
		};
	}
}

#endif

