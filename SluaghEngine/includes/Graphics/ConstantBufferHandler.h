#pragma once
#ifndef SE_GRAPHICS_CONSTANTBUFFERHANDLER_H_
#define SE_GRAPHICS_CONSTANTBUFFERHANDLER_H_
#include <vector>
#include <stack>
#include <wrl.h>
#include <d3d11.h>

namespace SE
{
	namespace Graphics
	{
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

		class ConstantBufferHandler
		{
		public:
			ConstantBufferHandler();
			ConstantBufferHandler(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext);
			~ConstantBufferHandler();
			void Shutdown();
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
			HRESULT AddConstantBuffer(size_t size, TargetOffset& targetOffset, int *constBufferID);

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
		private:
			std::vector<ConstantBuffer> buffers;
			std::vector<TargetOffset> targetOffset;
			std::stack<int> freeBufferLocations;
			ID3D11Device* device;
			ID3D11DeviceContext* deviceContext;
		};

	}
}
#endif //SE_GRAPHICS_CONSTANTBUFFERHANDLER_H_