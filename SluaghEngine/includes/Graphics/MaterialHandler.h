#ifndef MATERIALHANDLER_H
#define MATERIALHANDLER_H

#include <d3d11.h>
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include <wrl.h>

#include "LiveObjectReporter.h"

namespace SE {

	namespace Graphics {

		class MaterialHandler {

		public:

			MaterialHandler(Microsoft::WRL::ComPtr<ID3D11Device> gDevice);
			~MaterialHandler();

			HRESULT Init();

			HRESULT InitializeDefaultShaders(Microsoft::WRL::ComPtr<ID3D11Device> gDevice);

			void Shutdown();

		private:

			Microsoft::WRL::ComPtr<ID3D11Device> gDevice;

			Microsoft::WRL::ComPtr<ID3D11VertexShader>d_vertexShader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>d_pixelShader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>d_layout;
		};
	}
}

#endif