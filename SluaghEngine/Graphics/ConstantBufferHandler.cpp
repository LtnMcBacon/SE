#include "ConstantBufferHandler.h"


namespace SE
{
	namespace Graphics
	{
		ConstantBufferHandler::ConstantBufferHandler()
		{

		}

		ConstantBufferHandler::ConstantBufferHandler(Microsoft::WRL::ComPtr<ID3D11Device> inDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> inDeviceContext)
		{
			device = inDevice;
			deviceContext = inDeviceContext;
		}

		ConstantBufferHandler::~ConstantBufferHandler()
		{

		}

		HRESULT ConstantBufferHandler::AddConstantBuffer(int size, bool* target, int* offset, int *constBufferID)
		{			
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = size;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;

			//skapar constant buffer
			constSize *tempConstSize = new constSize();
			HRESULT hr = device->CreateBuffer(&bufferDesc, NULL, &tempConstSize->constBuffer);
			tempConstSize->size = size;

			if (hr != S_OK)
			{
				return hr;
			}

			TargetOffset *tempTargetOffset = new TargetOffset();
			tempTargetOffset->offset[0] = offset[0];
			tempTargetOffset->offset[1] = offset[1];
			tempTargetOffset->offset[2] = offset[2];
			tempTargetOffset->shaderTarget[0] = target[0];
			tempTargetOffset->shaderTarget[1] = target[1];
			tempTargetOffset->shaderTarget[2] = target[2];

			if (freeBufferLocations.size() > 0)
			{
				constBuffer.push_back(tempConstSize);
				targetOffset.push_back(tempTargetOffset);
				*constBufferID = constBuffer.size();
			}
			else
			{
				constBuffer.at(freeBufferLocations.top()) = tempConstSize;
				targetOffset.at(freeBufferLocations.top()) = tempTargetOffset;
				*constBufferID = freeBufferLocations.top();
				freeBufferLocations.pop();
			}
			return hr;
		}

		void ConstantBufferHandler::SetConstantBuffer(void* inData, int constBufferID)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;

			HRESULT hr = deviceContext->Map(constBuffer.at(constBufferID)->constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			memcpy(constBuffer.at(constBufferID)->constBuffer, inData, constBuffer.at(constBufferID)->size);
			deviceContext->Unmap(constBuffer.at(constBufferID)->constBuffer, 0);

			if (targetOffset.at(constBufferID)->shaderTarget[0] == true)
			{
				deviceContext->VSSetConstantBuffers(targetOffset.at(constBufferID)->offset[0], 1, &constBuffer.at(constBufferID)->constBuffer);
			}
			if (targetOffset.at(constBufferID)->shaderTarget[1] == true)
			{
				deviceContext->GSSetConstantBuffers(targetOffset.at(constBufferID)->offset[1], 1, &constBuffer.at(constBufferID)->constBuffer);
			}
			if (targetOffset.at(constBufferID)->shaderTarget[2] == true)
			{
				deviceContext->PSSetConstantBuffers(targetOffset.at(constBufferID)->offset[2], 1, &constBuffer.at(constBufferID)->constBuffer);
			}
		}

		void ConstantBufferHandler::RemoveConstantBuffer(int constBufferID)
		{
			constBuffer.at(constBufferID)->constBuffer->Release();
			delete constBuffer.at(constBufferID);
			delete[] targetOffset.at(constBufferID)->offset;
			delete[] targetOffset.at(constBufferID)->shaderTarget;
			delete targetOffset.at(constBufferID);
			freeBufferLocations.push(constBufferID);
		}
	}
}