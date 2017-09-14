#include "StaticVertexBufferHandler.h"

namespace SE
{
	namespace Graphics
	{
		StaticVertexBufferHandler::StaticVertexBufferHandler()
		{

		}

		StaticVertexBufferHandler::~StaticVertexBufferHandler()
		{
			
		}

		StaticVertexBufferHandler::StaticVertexBufferHandler(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceContext)
		{
			device = inDevice;
			deviceContext = inDeviceContext;
		}

		void StaticVertexBufferHandler::Shutdown()
		{
			for (auto &vb : vertexCount)
			{
				if (vb->vertexBuffer)
				{
					vb->vertexBuffer->Release();
				}
				delete vb;
			}
		}

		HRESULT StaticVertexBufferHandler::CreateVertexBuffer(void* inputData, int inputSize, int *vertexBufferID)
		{
			D3D11_BUFFER_DESC _vertexBufferDesc;
			// description for vertexbuffer
			_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			_vertexBufferDesc.ByteWidth = inputSize;
			_vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			_vertexBufferDesc.CPUAccessFlags = 0;
			_vertexBufferDesc.MiscFlags = 0;
			_vertexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA _vertexData;
			_vertexData.pSysMem = inputData;
			_vertexData.SysMemPitch = 0;
			_vertexData.SysMemSlicePitch = 0;

			
			// Creates vertex buffern
			ID3D11Buffer* tempBuffer;
			HRESULT _hr = device->CreateBuffer(&_vertexBufferDesc, &_vertexData, &tempBuffer);

			if (_hr != S_OK)
			{
				return _hr;
			}
			if (stackID.size() == 0)
			{
				VertexCount* tempVertexCount = new VertexCount();
				tempVertexCount->vertexBuffer = tempBuffer;
				tempVertexCount->size = inputSize;
				vertexCount.push_back(tempVertexCount);
				*vertexBufferID = vertexCount.size() - 1;
			}
			else
			{
				delete vertexCount.at(stackID.top())->vertexBuffer;
				vertexCount.at(stackID.top())->vertexBuffer = tempBuffer;
				*vertexBufferID = stackID.top();
				stackID.pop();
			}
			return _hr;
		}

		void StaticVertexBufferHandler::SetVertexBuffer(int vertexBufferID)
		{
			UINT32 vertexSize = sizeof(float) * 8;
			UINT32 offset = 0;
			deviceContext->IASetVertexBuffers(0, 1, &vertexCount.at(vertexBufferID)->vertexBuffer, &vertexSize, &offset);
		}

		void StaticVertexBufferHandler::RemoveVertexBuffer(int vertexBufferID)
		{
			vertexCount.at(vertexBufferID)->vertexBuffer->Release();
			vertexCount.at(vertexBufferID)->vertexBuffer = nullptr;
			stackID.push(vertexBufferID);
		}

		int StaticVertexBufferHandler::GetVertexCount(int vertexBufferID)
		{
			return vertexCount.at(vertexBufferID)->size;
		}
	}	//namespace Graphics
}	//namespace SE