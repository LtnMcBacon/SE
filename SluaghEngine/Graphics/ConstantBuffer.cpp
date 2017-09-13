#include "ConstantBuffer.h"
namespace SE
{
	namespace Graphics
	{
		ConstantBuffer::ConstantBuffer()
		{

		}

		ConstantBuffer::~ConstantBuffer()
		{
			if (constBuffer != nullptr)
			{
				constBuffer->Release();
			}
		}

		HRESULT ConstantBuffer::CreateBuffer(ID3D11Device* device, int size)
		{
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = size;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;

			//skapar constant buffer
			HRESULT hr = device->CreateBuffer(&bufferDesc, NULL, &constBuffer);
			bufferSize = size;

			return hr;
		}

		void ConstantBuffer::SetConstBuffer(ID3D11DeviceContext* deviceContext, void* inData, TargetOffset *targetOffset)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;

			HRESULT hr = deviceContext->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			memcpy(constBuffer, inData, bufferSize);
			deviceContext->Unmap(constBuffer, 0);

			if (targetOffset->shaderTarget[0] == true)
			{
				deviceContext->VSSetConstantBuffers(targetOffset->offset[0], 1, &constBuffer);
			}
			else if (targetOffset->shaderTarget[1] == true)
			{
				deviceContext->GSSetConstantBuffers(targetOffset->offset[1], 1, &constBuffer);
			}
			else if (targetOffset->shaderTarget[2] == true)
			{
				deviceContext->PSSetConstantBuffers(targetOffset->offset[2], 1, &constBuffer);
			}
		}
	}
}