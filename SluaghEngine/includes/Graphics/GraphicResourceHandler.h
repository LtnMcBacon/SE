#ifndef GRAPHICRESOURCEHANDLER_H
#define GRAPHICRESOURCEHANDLER_H

#include <d3d11.h>
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include <vector>
#include <stack>

#include "LiveObjectReporter.h"

namespace SE {

	namespace Graphics {

		struct VertexCount
		{
			ID3D11Buffer* vertexBuffer;
			size_t vertexCount;
			size_t stride;
		};

		struct ConstantBuffer
		{
			ID3D11Buffer* constBuffer;
			size_t size;
		};

		struct TargetOffset
		{
			bool shaderTarget[3] = { false, false, false };
			int offset[3] = { 0, 0, 0 };
		};

		struct VertexData {

			VertexCount vertexBuffer;
		};

		struct ConstantData {

			ConstantBuffer constantBuffer;
			TargetOffset targetOffset;
		};

		struct VertexShaderData {

			ID3D11VertexShader*		vertexShader;
			ID3D11InputLayout*		inputLayout;


		};

		struct PixelShaderData {

			ID3D11PixelShader*		pixelShader;
		};

		/**
		*
		* @brief
		* GraphicResourceHandler is responsible for initializing graphic components
		* Vertex and Constant Buffers
		* Shaders
		*
		* @details
		* The GraphicResourceHandler loads buffers and shaders
		*
		*
		**/
		class GraphicResourceHandler {

		public:

			GraphicResourceHandler(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext);
			~GraphicResourceHandler();

			/**
			* @brief Shutdown releases the buffers and shaders
			* @details Releases the pointers
			* @endcode
			*/
			void Shutdown();

			/**
			* @brief CreateVertexShader initializes a vertex shader with default input layout
			* @details It requires the graphic device to create the shader and input layout
			* @param[in] gDevice The ID3D11Device pointer
			* @param[in] data Pointer to the blob
			* @param[in] size Size of the blob
			* @param[in] int The vertex shader ID
			* @retval return_value_n Returns a HRESULT indicating if the shader and input layout were successfully created or not
			* @warning If shaders are moved to another folder, make sure to change the path in the D3DCompileFromFile function
			*/
			HRESULT CreateVertexShader(ID3D11Device* gDevice, void* data, size_t size, int *vertexShaderID);

			/**
			* @brief CreatePixelShader initializes a pixel shader
			* @details It requires the graphic device to create the shader and and index to the 
			* @param[in] gDevice The ID3D11Device pointer
			* @param[in] data Pointer to the blob
			* @param[in] size Size of the blob
			* @param[in] int The pixel shader ID
			* @retval return_value_n Returns a HRESULT indicating if the shader was successfully created or not
			* @warning If shaders are moved to another folder, make sure to change the path in the D3DCompileFromFile function
			*/
			HRESULT CreatePixelShader(ID3D11Device* gDevice, void* data, size_t size, int *pixelShaderID);

			/**
			* @brief UnbindShaders clears the previously used shaders and input layout
			* @details It's necessary to unbind from previous render call before rendering to avoid errors
			* @warning Should only be used before a render call
			* @endcode
			*/
			void UnbindShaders();

			/**
			* @brief SetMaterial sets the default vertex and pixel shaders with corresponding input layout
			* @details Sets all the minimum components for rendering
			* @endcode
			*/
			void SetMaterial(int vertexID, int pixelID);

			/**
			* @brief	Creates a vertexbuffer with the inputData
			*
			* @param[in] device ID3D11Device to allow for creation of the vertexbuffer
			*
			* @param[in] inputData The data that will be set to the vertexbuffer
			*
			* @param[in] vertexCount The number of vertices.
			*
			* @param[in] stride The size of one vertex.
			*
			* @param[in,out] vertexBufferID Variable to save the vertex buffers id in
			*
			* @retval S_OK Returns if vertex buffer creation succeded
			*
			* @retval nonZero Returns if failed
			*
			*/
			HRESULT CreateVertexBuffer(void* inputData, size_t vertexCount, size_t stride, int *vertexBufferID);
			void SetVertexBuffer(int vertexBufferID);

			/**
			* @brief	Removes the targeted vertex buffer
			*
			* @param[in] vertexBufferID The id of the vertex buffer we want to remove
			*
			*/
			void RemoveVertexBuffer(int vertexBufferID);
			size_t GetVertexCount(int vertexBufferID) const;

			/**
			* @brief	Adds a constant buffer
			*
			* @param[in] size The size to be given to the buffer
			*
			* @param[in] target A bool array to that holds the info for which shaders to bind the buffer to
			*
			* @param[in] offset A int array to that holds the info for which offset to bind the buffer to
			*
			* @param[in] constBufferID To return the ID of the buffer
			*
			* @retval S_OK Buffer creation succeded
			*
			* @retval nonZero Creation failed
			*
			*/
			HRESULT CreateConstantBuffer(size_t size, TargetOffset& targetOffset, int *constBufferID);

			/**
			* @brief	Bind the constant buffer to the shaders.
			*
			* @param[in] inData The data to be place in the buffer
			*
			* @param[in] constBufferID Tells which constant buffer to use
			*
			*/
			void BindConstantBuffer(int constBufferID);
			/**
			* @brief	Set constant buffer data
			*
			* @param[in] inData The data to be place in the buffer
			*
			* @param[in] constBufferID Tells which constant buffer to use
			*
			*/
			void SetConstantBuffer(void* inData, int constBufferID);
			/**
			* @brief	Removes a constant buffer
			*
			* @param[in] constBufferID Tells which constant buffer to remove
			*
			*/
			void RemoveConstantBuffer(int constBufferID);
			/**
			* @brief	Creates and sets a simple sampler
			*
			* @retval S_OK Buffer creation succeded
			*
			* @retval nonZero Creation failed
			*
			*/
			HRESULT CreateSamplerState();
		private:

			// Device and device context references
			ID3D11Device* gDevice;
			ID3D11DeviceContext* gDeviceContext;

			// Samplerstate
			ID3D11SamplerState* sampleState = nullptr;

			// Vertex shader specific data
			std::vector<VertexShaderData>vShaders;
			std::stack<int>freeVertexShaderLocations;

			// Pixel shader specific data
			std::vector<PixelShaderData>pShaders;
			std::stack<int>freePixelShaderLocations;

			// Vertex buffer specific data
			std::vector<VertexCount> vBuffers;
			std::stack<int> freeVertexBufferLocations;

			// Constant buffer specific data
			std::vector<ConstantBuffer> cBuffers;
			std::vector<TargetOffset> targetOffset;
			std::stack<int> freeConstantBufferLocations;

		};
	}
}



#endif
