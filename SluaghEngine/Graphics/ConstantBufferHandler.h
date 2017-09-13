#pragma once
#ifndef SE_GRAPHICS_CONSTANTBUFFERHANDLER_H_
#define SE_GRAPHICS_CONSTANTBUFFERHANDLER_H_
#include "ConstantBuffer.h"
#include <vector>
#include <stack>

namespace SE
{
	namespace Graphics
	{
		class ConstantBufferHandler
		{
		public:
			ConstantBufferHandler();
			~ConstantBufferHandler();
			/**
			* @brief	Adds a constant buffer
			*
			* @param[in] device The ID3D11Device to allow us to create the buffer
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
			HRESULT AddConstantBuffer(ID3D11Device* device, int size, bool* target, int* offset, int &constBufferID);
			/**
			* @brief	Set constant buffer to shader
			*
			* @param[in] deviceContext The ID3D11DeviceContext to allow us to bind the buffer
			*
			* @param[in] inData The data to be place in the buffer
			*
			* @param[in] constBufferID Tells which constant buffer to use
			*
			*/
			void SetConstantBuffer(ID3D11DeviceContext* deviceContext, void* inData, int constBufferID);
			/**
			* @brief	Removes a constant buffer
			*
			* @param[in] constBufferID Tells which constant buffer to remove
			*
			*/
			void RemoveConstantBuffer(int constBufferID);
		private:
			std::vector<ConstantBuffer*> constBuffer;
			std::vector<TargetOffset*> targetOffset;
			std::stack<int> freeBufferLocations;
		};

	}
}
#endif //SE_GRAPHICS_CONSTANTBUFFERHANDLER_H_