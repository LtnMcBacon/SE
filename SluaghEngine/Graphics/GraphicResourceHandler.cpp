
#include "GraphicResourceHandler.h"
#include <Profiler.h>


#include <d3dcompiler.h>
#pragma comment(lib, "D3Dcompiler.lib")

using namespace SE::Graphics;



GraphicResourceHandler::GraphicResourceHandler(ID3D11Device* gDevice, ID3D11DeviceContext* gDeviceContext) {

	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
	sampleState = nullptr;
}

GraphicResourceHandler::~GraphicResourceHandler() {


}

void GraphicResourceHandler::Shutdown() {

	// Remove vertex buffers
	for (auto &vb : vBuffers)
	{
		if (vb.vertexBuffer)
		{
			vb.vertexBuffer->Release();
		}
	}

	// Remove constant buffers
	for (auto &cb : cBuffers)
	{
		if (cb.constBuffer)
		{
			cb.constBuffer->Release();
		}
	}

	for (auto &vs : vShaders) {

		if (vs.vertexShader) {

			vs.vertexShader->Release();
		}

		if (vs.inputLayout) {

			vs.inputLayout->Release();
		}
	}

	for (auto &ps : pShaders) {

		if (ps.pixelShader) {

			ps.pixelShader->Release();
		}
	}
	for (auto &cs : cShaders) {

		if (cs.shader) {

			cs.shader->Release();
		}
	}

	// Release Samplerstate
	if (sampleState != nullptr)
	{
		sampleState->Release();
		sampleState = nullptr;
	}
	for (auto& srv : shaderResourceViews)
	{
		if (srv)
			srv->Release();
	}
	for (auto& rtv : renderTargetViews)
	{
		if (rtv)
			rtv->Release();
	}
	for (auto& texture : texture2Ds)
	{
		if (texture)
			texture->Release();
	}
	for (auto& uav : unorderedAccessViews)
	{
		if (uav)
			uav->Release();
	}
}

