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
			for (auto &cb : constSizeBuffer)
			{
				if (cb->constBuffer)
				{
					cb->constBuffer->Release();
				}
				delete cb;
			}
		}

		void ConstantBufferHandler::Shutdown()
		{
			for (auto &cb : constSizeBuffer)
			{
				if (cb->constBuffer)
				{
					cb->constBuffer->Release();
				}
				delete cb;
			}
		}

		HRESULT ConstantBufferHandler::AddConstantBuffer(int size, TargetOffset& inTargetOffset, int *constBufferID)
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

			int hej = freeBufferLocations.size();

			if (freeBufferLocations.size() == 0)
			{
				constSizeBuffer.push_back(tempConstSize);
				targetOffset.push_back(inTargetOffset);
				*constBufferID = constSizeBuffer.size() - 1;
			}
			else
			{
				delete constSizeBuffer.at(freeBufferLocations.top());
				constSizeBuffer.at(freeBufferLocations.top()) = tempConstSize;
				targetOffset.at(freeBufferLocations.top()) = inTargetOffset;
				*constBufferID = freeBufferLocations.top();
				freeBufferLocations.pop();
			}
			return hr;
		}

		void ConstantBufferHandler::SetConstantBuffer(void* inData, int constBufferID)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = deviceContext->Map(constSizeBuffer.at(constBufferID)->constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			memcpy(mappedResource.pData, inData, constSizeBuffer.at(constBufferID)->size);
			deviceContext->Unmap(constSizeBuffer.at(constBufferID)->constBuffer, 0);


			if (targetOffset.at(constBufferID).shaderTarget[0] == true)
			{
				deviceContext->VSSetConstantBuffers(targetOffset.at(constBufferID).offset[0], 1, &constSizeBuffer.at(constBufferID)->constBuffer);
			}
			if (targetOffset.at(constBufferID).shaderTarget[1] == true)
			{
				deviceContext->GSSetConstantBuffers(targetOffset.at(constBufferID).offset[1], 1, &constSizeBuffer.at(constBufferID)->constBuffer);
			}
			if (targetOffset.at(constBufferID).shaderTarget[2] == true)
			{
				deviceContext->PSSetConstantBuffers(targetOffset.at(constBufferID).offset[2], 1, &constSizeBuffer.at(constBufferID)->constBuffer);
			}
		}

		void ConstantBufferHandler::RemoveConstantBuffer(int constBufferID)
		{
			constSizeBuffer.at(constBufferID)->constBuffer->Release();
			constSizeBuffer.at(constBufferID)->constBuffer = nullptr;
			freeBufferLocations.push(constBufferID);
		}
	}
}