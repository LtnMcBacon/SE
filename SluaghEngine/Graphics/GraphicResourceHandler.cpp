
#include <Graphics/GraphicResourceHandler.h>

#include <Utilz\Console.h>
#include <Profiler.h>


#include <d3dcompiler.h>
#pragma comment(lib, "D3Dcompiler.lib")

using namespace SE::Graphics;
using namespace SE::Utilz;
using namespace std;

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

	// Release Samplerstate
	if (sampleState != nullptr)
	{
		sampleState->Release();
		sampleState = nullptr;
	}
	for(auto& srv : shaderResourceViews)
	{
		if (srv)
			srv->Release();
	}
}

HRESULT GraphicResourceHandler::CreateVertexShader(ID3D11Device* gDevice, void* data, size_t size, int *vertexShaderID) {

	StartProfile;

	ID3D11InputLayout* tempInputLayout = nullptr;
	ID3D11VertexShader*	tempVertexShader = nullptr;

	HRESULT hr = S_OK;

	// We require three inputs (IN THE FOLLOWING ORDER):
	// POSITION
	// NORMAL
	// TEXCOORD
	D3D11_INPUT_ELEMENT_DESC vertexInputLayout[3];

	/*ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Asset\\SimpleVS.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		Console::Print("Vertex Shader Error: Vertex Shader could not be compiled or loaded from file");

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		ProfileReturnConst(hr);
	}*/

	hr = gDevice->CreateVertexShader(data, size, nullptr, &tempVertexShader);

	if (FAILED(hr)) {

		Console::Print("Vertex Shader Error: Vertex Shader could not be created");
		ProfileReturnConst(hr);
	}

	//Create the input layout with the help of shader reflection
	ID3D11ShaderReflection* reflection;
	hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
	if (FAILED(hr))
	{
		Console::Print("Failed to reflect vertex shader.\n");
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
		Utilz::Console::Print("Failed to create input layout.\n");
		ProfileReturnConst(hr);
	}
	reflection->Release();
	if(freeVertexShaderLocations.size())
	{
		const size_t index = freeVertexShaderLocations.top();
		vShaders[index] = { tempVertexShader, inputLayout };
		freeVertexShaderLocations.pop();
		*vertexShaderID = index;
	}
	else
	{
		const size_t index = vShaders.size();
		vShaders.push_back({ tempVertexShader, inputLayout });
		*vertexShaderID = index;
	}
	ProfileReturnConst(hr);

	/*vertexInputLayout[0].SemanticName = "POSITION";
	vertexInputLayout[0].SemanticIndex = 0;
	vertexInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexInputLayout[0].InputSlot = 0;
	vertexInputLayout[0].AlignedByteOffset = 0;
	vertexInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[0].InstanceDataStepRate = 0;

	vertexInputLayout[1].SemanticName = "NORMAL";
	vertexInputLayout[1].SemanticIndex = 0;
	vertexInputLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexInputLayout[1].InputSlot = 0;
	vertexInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[1].InstanceDataStepRate = 0;

	vertexInputLayout[2].SemanticName = "TEXCOORD";
	vertexInputLayout[2].SemanticIndex = 0;
	vertexInputLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexInputLayout[2].InputSlot = 0;
	vertexInputLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexInputLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[2].InstanceDataStepRate = 0;

	int inputLayoutSize = sizeof(vertexInputLayout) / sizeof(vertexInputLayout[0]);
	gDevice->CreateInputLayout(vertexInputLayout, inputLayoutSize, data, size, &tempInputLayout);

	if (FAILED(hr)) {

		Console::Print("Input Layout Error: Shader Input Layout could not be created");

		ProfileReturnConst(hr);
	}

	if (FAILED(hr))
	{
		ProfileReturnConst(hr);
	}
	if (freeVertexShaderLocations.size() == 0)
	{
		vShaders.push_back({ tempVertexShader, tempInputLayout });
		*vertexShaderID = vShaders.size() - 1;
	}
	else
	{
		auto top = freeVertexShaderLocations.top();
		vShaders[top].vertexShader = tempVertexShader;
		vShaders[top].inputLayout = tempInputLayout;
		*vertexShaderID = top;
		freeVertexShaderLocations.pop();
	}
*/
	ProfileReturnConst(hr);
}

