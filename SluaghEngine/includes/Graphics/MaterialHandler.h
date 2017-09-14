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

			MaterialHandler(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext);
			~MaterialHandler();

			/**
			* @brief Init initializes the MaterialHandler
			* @details Calls for all necessary functions for all components to be initialized
			* @retval return_value_n Returns a HRESULT indicating if the shaders were successfully created or not
			* @endcode
			*/
			HRESULT Init();

			/**
			* @brief Shutdown releases the DirectX components
			* @details Releases the pointers
			* @endcode
			*/
			void Shutdown();

			/**
			* @brief InitializeDefaultShaders initializes default vertex and pixel shader with default input layout
			* @details It requires the graphic device to create shaders and input layout
			* @param[in] gDevice The ID3D11Device pointer
			* @retval return_value_n Returns a HRESULT indicating if the shaders were successfully created or not
			* @warning If shaders are moved to another folder, make sure to change the path in the D3DCompileFromFile function
			* Example code:
			* @code
			* hr = InitializeDefaultShaders(this->gDevice);
			*
			*	if (FAILED(hr)) {
			*
			*		error handling...
			*	}
			* @endcode
			*/
			HRESULT InitializeDefaultShaders(ID3D11Device* gDevice);

			/**
			* @brief UnbindShaders clears the previously used shaders and input layout
			* @details It's necessary to unbind from previous render call before rendering to avoid errors
			* @warning Should only be used before a render call
			* @endcode
			*/
			void UnbindShaders();

			/**
			* @brief SetMaterial sets the default vertex and pixel shaders with input layout
			* @details Sets all the minimum components for rendering
			* @endcode
			*/
			void SetMaterial();

		private:

			ID3D11Device*			gDevice;
			ID3D11DeviceContext*	gDeviceContext;

			ID3D11VertexShader*		d_vertexShader;
			ID3D11PixelShader*		d_pixelShader;
			ID3D11InputLayout*		d_layout;
		};
	}
}

#endif