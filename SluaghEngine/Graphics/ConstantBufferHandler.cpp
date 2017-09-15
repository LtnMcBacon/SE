#include "ConstantBufferHandler.h"


namespace SE
{
	namespace Graphics
	{
		ConstantBufferHandler::ConstantBufferHandler()
		{

		}

		ConstantBufferHandler::ConstantBufferHandler(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext)
		{
			device = inDevice;
			deviceContext = inDeviceContext;
		}

		ConstantBufferHandler::~ConstantBufferHandler()
		{

		}

		void ConstantBufferHandler::Shutdown()
		{
			for (auto &cb : buffers)
			{
				if (cb.constBuffer)
				{
					cb.constBuffer->Release();
				}
			}
		}

		HRESULT ConstantBufferHandler::AddConstantBuffer(size_t size, TargetOffset& inTargetOffset, int *constBufferID)
		{			
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = size;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;

			//skapar constant buffer
			ConstantBuffer tempBuffer;
			HRESULT hr = device->CreateBuffer(&bufferDesc, NULL, &tempBuffer.constBuffer);
			tempBuffer.size = size;

			if (FAILED(hr))
			{
				return hr;
			}
			if (freeBufferLocations.size() == 0)
			{
				buffers.push_back(tempBuffer);
				targetOffset.push_back(inTargetOffset);
				*constBufferID = buffers.size() - 1;
			}
			else
			{
				auto top = freeBufferLocations.top();
				buffers[top] = tempBuffer;
				targetOffset[top] = inTargetOffset;
				*constBufferID = top;
				freeBufferLocations.pop();
			}
			return hr;
		}

		void ConstantBufferHandler::SetConstantBuffer(void* inData, int constBufferID)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = deviceContext->Map(buffers[constBufferID].constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			memcpy(mappedResource.pData, inData, buffers[constBufferID].size);
			deviceContext->Unmap(buffers[constBufferID].constBuffer, 0);


			if (targetOffset.at(constBufferID).shaderTarget[0] == true)
			{
				deviceContext->VSSetConstantBuffers(targetOffset.at(constBufferID).offset[0], 1, &buffers[constBufferID].constBuffer);
			}
			if (targetOffset.at(constBufferID).shaderTarget[1] == true)
			{
				deviceContext->GSSetConstantBuffers(targetOffset.at(constBufferID).offset[1], 1, &buffers[constBufferID].constBuffer);
			}
			if (targetOffset.at(constBufferID).shaderTarget[2] == true)
			{
				deviceContext->PSSetConstantBuffers(targetOffset.at(constBufferID).offset[2], 1, &buffers[constBufferID].constBuffer);
			}
		}

		void ConstantBufferHandler::RemoveConstantBuffer(int constBufferID)
		{
			buffers[constBufferID].constBuffer->Release();
			buffers[constBufferID].constBuffer = nullptr;
			freeBufferLocations.push(constBufferID);
		}
	}
}