HRESULT GraphicResourceHandler::CreateVertexShader(ID3D11Device* gDevice, void* data, size_t size, int *vertexShaderID) {

	StartProfile;

	ID3D11InputLayout* tempInputLayout = nullptr;
	ID3D11VertexShader*	tempVertexShader = nullptr;

	HRESULT hr = gDevice->CreateVertexShader(data, size, nullptr, &tempVertexShader);

	if (FAILED(hr)) {
		ProfileReturnConst(hr);
	}

	//Create the input layout with the help of shader reflection
	ID3D11ShaderReflection* reflection;
	hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
	if (FAILED(hr))
	{
		ProfileReturnConst(hr);
	}
	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
	uint32_t offset = 0;
	for(uint32_t i = 0; i < shaderDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC signatureParamaterDesc;
		reflection->GetInputParameterDesc(i, &signatureParamaterDesc);
		D3D11_INPUT_ELEMENT_DESC inputElementDesc;
		inputElementDesc.SemanticName = signatureParamaterDesc.SemanticName;
		inputElementDesc.SemanticIndex = signatureParamaterDesc.SemanticIndex;
		inputElementDesc.AlignedByteOffset = offset;
		inputElementDesc.InputSlot = 0;
		inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElementDesc.InstanceDataStepRate = 0;

		if(signatureParamaterDesc.Mask == 1)
		{
			const std::string semName(inputElementDesc.SemanticName);
			if (semName == "SV_InstanceID")
				continue;
			if (semName == "SV_VertexID")
				continue;
			if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				inputElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				inputElementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				inputElementDesc.Format = DXGI_FORMAT_R32_UINT;
			offset += 4;
			
		}
		else if (signatureParamaterDesc.Mask <= 3)
		{
			if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				inputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				inputElementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				inputElementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			offset += 8;
		}
		else if (signatureParamaterDesc.Mask <= 7)
		{
			if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			offset += 12;
		}
		else if (signatureParamaterDesc.Mask <= 15)
		{
			if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (signatureParamaterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				inputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			offset += 16;
		}
		inputElementDescs.push_back(inputElementDesc);
	}
	ID3D11InputLayout* inputLayout;
	hr = gDevice->CreateInputLayout(inputElementDescs.data(), inputElementDescs.size(), data, size, &inputLayout);
	if(FAILED(hr))
	{
		ProfileReturnConst(hr);
	}

	int index = -1;

	if(freeVertexShaderLocations.size())
	{
		index = freeVertexShaderLocations.top();
		vShaders[index] = { tempVertexShader, inputLayout };
		freeVertexShaderLocations.pop();
	}
	else
	{
		index = vShaders.size();
		vShaders.push_back({ tempVertexShader, inputLayout });
		*vertexShaderID = index;
	}
	D3D11_SHADER_BUFFER_DESC sbd;
	for (unsigned int i = 0; i < shaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(i);
		srcb->GetDesc(&sbd);
		const int constBufferHandle = CreateConstantBuffer(sbd.Size);
		if(constBufferHandle < 0)
		{
			reflection->Release();
			ProfileReturnConst(-1)
		}
		vShaders[index].constBufferNameToHandleAndBindSlot[std::string(sbd.Name)].handle = constBufferHandle;
	}
	for (unsigned int i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC sibd;
		reflection->GetResourceBindingDesc(i, &sibd);
		if(sibd.Type == D3D_SIT_CBUFFER)
		{
			vShaders[index].constBufferNameToHandleAndBindSlot[std::string(sibd.Name)].bindSlot = sibd.BindPoint;
		}
	}

	reflection->Release();
	ProfileReturnConst(hr);
}

HRESULT GraphicResourceHandler::CreatePixelShader(ID3D11Device* gDevice, void* data, size_t size, int *pixelShaderID, ShaderSettings* reflectionOut) {

	StartProfile;

	ID3D11PixelShader* tempPixelShader = nullptr;

	if (reflectionOut)
	{
		ID3D11ShaderReflection* reflection = nullptr;
		HRESULT hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
		if (FAILED(hr))
		{
		//	Utilz::Console::Print("Failed to reflect pixel shader.\n");
			ProfileReturnConst(hr);
		}
		D3D11_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);
		ShaderSettings settings;
		for (unsigned int i = 0; i < shaderDesc.ConstantBuffers; ++i)
		{
			D3D11_SHADER_BUFFER_DESC sbd;
			ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(i);
			srcb->GetDesc(&sbd);
			auto& cbInfo = settings.bufferNameToBufferInfo[std::string(sbd.Name)];
			cbInfo.size = sbd.Size;
			cbInfo.bindSlot = i;
			for (unsigned int j = 0; j < sbd.Variables; j++)
			{
				ID3D11ShaderReflectionVariable* var = srcb->GetVariableByIndex(i);
				D3D11_SHADER_VARIABLE_DESC svd;
				var->GetDesc(&svd);
				auto& varInfo = cbInfo.variables[std::string(svd.Name)];
				varInfo.size = svd.Size;
				varInfo.offset = svd.StartOffset;
			}
		}
		for (unsigned int i = 0; i < shaderDesc.BoundResources; ++i)
		{
			D3D11_SHADER_INPUT_BIND_DESC sibd;
			reflection->GetResourceBindingDesc(i, &sibd);
			if (sibd.Type == D3D_SIT_TEXTURE || sibd.Type == D3D_SIT_STRUCTURED)
			{
				settings.textureNameToBindSlot[std::string(sibd.Name)] = sibd.BindPoint;
			}
		}
		reflection->Release();

		*reflectionOut = settings;
	}
	HRESULT hr = gDevice->CreatePixelShader(data, size, nullptr, &tempPixelShader);

	if (FAILED(hr)) {
		ProfileReturnConst(hr);
	}

	if (FAILED(hr))
	{
		ProfileReturnConst(hr);
	}
	if (freePixelShaderLocations.size() == 0)
	{
		pShaders.push_back({ tempPixelShader });
		*pixelShaderID = pShaders.size() - 1;
	}
	else
	{
		auto top = freePixelShaderLocations.top();
		pShaders[top].pixelShader = tempPixelShader;
		*pixelShaderID = top;
		freePixelShaderLocations.pop();
	}
	
	ProfileReturnConst(hr);
}

HRESULT SE::Graphics::GraphicResourceHandler::CreateComputeShader(ID3D11Device * gDevice, void * data, size_t size, int * computeShaderID)
{
	StartProfile;

	ID3D11ComputeShader* temp = nullptr;

	HRESULT hr = gDevice->CreateComputeShader(data, size, nullptr, &temp);

	if (FAILED(hr)) {
		ProfileReturnConst(hr);
	}

	if (FAILED(hr))
	{
		ProfileReturnConst(hr);
	}
	if (freeComputeShaderLocations.size() == 0)
	{
		cShaders.push_back({ temp });
		*computeShaderID = cShaders.size() - 1;
	}
	else
	{
		auto top = freeComputeShaderLocations.top();
		cShaders[top].shader = temp;
		*computeShaderID = top;
		freeComputeShaderLocations.pop();
	}

	ProfileReturnConst(hr);
}

