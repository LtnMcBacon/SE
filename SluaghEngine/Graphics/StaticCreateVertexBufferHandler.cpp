#include "StaticCreateVertexBufferHandler.h"

namespace SE
{
	namespace Graphics
	{
		StaticCreateVertexBufferHandler::StaticCreateVertexBufferHandler()
		{

		}

		StaticCreateVertexBufferHandler::~StaticCreateVertexBufferHandler()
		{

		}

		HRESULT StaticCreateVertexBufferHandler::CreateVertexBuffer(ID3D11Device* device, void* inputData, int inputSize, int &vertexBufferID)
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
				vertexBuffer.push_back(tempBuffer);
				vertexBufferID = vertexBuffer.size() - 1;
			}
			else
			{
				vertexBuffer.at(stackID.top()) = tempBuffer;
				vertexBufferID = stackID.top();
				stackID.pop();
			}
			return _hr;
		}

		ID3D11Buffer* StaticCreateVertexBufferHandler::GetVertexBuffer(int vertexBufferID)
		{
			return vertexBuffer.at(vertexBufferID);
		}

		void StaticCreateVertexBufferHandler::RemoveVertexBuffer(int vertexBufferID)
		{
			vertexBuffer.at(vertexBufferID)->Release();
			stackID.push(vertexBufferID);
		}
	}	//namespace Graphics
}	//namespace SE