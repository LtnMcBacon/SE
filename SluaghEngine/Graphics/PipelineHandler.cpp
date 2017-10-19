#include "PipelineHandler.h"
#include "Profiler.h"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <vector>
SE::Graphics::PipelineHandler::PipelineHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;
}

SE::Graphics::PipelineHandler::~PipelineHandler()
{
	//lots of loops...
	for (auto& r : vertexBuffers)
		r.second->Release();
	for (auto& r : indexBuffers)
		r.second->Release();
	for (auto& r : inputLayouts)
		r.second->Release();
	for (auto& r : vertexShaders)
		r.second->Release();
	for (auto& r : geometryShaders)
		r.second->Release();
	for (auto& r : pixelShaders)
		r.second->Release();
	for (auto& r : computeShaders)
		r.second->Release();
	for (auto& r : constantBuffers)
		r.second.buffer->Release();
	for (auto& r : shaderResourceViews)
		r.second->Release();
	for (auto& r : renderTargetViews)
		r.second->Release();
	for (auto& r : samplerStates)
		r.second->Release();
	for (auto& r : blendStates)
		r.second->Release();
	for (auto& r : rasterizerStates)
		r.second->Release();
	for (auto& r : depthStencilStates)
		r.second->Release();
}

void SE::Graphics::PipelineHandler::CreateVertexBuffer(const Utilz::GUID& id, void* data, size_t vertexCount,
	size_t stride, bool dynamic)
{
	StartProfile;
	const auto exists = vertexBuffers.find(id);
	if (exists != vertexBuffers.end())
		ProfileReturnVoid;

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = stride * vertexCount;
	bd.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	bd.MiscFlags = 0;
	bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bd.StructureByteStride = stride;

	ID3D11Buffer* buffer;
	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = data;
	sd.SysMemPitch = 0;
	sd.SysMemSlicePitch = 0;
	HRESULT hr = device->CreateBuffer(&bd, &sd, &buffer);
	if (FAILED(hr))
		throw std::exception("Failed to create Vertex Buffer");

	vertexBuffers[id] = buffer;
	ProfileReturnVoid;
}

void SE::Graphics::PipelineHandler::DestroyVertexBuffer(const Utilz::GUID& id)
{
	auto exists = vertexBuffers.find(id);
	if (exists == vertexBuffers.end())
		return;
	exists->second->Release();
	vertexBuffers.erase(exists);
}

void SE::Graphics::PipelineHandler::CreateIndexBuffer(const Utilz::GUID& id, void* data, size_t indexCount,
	size_t indexSize)
{
	const auto exists = indexBuffers.find(id);
	if (exists != indexBuffers.end())
		return;
	
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = indexSize * indexCount;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.StructureByteStride = indexSize;

	ID3D11Buffer* buffer;
	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = data;
	sd.SysMemPitch = 0;
	sd.SysMemSlicePitch = 0;
	HRESULT hr = device->CreateBuffer(&bd, &sd, &buffer);
	if (FAILED(hr))
		throw std::exception("Failed to create Index Buffer");

	indexBuffers[id] = buffer;
}

void SE::Graphics::PipelineHandler::DestroyIndexBuffer(const Utilz::GUID& id)
{
	auto exists = indexBuffers.find(id);
	if (exists == indexBuffers.end())
		return;
	exists->second->Release();
	indexBuffers.erase(exists);
}