HRESULT SE::Graphics::GraphicResourceHandler::CreateGeometryShader(ID3D11Device * gDevice, void * data, size_t size, int * geometryShaderID)
{
	StartProfile;
	HRESULT hr = S_OK;
	ID3D11GeometryShader* tempGeometryShader = nullptr;
	hr = gDevice->CreateGeometryShader(data, size, nullptr, &tempGeometryShader);

	if (FAILED(hr))
	{
		//Utilz::Console::Print("Geometry Shader Error: Geometry Shader could not be created");
		ProfileReturnConst(hr);
	}
	if (freeGeometryShaderLocations.size() == 0)
	{
		gShaders.push_back({ tempGeometryShader });
		*geometryShaderID = gShaders.size() - 1;
	}
	else
	{
		auto top = freeGeometryShaderLocations.top();
		gShaders[top].geometryShader = tempGeometryShader;
		*geometryShaderID = top;
		freeGeometryShaderLocations.pop();
	}
	ProfileReturnConst(hr);
}

void GraphicResourceHandler::UnbindShaders() {

	StartProfile;

	// Define null pointers for each type
	ID3D11InputLayout* nullInputLayout = nullptr;
	ID3D11VertexShader* nullVS = nullptr;
	ID3D11GeometryShader* nullGS = nullptr;
	ID3D11PixelShader* nullPS = nullptr;

	// Set the null input layout
	gDeviceContext->IASetInputLayout(nullInputLayout);

	// Set the null vertex and pixel shaders
	gDeviceContext->VSSetShader(nullVS, nullptr, 0);
	gDeviceContext->GSSetShader(nullGS, nullptr, 0);
	gDeviceContext->PSSetShader(nullPS, nullptr, 0);

	StopProfile;
}

void GraphicResourceHandler::SetMaterial(int vertexID, int pixelID) {

	// Unbind shaders
	UnbindShaders();

	// Set the input layout
	gDeviceContext->IASetInputLayout(vShaders[vertexID].inputLayout);

	// Set the vertex and pixel shaders
	gDeviceContext->VSSetShader(vShaders[vertexID].vertexShader, nullptr, 0);
	gDeviceContext->PSSetShader(pShaders[pixelID].pixelShader, nullptr, 0);

}

void SE::Graphics::GraphicResourceHandler::SetGeometryShader(int geometryID)
{
	gDeviceContext->GSSetShader(gShaders[geometryID].geometryShader, nullptr, 0);
}

HRESULT GraphicResourceHandler::CreateVertexBuffer(void* inputData, size_t vertexCount, size_t stride, int *vertexBufferID)
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
	HRESULT _hr = gDevice->CreateBuffer(&_vertexBufferDesc, &_vertexData, &tempBuffer);


	if (FAILED(_hr))
	{
		ProfileReturnConst(_hr);
	}
	if (freeVertexBufferLocations.size() == 0)
	{
		vBuffers.push_back({ tempBuffer, vertexCount, stride });
		*vertexBufferID = vBuffers.size() - 1;
	}
	else
	{
		auto top = freeVertexBufferLocations.top();
		vBuffers[top].vertexBuffer = tempBuffer;
		vBuffers[top].vertexCount = vertexCount;
		vBuffers[top].stride = stride;
		*vertexBufferID = top;
		freeVertexBufferLocations.pop();
	}
	ProfileReturnConst(_hr);
}

void GraphicResourceHandler::SetVertexBuffer(int vertexBufferID)
{
	UINT32 vertexSize = vBuffers[vertexBufferID].stride;
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &vBuffers[vertexBufferID].vertexBuffer, &vertexSize, &offset);
}

void GraphicResourceHandler::RemoveVertexBuffer(int vertexBufferID)
{
	vBuffers[vertexBufferID].vertexBuffer->Release();
	vBuffers[vertexBufferID].vertexBuffer = nullptr;
	freeVertexBufferLocations.push(vertexBufferID);
}

size_t GraphicResourceHandler::GetVertexCount(int vertexBufferID) const
{
	return vBuffers[vertexBufferID].vertexCount;
}




int GraphicResourceHandler::CreateConstantBuffer(size_t size)
{
	StartProfile;
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	//skapar constant buffer
	ConstantBuffer tempBuffer;
	HRESULT hr = gDevice->CreateBuffer(&bufferDesc, nullptr, &tempBuffer.constBuffer);
	tempBuffer.size = size;
	int handle = -1;
	if (FAILED(hr))
	{
		ProfileReturnConst(-1);
	}
	if (freeConstantBufferLocations.size() == 0)
	{
		cBuffers.push_back(tempBuffer);
		handle = cBuffers.size() - 1;
	}
	else
	{
		const auto top = freeConstantBufferLocations.top();
		cBuffers[top] = tempBuffer;
		handle = top;
		freeConstantBufferLocations.pop();
	}

	ProfileReturnConst(handle);
}

