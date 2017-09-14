#pragma once
#ifndef SE_GRAPHICS_STATICVERTEXBUFFERHANDLER_H_
#define SE_GRAPHICS_STATICVERTEXBUFFERHANDLER_H_
#include <vector>
#include <stack>
#include <d3d11.h>
#include <wrl.h>

namespace SE
{
	namespace Graphics
	{
		struct VertexCount
		{
			ID3D11Buffer* vertexBuffer;
			int size;
		};
		class StaticVertexBufferHandler
		{
		public:
			StaticVertexBufferHandler();
			StaticVertexBufferHandler(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext);
			~StaticVertexBufferHandler();
			void Shutdown();
			/**
			* @brief	Creates a vertexbuffer with the inputData
			*
			* @param[in] device ID3D11Device to allow for creation of the vertexbuffer
			*
			* @param[in] inputData The data that will be set to the vertexbuffer
			*
			* @param[in] inputSize The size of the inputData to be able to create a big enough buffer
			*
			* @param[in,out] vertexBufferID Variable to save the vertex buffers id in
			*
			* @retval S_OK Returns if vertex buffer creation succeded
			*
			* @retval nonZero Returns if failed
			*
			*/
			HRESULT CreateVertexBuffer(void* inputData, int inputSize, int *vertexBufferID);
			void SetVertexBuffer(int vertexBufferID);
			/**
			* @brief	Removes the targeted vertex buffer
			*
			* @param[in] vertexBufferID The id of the vertex buffer we want to remove
			*
			*/
			void RemoveVertexBuffer(int vertexBufferID);
			int GetVertexCount(int vertexBufferID);
		private:
			std::vector<VertexCount*> vertexCount;
			std::stack<int> stackID;
			ID3D11Device* device;
			ID3D11DeviceContext* deviceContext;
		};

	}	//namespace Graphics
}	//namespace SE
#endif //SE_GRAPHICS_STATICVERTEXBUFFERHANDLER_H_