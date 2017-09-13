#pragma once
#ifndef SE_GRAPHICS_STATICCREATEVERTEXBUFFERHANDLER_H_
#define SE_GRAPHICS_STATICCREATEVERTEXBUFFERHANDLER_H_
#include <vector>
#include <stack>
#include <d3d11.h>

namespace SE
{
	namespace Graphics
	{
		class StaticCreateVertexBufferHandler
		{
		public:
			StaticCreateVertexBufferHandler();
			~StaticCreateVertexBufferHandler();
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
			HRESULT CreateVertexBuffer(ID3D11Device* device, void* inputData, int inputSize, int &vertexBufferID);
			ID3D11Buffer* GetVertexBuffer(int vertexBufferID);
			/**
			* @brief	Removes the targeted vertex buffer
			*
			* @param[in] vertexBufferID The id of the vertex buffer we want to remove
			*
			*/
			void RemoveVertexBuffer(int vertexBufferID);
		private:
			std::vector<ID3D11Buffer*> vertexBuffer;
			std::stack<int> stackID;
		};

	}	//namespace Graphics
}	//namespace SE
#endif //SE_GRAPHICS_STATICCREATEVERTEXBUFFERHANDLER_H_