int GraphicResourceHandler::GetVSConstantBufferByName(int vertexShaderHandle, const SE::Utilz::GUID& bufferName, int* bindSlot)
{
	StartProfile;
	auto f = vShaders[vertexShaderHandle].constBufferNameToHandleAndBindSlot.find(bufferName);
	if (f == vShaders[vertexShaderHandle].constBufferNameToHandleAndBindSlot.end())
		ProfileReturnConst(-1);
	if (bindSlot)
		*bindSlot = f->second.bindSlot;
	ProfileReturnConst(f->second.handle);

}

void GraphicResourceHandler::BindVSConstantBuffer(int constBufferHandle, int bindSlot)
{
	gDeviceContext->VSSetConstantBuffers(bindSlot, 1, &cBuffers[constBufferHandle].constBuffer);
}

void GraphicResourceHandler::BindConstantBuffer(ShaderStage shaderStage, int constBufferHandle, int bindSlot)
{
	switch(shaderStage)
	{
	case ShaderStage::VERTEX:
		gDeviceContext->VSSetConstantBuffers(bindSlot, 1, &cBuffers[constBufferHandle].constBuffer);
		break;
	case ShaderStage::GEOMETRY:
		gDeviceContext->GSSetConstantBuffers(bindSlot, 1, &cBuffers[constBufferHandle].constBuffer);
		break;
	case ShaderStage::PIXEL:
		gDeviceContext->PSSetConstantBuffers(bindSlot, 1, &cBuffers[constBufferHandle].constBuffer);
		break;
	case ShaderStage::COMPUTE:
		gDeviceContext->CSSetConstantBuffers(bindSlot, 1, &cBuffers[constBufferHandle].constBuffer);
		break;
	}
}


HRESULT GraphicResourceHandler::UpdateConstantBuffer(const void* data, size_t size, int id)
{
	StartProfile;
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HRESULT hr = gDeviceContext->Map(cBuffers[id].constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
		ProfileReturnConst(hr);
	memcpy(mappedData.pData, data, size);
	gDeviceContext->Unmap(cBuffers[id].constBuffer, 0);
	ProfileReturnConst(hr);
}



void GraphicResourceHandler::RemoveConstantBuffer(int constBufferID)
{
	cBuffers[constBufferID].constBuffer->Release();
	cBuffers[constBufferID].constBuffer = nullptr;
	freeConstantBufferLocations.push(constBufferID);
}

int GraphicResourceHandler::CreateShaderResourceView(void* textureData, const TextureDesc& description)
{
	
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = description.width;
	desc.Height = description.height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	ID3D11Texture2D* texture;
	D3D11_SUBRESOURCE_DATA d;
	d.pSysMem = textureData;
	d.SysMemPitch = description.width * 4;
	d.SysMemSlicePitch = 0;
	HRESULT hr = gDevice->CreateTexture2D(&desc, &d, &texture);
	if (FAILED(hr))
	{
		return -1;
	}

	ID3D11ShaderResourceView* srv;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = gDevice->CreateShaderResourceView(texture, &srvDesc, &srv);
	if(FAILED(hr))
	{
		return -1;
	}
	texture->Release();

	int index = -1;
	if (freeSRVIndices.size() > 0)
	{
		index = freeSRVIndices.top();
		freeSRVIndices.pop();
		shaderResourceViews[index] = srv;
	}
	else
	{
		index = shaderResourceViews.size();
		shaderResourceViews.push_back(srv);
	}

	return index;
}

void GraphicResourceHandler::RemoveShaderResourceView(int id)
{
	shaderResourceViews[id]->Release();
	shaderResourceViews[id] = nullptr;
	freeSRVIndices.push(id);

}

void GraphicResourceHandler::BindShaderResourceView(int id, int slot)
{
	if(shaderResourceViews.size() > id && shaderResourceViews[id] != nullptr)
		gDeviceContext->PSSetShaderResources(slot, 1, &shaderResourceViews[id]);
}

HRESULT GraphicResourceHandler::CreateSamplerState()
{
	StartProfile;
	D3D11_SAMPLER_DESC samplDesc;
	ZeroMemory(&samplDesc, sizeof(D3D11_SAMPLER_DESC));

	samplDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplDesc.MaxAnisotropy = 4;
	samplDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplDesc.MinLOD = 0;
	samplDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = gDevice->CreateSamplerState(&samplDesc, &sampleState);
	if (FAILED(hr))
	{
		ProfileReturnConst(hr);
	}
	//Set the sampler state
	gDeviceContext->PSSetSamplers(0, 1, &sampleState);
	ProfileReturnConst(hr);
}

int GraphicResourceHandler::CreateDynamicVertexBuffer(size_t bytewidth, size_t vertexByteSize, void* initialData, size_t initialDataSize)
{
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = bytewidth;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DYNAMIC;

	ID3D11Buffer* vBuffer;	
	HRESULT hr = gDevice->CreateBuffer(&bd, nullptr, &vBuffer);
	if (FAILED(hr))
		return -1;
	if (initialData && initialDataSize)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		gDeviceContext->Map(vBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, initialData, initialDataSize);
		gDeviceContext->Unmap(vBuffer, 0);
	}
	const size_t index = vBuffers.size();
	const size_t vertexCount = initialDataSize / vertexByteSize;
	vBuffers.push_back({ vBuffer, vertexCount, vertexByteSize });
	return index;
}

