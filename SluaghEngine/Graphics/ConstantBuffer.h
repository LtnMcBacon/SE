#pragma once
#ifndef SE_GRAPHICS_CONSTANTBUFFER_H_
#define SE_GRAPHICS_CONSTANTBUFFER_H_
#include <d3d11.h>

namespace SE
{
	namespace Graphics
	{
		struct TargetOffset
		{
			bool shaderTarget[3] = { false, false, false };
			int offset[3] = { 0, 0, 0 };
		};

		class ConstantBuffer
		{
		public:
			ConstantBuffer();
			~ConstantBuffer();
			/** 
			* @brief	Creates the constant buffer
			*
			* @param[in] device The ID3D11Device to allow us to create the buffer
			*
			* @param[in] size The size to be given to the buffer
			*
			* @retval S_OK Buffer creation succeded
			*
			* @retval nonZero Creation failed
			*
			*/
			HRESULT CreateBuffer(ID3D11Device* device, int size);
			/**
			* @brief	Set constant buffer to shader
			*
			* @param[in] deviceContext The ID3D11DeviceContext to allow us to bind the buffer
			*
			* @param[in] inData The data to be place in the buffer
			*
			* @param[in] targetOffset Tells which shaders and offsets to use
			*
			*/
			void SetConstBuffer(ID3D11DeviceContext* deviceContext, void* inData, TargetOffset *targetOffset);
		private:
			ID3D11Buffer* constBuffer = nullptr;
			int bufferSize;
		};

	}
}
#endif //SE_GRAPHICS_CONSTANTBUFFER_H_