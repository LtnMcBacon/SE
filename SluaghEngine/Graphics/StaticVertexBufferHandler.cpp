#include "StaticVertexBufferHandler.h"
#include <Profiler.h>

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
			for (auto &vb : buffers)
			{
				if (vb.vertexBuffer)
				{
					vb.vertexBuffer->Release();
				}
			}
		}

		HRESULT StaticVertexBufferHandler::CreateVertexBuffer(void* inputData, size_t vertexCount, size_t stride, int *vertexBufferID)
		{
			StartProfile;
			D3D11_BUFFER_DESC _vertexBufferDesc;
			// description for vertexbuffer
			_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			_vertexBufferDesc.ByteWidth = vertexCount * stride;
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

			if (FAILED(_hr))
			{
				ProfileReturnConst(_hr);
			}
			if (stackID.size() == 0)
			{
				buffers.push_back({ tempBuffer, vertexCount, stride });
				*vertexBufferID = buffers.size() - 1;
			}
			else
			{
				auto top = stackID.top();
				buffers[top].vertexBuffer = tempBuffer;
				buffers[top].vertexCount = vertexCount;
				buffers[top].stride = stride;
				*vertexBufferID = top;
				stackID.pop();
			}
			ProfileReturnConst(_hr);
		}

		void StaticVertexBufferHandler::SetVertexBuffer(int vertexBufferID)
		{
			UINT32 vertexSize = sizeof(float) * 8;
			UINT32 offset = 0;
			deviceContext->IASetVertexBuffers(0, 1, &buffers[vertexBufferID].vertexBuffer, &vertexSize, &offset);
		}

		void StaticVertexBufferHandler::RemoveVertexBuffer(int vertexBufferID)
		{
			buffers[vertexBufferID].vertexBuffer->Release();
			buffers[vertexBufferID].vertexBuffer = nullptr;
			stackID.push(vertexBufferID);
		}

		size_t StaticVertexBufferHandler::GetVertexCount(int vertexBufferID) const
		{
			return buffers[vertexBufferID].vertexCount;
		}
	}	//namespace Graphics
}	//namespace SE