int GraphicResourceHandler::UpdateDynamicVertexBuffer(int handle, void* data, size_t totalSize, size_t sizePerElement)
{
	const size_t vertexCount = totalSize / sizePerElement;
	ID3D11Buffer* vBuffer = vBuffers[handle].vertexBuffer;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	gDeviceContext->Map(vBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, data, totalSize);
	gDeviceContext->Unmap(vBuffer, 0);

	vBuffers[handle].vertexCount = vertexCount;
	vBuffers[handle].stride = sizePerElement;
	return 0;
}

int GraphicResourceHandler::CreateRenderTargetView(int textureHandle, int* renderTargetViewHandle)
{
	int status = -1;

	ID3D11RenderTargetView* renderTargetView;
	HRESULT hr = gDevice->CreateRenderTargetView(texture2Ds[textureHandle], NULL, &renderTargetView);

	if (!FAILED(hr))
	{
		status = 0;

		renderTargetViews.push_back(renderTargetView);
		if (renderTargetViewHandle != nullptr)
			*renderTargetViewHandle = renderTargetViews.size() - 1;
	}

	return status;
}

int GraphicResourceHandler::CreateTexture2D(const D3D11_TEXTURE2D_DESC& description, int& textureHandle, bool isBloomBuffer)
{
	int status = -1;

	ID3D11Texture2D* texture2D;
	HRESULT hr = gDevice->CreateTexture2D(&description, NULL, &texture2D);

	if (!FAILED(hr))
	{
		status = 0;

		texture2Ds.push_back(texture2D);
		textureHandle = texture2Ds.size() - 1;

		if (isBloomBuffer)
			bloomBufferTextureHandle = textureHandle;
	}

	return status;
}

int GraphicResourceHandler::CreateCustomShaderResourceView(const D3D11_SHADER_RESOURCE_VIEW_DESC& description, int textureHandle, int& shaderResourceViewHandle, ID3D11Texture2D* texture)
{
	int status = -1;

	ID3D11ShaderResourceView* shaderResourceView;
	HRESULT hr = gDevice->CreateShaderResourceView((texture == nullptr) ? texture2Ds[textureHandle] : texture, &description, &shaderResourceView);

	if (!FAILED(hr))
	{
		status = 0;

		shaderResourceViews.push_back(shaderResourceView);
		shaderResourceViewHandle = shaderResourceViews.size() - 1;
	}

	return status;
}

int GraphicResourceHandler::CreateUnorderedAccessView(int textureHandle, int& unorderedAccessViewHandle)
{
	int status = -1;

	D3D11_UNORDERED_ACCESS_VIEW_DESC description;
	description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	description.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	description.Texture2D.MipSlice = 0;

	ID3D11UnorderedAccessView* unorderedAccessView;
	HRESULT hr = gDevice->CreateUnorderedAccessView(texture2Ds[textureHandle], &description, &unorderedAccessView);

	if (!FAILED(hr))
	{
		status = 0;

		unorderedAccessViews.push_back(unorderedAccessView);
		unorderedAccessViewHandle = unorderedAccessViews.size() - 1;
	}

	return status;
}

void GraphicResourceHandler::SetCompute(int computeID)
{
	gDeviceContext->CSSetShader(cShaders[computeID].shader, nullptr, 0);
}

ID3D11UnorderedAccessView* GraphicResourceHandler::GetUnorderedAccessView(int uavID)
{
	return unorderedAccessViews[uavID];
}

ID3D11RenderTargetView* GraphicResourceHandler::GetRenderTargetView(int rtvID)
{
	return renderTargetViews[rtvID];
}

ID3D11Texture2D* GraphicResourceHandler::GetBloomBufferTexture()
{
	return texture2Ds[bloomBufferTextureHandle];
}