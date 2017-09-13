#include "ConstantBufferHandler.h"


namespace SE
{
	namespace Graphics
	{
		ConstantBufferHandler::ConstantBufferHandler()
		{

		}

		ConstantBufferHandler::~ConstantBufferHandler()
		{

		}

		HRESULT ConstantBufferHandler::AddConstantBuffer(ID3D11Device* device, int size, bool* target, int* offset, int &constBufferID)
		{	
			ConstantBuffer *tempConstBuffer = new ConstantBuffer();
			HRESULT hr = tempConstBuffer->CreateBuffer(device, size);
			
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
				constBuffer.push_back(tempConstBuffer);
				targetOffset.push_back(tempTargetOffset);
				constBufferID = constBuffer.size();
			}
			else
			{
				constBuffer.at(freeBufferLocations.top()) = tempConstBuffer;
				targetOffset.at(freeBufferLocations.top()) = tempTargetOffset;
				constBufferID = freeBufferLocations.top();
				freeBufferLocations.pop();
			}
			return hr;
		}

		void ConstantBufferHandler::SetConstantBuffer(ID3D11DeviceContext* deviceContext, void* inData, int constBufferID)
		{
			constBuffer.at(constBufferID)->SetConstBuffer(deviceContext, inData, targetOffset.at(constBufferID));
		}

		void ConstantBufferHandler::RemoveConstantBuffer(int constBufferID)
		{
			delete constBuffer.at(constBufferID);
			delete targetOffset.at(constBufferID)->offset;
			delete targetOffset.at(constBufferID)->shaderTarget;
			delete targetOffset.at(constBufferID);
			freeBufferLocations.push(constBufferID);
		}
	}
}