void SE::Graphics::PipelineHandler::CreateVertexShader(const Utilz::GUID& id, void* data, size_t size)
{
	StartProfile;
	const auto exists = vertexShaders.find(id);
	if (exists != vertexShaders.end())
		ProfileReturnVoid;
	ID3D11VertexShader* vs;
	ID3D11InputLayout* inputLayout;

	HRESULT hr = device->CreateVertexShader(data, size, nullptr, &vs);
	if (FAILED(hr))
		throw std::exception("Failed to create vertex shader");

	vertexShaders[id] = vs;
	//Create the input layout with the help of shader reflection
	ID3D11ShaderReflection* reflection;
	hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
	
	if (FAILED(hr))
		throw std::exception("Failed to reflect vertex shader.");

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
	uint32_t offset = 0;
	for (uint32_t i = 0; i < shaderDesc.InputParameters; ++i)
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

		if (signatureParamaterDesc.Mask == 1)
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
	
	hr = device->CreateInputLayout(inputElementDescs.data(), inputElementDescs.size(), data, size, &inputLayout);
	if (FAILED(hr))
		throw std::exception("Failed to create input layout");

	inputLayouts[id] = inputLayout;

	for (unsigned int i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC sibd;
		reflection->GetResourceBindingDesc(i, &sibd);
		if (sibd.Type == D3D_SIT_CBUFFER)
		{
			ConstantBuffer cb;
			cb.bindSlot = sibd.BindPoint;
			//Can't get the size from the RBD, can't get bindslot from the SBD...	
			//Find the sbd with the same name to get the size.
			for (unsigned int i = 0; i < shaderDesc.ConstantBuffers; ++i)
			{
				D3D11_SHADER_BUFFER_DESC sbd;
				ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(i);
				srcb->GetDesc(&sbd);
				if (sbd.Name == sibd.Name)
				{
					D3D11_BUFFER_DESC bufDesc;
					bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
					bufDesc.StructureByteStride = 0;
					bufDesc.ByteWidth = sbd.Size;
					bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
					bufDesc.MiscFlags = 0;
					bufDesc.Usage = D3D11_USAGE_DYNAMIC;

					hr = device->CreateBuffer(&bufDesc, nullptr, &cb.buffer);
					if (FAILED(hr))
						throw std::exception("Failed to create constant buffer.");
					const Utilz::GUID cbNameGuid(sbd.Name);
					const Utilz::GUID combined = id + cbNameGuid;
					constantBuffers[combined] = cb;
					break;
				}
			}
		}
	}
	reflection->Release();
	ProfileReturnVoid;
}

void SE::Graphics::PipelineHandler::CreateGeometryShader(const Utilz::GUID& id, void* data, size_t size)
{
	auto exists = geometryShaders.find(id);
	if (exists != geometryShaders.end())
		return;

	ID3D11GeometryShader* gs;
	HRESULT hr = device->CreateGeometryShader(data, size, nullptr, &gs);
	if (FAILED(hr))
		throw std::exception("Could not create geometry shader.");

	geometryShaders[id] = gs;
}

void SE::Graphics::PipelineHandler::CreatePixelShader(const Utilz::GUID& id, void* data, size_t size)
{
	auto exists = pixelShaders.find(id);
	if (exists != pixelShaders.end())
		return;

	ID3D11PixelShader* ps;
	HRESULT hr = device->CreatePixelShader(data, size, nullptr, &ps);
	if (FAILED(hr))
		throw std::exception("Could not create pixel shader");

	pixelShaders[id] = ps;

	ID3D11ShaderReflection* reflection;
	hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
	if (FAILED(hr))
		throw std::exception("Failed to reflect vertex shader.");

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	for(int i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC sibd;
		reflection->GetResourceBindingDesc(i, &sibd);
		if(sibd.Type == D3D_SIT_TEXTURE)
		{
			const Utilz::GUID bindGuid(sibd.Name);
			const Utilz::GUID combinedGuid = id + bindGuid;
			pixelShaderAndResourceNameToBindSlot[combinedGuid] = sibd.BindPoint;
		}
	}

	reflection->Release();
}

void SE::Graphics::PipelineHandler::CreateComputeShader(const Utilz::GUID& id, void* data, size_t size)
{
	const auto exists = computeShaders.find(id);
	if (exists != computeShaders.end())
		return;

	ID3D11ComputeShader* cs;
	HRESULT hr = device->CreateComputeShader(data, size, nullptr, &cs);
	if (FAILED(hr))
		throw std::exception("Failed to create compute shader");

	computeShaders[id] = cs;
}

void SE::Graphics::PipelineHandler::DestroyVertexShader(const Utilz::GUID& id)
{
	auto exists = vertexShaders.find(id);
	if (exists == vertexShaders.end())
		return;
	exists->second->Release();
	vertexShaders.erase(exists);
}

void SE::Graphics::PipelineHandler::DestroyGeometryShader(const Utilz::GUID& id)
{
	auto exists = geometryShaders.find(id);
	if (exists == geometryShaders.end())
		return;
	exists->second->Release();
	geometryShaders.erase(exists);
}

void SE::Graphics::PipelineHandler::DestroyPixelShader(const Utilz::GUID& id)
{
	auto exists = pixelShaders.find(id);
	if (exists == pixelShaders.end())
		return;
	exists->second->Release();
	pixelShaders.erase(exists);
}

void SE::Graphics::PipelineHandler::DestroyComputeShader(const Utilz::GUID& id)
{
	auto exists = computeShaders.find(id);
	if (exists == computeShaders.end())
		return;
	exists->second->Release();
	computeShaders.erase(exists);
}
