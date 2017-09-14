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

		/**
		*
		* @brief 
		* MaterialHandler is responsible for initializng input layouts and shaders
		*
		* @details 
		* The MaterialHandler holds pointers to the graphic device and device context to easily reach them for creating and setting shaders
		*
		* @warning 
		* It only holds a default shader pipeline so far, consisting of an input in the following order: POSITION, NORMAL and TEXCOORD
		*
		* @sa ”See Also”, 
		* Renderer
		**/

		class MaterialHandler {

		public:

			MaterialHandler(Microsoft::WRL::ComPtr<ID3D11Device> gDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> gDeviceContext);
			~MaterialHandler();

			HRESULT Init();

			HRESULT InitializeDefaultShaders(Microsoft::WRL::ComPtr<ID3D11Device> gDevice);

			void UnbindShaders();
			void SetMaterial();

			void Shutdown();

		private:

			Microsoft::WRL::ComPtr<ID3D11Device> gDevice;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> gDeviceContext;

			Microsoft::WRL::ComPtr<ID3D11VertexShader>d_vertexShader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>d_pixelShader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>d_layout;
		};
	}
}

#endif