HRESULT GraphicResourceHandler::CreatePixelShader(ID3D11Device* gDevice, void* data, size_t size, int *pixelShaderID, ShaderSettings* reflectionOut) {

	StartProfile;

	ID3D11PixelShader* tempPixelShader = nullptr;

	HRESULT hr = S_OK;

	//ID3DBlob* psBlob = nullptr;
	//ID3DBlob* psErrorBlob = nullptr;

	//hr = D3DCompileFromFile(
	//	L"Asset\\SimplePS.hlsl",
	//	nullptr,
	//	nullptr,
	//	"PS_main",
	//	"ps_5_0",
	//	D3DCOMPILE_OPTIMIZATION_LEVEL3,
	//	0,
	//	&psBlob,
	//	&psErrorBlob
	//);

	//if (FAILED(hr)) {

	//	Console::Print("Pixel Shader Error: Pixel Shader could not be compiled or loaded from file");

	//	if (psErrorBlob) {

	//		OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
	//		psErrorBlob->Release();
	//	}

	//	ProfileReturnConst(hr);
	//}
	if (reflectionOut)
	{
		ID3D11ShaderReflection* reflection = nullptr;
		hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
		if (FAILED(hr))
		{
			Console::Print("Failed to reflect pixel shader.\n");
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
			auto& cbInfo = settings.bufferNameToBufferInfo[sbd.Name];
			cbInfo.size = sbd.Size;
			cbInfo.bindSlot = i;
			for (unsigned int j = 0; j < sbd.Variables; j++)
			{
				ID3D11ShaderReflectionVariable* var = srcb->GetVariableByIndex(i);
				D3D11_SHADER_VARIABLE_DESC svd;
				var->GetDesc(&svd);
				auto& varInfo = cbInfo.variables[svd.Name];
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
				settings.textureNameToBindSlot[sibd.Name] = sibd.BindPoint;
			}
		}
		reflection->Release();

		*reflectionOut = settings;
	}
	hr = gDevice->CreatePixelShader(data, size, nullptr, &tempPixelShader);

	if (FAILED(hr)) {

		Console::Print("Pixel Shader Error: Pixel Shader could not be created");
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

HRESULT GraphicResourceHandler::CreateConstantBuffer(size_t size, TargetOffset& inTargetOffset, int *constBufferID)
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
	HRESULT hr = gDevice->CreateBuffer(&bufferDesc, NULL, &tempBuffer.constBuffer);
	tempBuffer.size = size;

	if (FAILED(hr))
	{
		ProfileReturnConst(hr);
	}
	if (freeConstantBufferLocations.size() == 0)
	{
		cBuffers.push_back(tempBuffer);
		targetOffset.push_back(inTargetOffset);
		*constBufferID = cBuffers.size() - 1;
	}
	else
	{
		auto top = freeConstantBufferLocations.top();
		cBuffers[top] = tempBuffer;
		targetOffset[top] = inTargetOffset;
		*constBufferID = top;
		freeConstantBufferLocations.pop();
	}

	ProfileReturnConst(hr);
}

void GraphicResourceHandler::BindConstantBuffer(int constBufferID)
{
	if (targetOffset.at(constBufferID).shaderTarget[0] == true)
	{
		gDeviceContext->VSSetConstantBuffers(targetOffset.at(constBufferID).offset[0], 1, &cBuffers[constBufferID].constBuffer);
	}
	if (targetOffset.at(constBufferID).shaderTarget[1] == true)
	{
		gDeviceContext->GSSetConstantBuffers(targetOffset.at(constBufferID).offset[1], 1, &cBuffers[constBufferID].constBuffer);
	}
	if (targetOffset.at(constBufferID).shaderTarget[2] == true)
	{
		gDeviceContext->PSSetConstantBuffers(targetOffset.at(constBufferID).offset[2], 1, &cBuffers[constBufferID].constBuffer);
	}
}

void GraphicResourceHandler::SetConstantBuffer(void* inData, int constBufferID)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = gDeviceContext->Map(cBuffers[constBufferID].constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, inData, cBuffers[constBufferID].size);
	gDeviceContext->Unmap(cBuffers[constBufferID].constBuffer, 0);

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
	HRESULT hr = S_OK;
	hr = gDevice->CreateTexture2D(&desc, &d, &texture);
	if (FAILED(hr))
	{
		Utilz::Console::Print("Failed to create texture from data.\n");
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
		Utilz::Console::Print("Failed to create shader resource view from texture.\n");
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
