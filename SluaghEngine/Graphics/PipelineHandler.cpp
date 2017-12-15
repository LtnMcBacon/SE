#include "PipelineHandler.h"
#include <Profiler.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <vector>
enum RETURN_CODES
{
	SUCCESS = 0,
	EXISTS = 1,
	NOT_FOUND = -1,
	DEVICE_FAIL = -2
};

static const char* fullscreenQuadVS = 
"void VS_main( uint VertexID : SV_VertexID, out float4 oPosH : SV_POSITION, out float2 oTexC : TEXCOORD ) \
{ \
	oPosH.x = (VertexID == 2) ? 3.0f : -1.0f; \
	oPosH.y = (VertexID == 0) ? -3.0f : 1.0f; \
	oPosH.zw = 1.0f; \
 \
	oTexC = oPosH.xy * float2(0.5f, -0.5f) + 0.5f; \
}";

static const char* MultiPS =
"Texture2D gTexture : register(t0); \
SamplerState gTriLinearSam : register(s0); \
float4 PS_main(float4 posH : SV_POSITION, float2 texC : TEXCOORD) : SV_TARGET \
{ \
	return gTexture.Sample(gTriLinearSam, texC).rgba; \
}";

static const char* SinglePS = 
"Texture2D gTexture : register(t0); \
SamplerState gTriLinearSam : register(s0); \
float4 PS_main(float4 posH : SV_POSITION, float2 texC : TEXCOORD) : SV_TARGET \
{ \
	return gTexture.Sample(gTriLinearSam, texC).rrrr; \
}";



SE::Graphics::PipelineHandler::PipelineHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11RenderTargetView* backbuffer, ID3D11DepthStencilView* dsv)
{
	this->device = device;
	this->deviceContext = deviceContext;
	renderTargetViews["backbuffer"] = { backbuffer, {0.0f, 0.0f,1.0f,0.0f} };
	depthStencilViews["backbuffer"] = dsv;
	//Create nullptrs for IDs that are ""
	vertexBuffers[Utilz::GUID()].buffer = nullptr;
	vertexBuffers[Utilz::GUID()].stride = 0;
	indexBuffers[Utilz::GUID()].buffer = nullptr;
	indexBuffers[Utilz::GUID()].stride = 0;
	inputLayouts[Utilz::GUID()] = nullptr;
	vertexShaders[Utilz::GUID()] = { nullptr };
	geometryShaders[Utilz::GUID()] = { nullptr };
	pixelShaders[Utilz::GUID()] = { nullptr };
	computeShaders[Utilz::GUID()] = { nullptr };
	constantBuffers[Utilz::GUID()] = nullptr;
	shaderResourceViews[Utilz::GUID()] = nullptr;
	renderTargetViews[Utilz::GUID()] = { nullptr };
	samplerStates[Utilz::GUID()] = nullptr;
	blendStates[Utilz::GUID()] = nullptr;
	rasterizerStates[Utilz::GUID()] = nullptr;
	depthStencilStates[Utilz::GUID()] = nullptr;
	unorderedAccessViews[Utilz::GUID()] = { nullptr };
	ID3DBlob* blob;

	auto hr = D3DCompile(fullscreenQuadVS, strlen(fullscreenQuadVS), NULL, NULL, NULL, "VS_main", "vs_5_0", 0, 0, &blob, NULL);
	if (FAILED(hr))
		throw std::exception("Could not compile fullscreenQuadVS");

	PipelineHandler::CreateVertexShader("FullscreenQuad", blob->GetBufferPointer(), blob->GetBufferSize());
	blob->Release();

	hr = D3DCompile(MultiPS, strlen(MultiPS), NULL, NULL, NULL, "PS_main", "ps_5_0", 0, 0, &blob, NULL);
	if (FAILED(hr))
		throw std::exception("Could not compile MultiPS");

	PipelineHandler::CreatePixelShader("MultiPS", blob->GetBufferPointer(), blob->GetBufferSize());
	blob->Release(); 
	
	hr = D3DCompile(SinglePS, strlen(SinglePS), NULL, NULL, NULL, "PS_main", "ps_5_0", 0, 0, &blob, NULL);
	if (FAILED(hr))
		throw std::exception("Could not compile SinglePS");

	PipelineHandler::CreatePixelShader("SinglePS", blob->GetBufferPointer(), blob->GetBufferSize());
	blob->Release();
	

}

SE::Graphics::PipelineHandler::~PipelineHandler()
{
	//Remove resources that are not owned by the pipelinehandler (added by AddExisting...)
	for(auto b : manuallyAddedResources)
	{
		renderTargetViews.erase(b);
		shaderResourceViews.erase(b);
		depthStencilViews.erase(b);
	}
	//lots of loops...
	for (auto& r : vertexBuffers)
		if (r.second.buffer) r.second.buffer->Release();
	for (auto& r : indexBuffers)
		if (r.second.buffer) r.second.buffer->Release();
	for (auto& r : inputLayouts)
		if (r.second) r.second->Release();
	for (auto& r : vertexShaders)
		if (r.second.shader)r.second.shader->Release();
	for (auto& r : geometryShaders)
		if (r.second.shader)r.second.shader->Release();
	for (auto& r : pixelShaders)
		if (r.second.shader)r.second.shader->Release();
	for (auto& r : computeShaders)
		if (r.second.shader)r.second.shader->Release();
	for (auto& r : constantBuffers)
		if (r.second)r.second->Release();
	for (auto& r : shaderResourceViews)
		if (r.second)r.second->Release();
	for (auto& r : renderTargetViews)
		if (r.second.rtv && r.first != Utilz::GUID("backbuffer"))r.second.rtv->Release();
	for (auto& r : depthStencilViews)
		if (r.second && r.first != Utilz::GUID("backbuffer"))
			r.second->Release();
	for (auto& r : samplerStates)
		if (r.second)r.second->Release();
	for (auto& r : unorderedAccessViews)
		if (r.second.uav)r.second.uav->Release();
	for (auto& r : blendStates)
		if (r.second)r.second->Release();
	for (auto& r : rasterizerStates)
		if (r.second)r.second->Release();
	for (auto& r : depthStencilStates)
		if (r.second)r.second->Release();
}

int SE::Graphics::PipelineHandler::AddExistingRenderTargetView(const Utilz::GUID& id, void* rtv)
{
	ID3D11RenderTargetView* renderTargetView = (ID3D11RenderTargetView*)rtv;
	renderTargetViews[id] = { renderTargetView, {0.0f,0.0f,0.0f,0.0f} };
	manuallyAddedResources.insert(id);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::AddExistingDepthStencilView(const Utilz::GUID& id, void* dsv)
{
	depthStencilViews[id] = (ID3D11DepthStencilView*)dsv;
	manuallyAddedResources.insert(id);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::AddExisitingShaderResourceView(const Utilz::GUID& id, void* srv)
{
	shaderResourceViews[id] = (ID3D11ShaderResourceView*)srv;
	manuallyAddedResources.insert(id);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::MergeHandlers(IPipelineHandler * other)
{
	// TODO: Move everything from other to this. Then clear other.

	return 0;
}

int SE::Graphics::PipelineHandler::CreateVertexShaderFromSource(const Utilz::GUID& id, const std::string& sourceCode,
	const std::string& entrypoint, const std::string& shaderModel)
{
	const auto exists = vertexShaders.find(id);
	if (exists != vertexShaders.end())
		return EXISTS;
	ID3D10Blob* blob = nullptr;
	HRESULT hr = D3DCompile(sourceCode.c_str(), sourceCode.size(), nullptr, nullptr, nullptr, entrypoint.c_str(), shaderModel.c_str(), 0, 0, &blob, nullptr);
	if (FAILED(hr))
		return DEVICE_FAIL;
	int ret = CreateVertexShader(id, blob->GetBufferPointer(), blob->GetBufferSize());
	blob->Release();
	return ret;

}

int SE::Graphics::PipelineHandler::CreateVertexBuffer(const Utilz::GUID& id, void* data, size_t vertexCount,
	size_t stride, bool dynamic)
{
	const auto exists = vertexBuffers.find(id);
	if (exists != vertexBuffers.end())
		return EXISTS;

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = stride * vertexCount;
	bd.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	bd.MiscFlags = 0;
	bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bd.StructureByteStride = stride;
	HRESULT hr;
	ID3D11Buffer* buffer;
	if (data)
	{
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem = data;
		sd.SysMemPitch = 0;
		sd.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&bd, &sd, &buffer);
	}
	else
	{
		hr = device->CreateBuffer(&bd, nullptr, &buffer);
	}
	if (FAILED(hr))
		return DEVICE_FAIL;
	vertexBuffers[id].buffer = buffer;
	vertexBuffers[id].stride = stride;
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::UpdateDynamicVertexBuffer(const Utilz::GUID& id, void* data, size_t size)
{
	StartProfile;
	const auto find = vertexBuffers.find(id);
	if (find == vertexBuffers.end())
		ProfileReturnConst( NOT_FOUND);
	D3D11_MAPPED_SUBRESOURCE ms;
	deviceContext->Map(find->second.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, data, size);
	deviceContext->Unmap(find->second.buffer, 0);
	ProfileReturnConst(SUCCESS);
}

int SE::Graphics::PipelineHandler::DestroyVertexBuffer(const Utilz::GUID& id)
{
	auto exists = vertexBuffers.find(id);
	if (exists == vertexBuffers.end())
		return NOT_FOUND;
	exists->second.buffer->Release();
	vertexBuffers.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateIndexBuffer(const Utilz::GUID& id, void* data, size_t indexCount,
	size_t indexSize)
{
	const auto exists = indexBuffers.find(id);
	if (exists != indexBuffers.end())
		return EXISTS;

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
		return DEVICE_FAIL;

	indexBuffers[id].buffer = buffer;
	indexBuffers[id].stride = indexSize;
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateBuffer(const Utilz::GUID& id, void* data, size_t elementCount, size_t elementStride, size_t maxElements, uint32_t flags)
{
	if (flags & BIND_VERTEX)
	{
		const auto exists = vertexBuffers.find(id);
		if (exists != vertexBuffers.end())
			return EXISTS;
	}
	else if (flags & BIND_INDEX)
	{
		const auto exists = indexBuffers.find(id);
		if (exists != indexBuffers.end())
			return EXISTS;

	}
	else if (flags & BIND_CONSTANT)
	{
		const auto exists = constantBuffers.find(id);
		if (exists != constantBuffers.end())
			return EXISTS;
	}
	else
	{
		throw std::exception("Buffer must be bound as either VERTEX, INDEX or CONSTANT.");
	}
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = 0;
	if (flags & BufferFlags::BIND_CONSTANT) bd.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
	if (flags & BufferFlags::BIND_VERTEX) bd.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
	if (flags & BufferFlags::BIND_INDEX) bd.BindFlags |= D3D11_BIND_INDEX_BUFFER;
	if (flags & BufferFlags::BIND_STREAMOUT) bd.BindFlags |= D3D11_BIND_STREAM_OUTPUT;
	bd.ByteWidth = maxElements * elementStride;
	bd.CPUAccessFlags = 0;
	if (flags & BufferFlags::CPU_WRITE) bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (flags & BufferFlags::CPU_READ) bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bd.Usage = D3D11_USAGE_DEFAULT;
	if (flags & BufferFlags::DYNAMIC) bd.Usage = D3D11_USAGE_DYNAMIC;
	if (flags & BufferFlags::IMMUTABLE) bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	ID3D11Buffer* buffer;
	HRESULT hr;

	if (data)
	{
		void* dummy = operator new(elementStride * maxElements);
		memcpy(dummy, data, elementCount * elementStride);
		D3D11_SUBRESOURCE_DATA d;
		d.pSysMem = dummy;
		d.SysMemPitch = 0;
		d.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&bd, &d, &buffer);
		operator delete(dummy);
	}
	else
	{
		hr = device->CreateBuffer(&bd, nullptr, &buffer);
	}

	if (FAILED(hr))
		return DEVICE_FAIL;

	if (flags & BIND_VERTEX) {
		vertexBuffers[id].buffer = buffer;
		vertexBuffers[id].stride = elementStride;
	}
	else if (flags & BIND_INDEX)
	{
		indexBuffers[id].buffer = buffer;
		indexBuffers[id].stride = elementStride;
	}
	else if (flags & BIND_CONSTANT)
	{
		constantBuffers[id] = buffer;
	}
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyIndexBuffer(const Utilz::GUID& id)
{
	auto exists = indexBuffers.find(id);
	if (exists == indexBuffers.end())
		return NOT_FOUND;
	exists->second.buffer->Release();
	indexBuffers.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateViewport(const Utilz::GUID& id, const Viewport& viewport)
{
	const auto exists = viewports.find(id);
	if (exists != viewports.end())
		return EXISTS;
	D3D11_VIEWPORT vp;
	vp.Width = viewport.width;
	vp.Height = viewport.height;
	vp.MaxDepth = viewport.maxDepth;
	vp.MinDepth = viewport.minDepth;
	vp.TopLeftX = viewport.topLeftX;
	vp.TopLeftY = viewport.topLeftY;
	viewports[id] = vp;
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateVertexShader(const Utilz::GUID& id, void* data, size_t size)
{
	const auto exists = vertexShaders.find(id);
	if (exists != vertexShaders.end())
		return EXISTS;
	ID3D11VertexShader* vs;
	ID3D11InputLayout* inputLayout;

	HRESULT hr = device->CreateVertexShader(data, size, nullptr, &vs);
	if (FAILED(hr))
		return DEVICE_FAIL;

	vertexShaders[id] = { vs };
	//Create the input layout with the help of shader reflection
	ID3D11ShaderReflection* reflection;
	hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);

	if (FAILED(hr))
		return DEVICE_FAIL;

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
	if (inputElementDescs.size() > 0)
	{
		hr = device->CreateInputLayout(inputElementDescs.data(), inputElementDescs.size(), data, size, &inputLayout);
		if (FAILED(hr))
			return DEVICE_FAIL;

		inputLayouts[id] = inputLayout;
	}
	for (unsigned int i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC sibd;
		reflection->GetResourceBindingDesc(i, &sibd);
		if (sibd.Type == D3D_SIT_CBUFFER)
		{
			//Can't get the size from the RBD, can't get bindslot from the SBD...	
			//Find the sbd with the same name to get the size.
			for (unsigned int j = 0; j < shaderDesc.ConstantBuffers; ++j)
			{
				D3D11_SHADER_BUFFER_DESC sbd;
				ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(j);
				srcb->GetDesc(&sbd);
				Utilz::GUID name = std::string(sbd.Name);
				if (name == std::string(sibd.Name))
				{
					const auto cbExists = constantBuffers.find(name);
					if (cbExists == constantBuffers.end())
					{
						D3D11_BUFFER_DESC bufDesc;
						bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						bufDesc.StructureByteStride = 0;
						bufDesc.ByteWidth = sbd.Size;
						bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
						bufDesc.MiscFlags = 0;
						bufDesc.Usage = D3D11_USAGE_DYNAMIC;
						ID3D11Buffer* buffer;
						hr = device->CreateBuffer(&bufDesc, nullptr, &buffer);
						if (FAILED(hr))
							return DEVICE_FAIL;
						constantBuffers[name] = buffer;
					}
					vertexShaders[id].constantBuffers.push_back(name);
					const Utilz::GUID cbNameGuid(sbd.Name);
					const Utilz::GUID combined = id + cbNameGuid;
					shaderAndResourceNameToBindSlot[combined] = sibd.BindPoint;
					break;
				}
			}
		}
	}
	reflection->Release();
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateGeometryShader(const Utilz::GUID& id, void* data, size_t size)
{
	const auto exists = geometryShaders.find(id);
	if (exists != geometryShaders.end())
		return EXISTS;

	ID3D11GeometryShader* gs;
	HRESULT hr = device->CreateGeometryShader(data, size, nullptr, &gs);
	if (FAILED(hr))
		return DEVICE_FAIL;

	geometryShaders[id] = { gs };

	ID3D11ShaderReflection* reflection;
	hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
	if (FAILED(hr))
		return DEVICE_FAIL;

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	for (unsigned int i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC sibd;
		reflection->GetResourceBindingDesc(i, &sibd);
		if (sibd.Type == D3D_SIT_CBUFFER)
		{
			//Can't get the size from the RBD, can't get bindslot from the SBD...	
			//Find the sbd with the same name to get the size.
			for (unsigned int j = 0; j < shaderDesc.ConstantBuffers; ++j)
			{
				D3D11_SHADER_BUFFER_DESC sbd;
				ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(j);
				srcb->GetDesc(&sbd);
				Utilz::GUID name = std::string(sbd.Name);
				if (name == std::string(sibd.Name))
				{
					const auto cbExists = constantBuffers.find(name);
					if (cbExists == constantBuffers.end())
					{
						D3D11_BUFFER_DESC bufDesc;
						bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						bufDesc.StructureByteStride = 0;
						bufDesc.ByteWidth = sbd.Size;
						bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
						bufDesc.MiscFlags = 0;
						bufDesc.Usage = D3D11_USAGE_DYNAMIC;
						ID3D11Buffer* buffer;
						hr = device->CreateBuffer(&bufDesc, nullptr, &buffer);
						if (FAILED(hr))
							return DEVICE_FAIL;
						constantBuffers[name] = buffer;
						
					}
					geometryShaders[id].constantBuffers.push_back(name);
					const Utilz::GUID cbNameGuid(sbd.Name);
					const Utilz::GUID combined = id + cbNameGuid;
					shaderAndResourceNameToBindSlot[combined] = sibd.BindPoint;
					break;
				}
			}
		}
	}
	reflection->Release();
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateGeometryShaderStreamOut(const Utilz::GUID& id, void* data, size_t size)
{
	const auto exists = geometryShaders.find(id);
	if (exists != geometryShaders.end())
		return EXISTS;

	ID3D11ShaderReflection* reflection;
	HRESULT hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
	if (FAILED(hr))
		return NOT_FOUND;

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);
	std::vector<D3D11_SO_DECLARATION_ENTRY> SOEntries;
	for (int i = 0; i < shaderDesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC signatureParameterDesc;
		reflection->GetInputParameterDesc(i, &signatureParameterDesc);
		BYTE mask = signatureParameterDesc.Mask;
		int varCount = 0;
		while (mask)
		{
			if (mask & 0x01) varCount++;
			mask = mask >> 1;
		}

		D3D11_SO_DECLARATION_ENTRY sode;
		sode.SemanticName = signatureParameterDesc.SemanticName;
		sode.Stream = signatureParameterDesc.Stream;
		sode.OutputSlot = 0;
		sode.StartComponent = 0;
		sode.ComponentCount = varCount;
		sode.SemanticIndex = signatureParameterDesc.SemanticIndex;

		SOEntries.push_back(sode);
	}
	uint32_t bufferStrides = 0;
	for (auto& e : SOEntries)
		bufferStrides += e.ComponentCount * 4;

	ID3D11GeometryShader* gs;
	hr = device->CreateGeometryShaderWithStreamOutput(data, size, SOEntries.data(), SOEntries.size(), &bufferStrides, 1, D3D11_SO_NO_RASTERIZED_STREAM, nullptr, &gs);
	if (FAILED(hr))
		return DEVICE_FAIL;

	geometryShaders[id] = { gs };

	for (unsigned int i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC sibd;
		reflection->GetResourceBindingDesc(i, &sibd);
		if (sibd.Type == D3D_SIT_CBUFFER)
		{
			//Can't get the size from the RBD, can't get bindslot from the SBD...	
			//Find the sbd with the same name to get the size.
			for (unsigned int j = 0; j < shaderDesc.ConstantBuffers; ++j)
			{
				D3D11_SHADER_BUFFER_DESC sbd;
				ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(j);
				srcb->GetDesc(&sbd);
				Utilz::GUID name = std::string(sbd.Name);
				if (name == std::string(sibd.Name))
				{
					const auto cbExists = constantBuffers.find(name);
					if (cbExists == constantBuffers.end())
					{
						D3D11_BUFFER_DESC bufDesc;
						bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						bufDesc.StructureByteStride = 0;
						bufDesc.ByteWidth = sbd.Size;
						bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
						bufDesc.MiscFlags = 0;
						bufDesc.Usage = D3D11_USAGE_DYNAMIC;
						ID3D11Buffer* buffer;
						hr = device->CreateBuffer(&bufDesc, nullptr, &buffer);
						if (FAILED(hr))
							return DEVICE_FAIL;
						constantBuffers[name] = buffer;
						
					}
					geometryShaders[id].constantBuffers.push_back(name);
					const Utilz::GUID cbNameGuid(sbd.Name);
					const Utilz::GUID combined = id + cbNameGuid;
					shaderAndResourceNameToBindSlot[combined] = sibd.BindPoint;
					break;
				}
			}
		}
	}

	reflection->Release();
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreatePixelShader(const Utilz::GUID& id, void* data, size_t size)
{
	const auto exists = pixelShaders.find(id);
	if (exists != pixelShaders.end())
		return EXISTS;

	ID3D11PixelShader* ps;
	HRESULT hr = device->CreatePixelShader(data, size, nullptr, &ps);
	if (FAILED(hr))
		return DEVICE_FAIL;

	pixelShaders[id] = { ps };

	ID3D11ShaderReflection* reflection;
	hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
	if (FAILED(hr))
		return DEVICE_FAIL;

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	for (int i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC sibd;
		reflection->GetResourceBindingDesc(i, &sibd);
		if (sibd.Type == D3D_SIT_TEXTURE)
		{
			const Utilz::GUID bindGuid(sibd.Name);
			const Utilz::GUID combinedGuid = id + bindGuid;
			shaderAndResourceNameToBindSlot[combinedGuid] = sibd.BindPoint;
		}
	}
	for (unsigned int i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC sibd;
		reflection->GetResourceBindingDesc(i, &sibd);
		if (sibd.Type == D3D_SIT_CBUFFER)
		{
			//Can't get the size from the RBD, can't get bindslot from the SBD...	
			//Find the sbd with the same name to get the size.
			for (unsigned int j = 0; j < shaderDesc.ConstantBuffers; ++j)
			{
				D3D11_SHADER_BUFFER_DESC sbd;
				ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(j);
				srcb->GetDesc(&sbd);
				Utilz::GUID name = std::string(sbd.Name);
				if (name == std::string(sibd.Name))
				{
					const auto cbExists = constantBuffers.find(name);
					if (cbExists == constantBuffers.end())
					{
						D3D11_BUFFER_DESC bufDesc;
						bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						bufDesc.StructureByteStride = 0;
						bufDesc.ByteWidth = sbd.Size;
						bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
						bufDesc.MiscFlags = 0;
						bufDesc.Usage = D3D11_USAGE_DYNAMIC;
						ID3D11Buffer* buffer;
						hr = device->CreateBuffer(&bufDesc, nullptr, &buffer);
						if (FAILED(hr))
							return DEVICE_FAIL;
						constantBuffers[name] = buffer;
						
					}
					pixelShaders[id].constantBuffers.push_back(name);
					const Utilz::GUID cbNameGuid(sbd.Name);
					const Utilz::GUID combined = id + cbNameGuid;
					shaderAndResourceNameToBindSlot[combined] = sibd.BindPoint;
					break;
				}
			}
		}
	}
	reflection->Release();
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateComputeShader(const Utilz::GUID& id, void* data, size_t size)
{
	const auto exists = computeShaders.find(id);
	if (exists != computeShaders.end())
		return EXISTS;

	ID3D11ComputeShader* cs;
	HRESULT hr = device->CreateComputeShader(data, size, nullptr, &cs);
	if (FAILED(hr))
		return DEVICE_FAIL;

	computeShaders[id] = { cs };


	ID3D11ShaderReflection* reflection;
	hr = D3DReflect(data, size, IID_ID3D11ShaderReflection, (void**)&reflection);
	if (FAILED(hr))
		return DEVICE_FAIL;

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	for (int i = 0; i < shaderDesc.BoundResources; ++i)
	{
		D3D11_SHADER_INPUT_BIND_DESC sibd;
		reflection->GetResourceBindingDesc(i, &sibd);
		if (sibd.Type == D3D_SIT_TEXTURE || sibd.Type == D3D_SIT_UAV_RWTYPED)
		{
			const Utilz::GUID bindGuid(sibd.Name);
			const Utilz::GUID combinedGuid = id + bindGuid;
			shaderAndResourceNameToBindSlot[combinedGuid] = sibd.BindPoint;
		}
		else if (sibd.Type == D3D_SIT_CBUFFER)
		{
			//Can't get the size from the RBD, can't get bindslot from the SBD...	
			//Find the sbd with the same name to get the size.
			for (unsigned int j = 0; j < shaderDesc.ConstantBuffers; ++j)
			{
				D3D11_SHADER_BUFFER_DESC sbd;
				ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(j);
				srcb->GetDesc(&sbd);
				Utilz::GUID name = std::string(sbd.Name);
				if (name == std::string(sibd.Name))
				{
					const auto cbExists = constantBuffers.find(name);
					if (cbExists == constantBuffers.end())
					{
						D3D11_BUFFER_DESC bufDesc;
						bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
						bufDesc.StructureByteStride = 0;
						bufDesc.ByteWidth = sbd.Size;
						bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
						bufDesc.MiscFlags = 0;
						bufDesc.Usage = D3D11_USAGE_DYNAMIC;
						ID3D11Buffer* buffer;
						hr = device->CreateBuffer(&bufDesc, nullptr, &buffer);
						if (FAILED(hr))
							return DEVICE_FAIL;
						constantBuffers[name] = buffer;

					}
					computeShaders[id].constantBuffers.push_back(name);
					const Utilz::GUID cbNameGuid(sbd.Name);
					const Utilz::GUID combined = id + cbNameGuid;
					shaderAndResourceNameToBindSlot[combined] = sibd.BindPoint;
					break;
				}
			}
		}
	}

	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyVertexShader(const Utilz::GUID& id)
{
	auto exists = vertexShaders.find(id);
	if (exists == vertexShaders.end())
		return NOT_FOUND;
	exists->second.shader->Release();
	vertexShaders.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyGeometryShader(const Utilz::GUID& id)
{
	auto exists = geometryShaders.find(id);
	if (exists == geometryShaders.end())
		return NOT_FOUND;
	exists->second.shader->Release();
	geometryShaders.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyPixelShader(const Utilz::GUID& id)
{
	auto exists = pixelShaders.find(id);
	if (exists == pixelShaders.end())
		return NOT_FOUND;
	exists->second.shader->Release();
	pixelShaders.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyComputeShader(const Utilz::GUID& id)
{
	auto exists = computeShaders.find(id);
	if (exists == computeShaders.end())
		return NOT_FOUND;
	exists->second.shader->Release();
	computeShaders.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateConstantBuffer(const Utilz::GUID& id, size_t size, void* initialData)
{
	const auto exists = constantBuffers.find(id);
	if (exists != constantBuffers.end())
		return EXISTS;

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = size;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DYNAMIC;


	HRESULT hr;
	ID3D11Buffer* buffer;
	if (initialData)
	{
		D3D11_SUBRESOURCE_DATA d;
		d.pSysMem = initialData;
		d.SysMemPitch = 0;
		d.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&bd, &d, &buffer);
	}
	else
	{
		hr = device->CreateBuffer(&bd, nullptr, &buffer);
	}
	
	if (FAILED(hr))
		return DEVICE_FAIL;

	constantBuffers[id] = buffer;
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::UpdateConstantBuffer(const Utilz::GUID& id, void* data, size_t size)
{
	const auto exists = constantBuffers.find(id);
	if (exists == constantBuffers.end())
		return NOT_FOUND;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HRESULT hr = deviceContext->Map(exists->second, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
		return DEVICE_FAIL;
	memcpy(mappedData.pData, data, size);
	deviceContext->Unmap(exists->second, 0);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::MapConstantBuffer(const Utilz::GUID & id, const std::function<void(void*mappedResource)>& mapCallback)
{
	const auto exists = constantBuffers.find(id);
	if (exists == constantBuffers.end())
		return NOT_FOUND;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HRESULT hr = deviceContext->Map(exists->second, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
		return DEVICE_FAIL;

	mapCallback(mappedData.pData);

	deviceContext->Unmap(exists->second, 0);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyConstantBuffer(const Utilz::GUID& id)
{
	auto exists = constantBuffers.end();
	if (exists == constantBuffers.end())
		return NOT_FOUND;

	exists->second->Release();
	constantBuffers.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateTexture(const Utilz::GUID& id, void* data, size_t width, size_t height)
{
	const auto exists = shaderResourceViews.find(id);
	if (exists != shaderResourceViews.end())
		return EXISTS;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
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
	d.pSysMem = data;
	d.SysMemPitch = width * 4;
	d.SysMemSlicePitch = 0;
	HRESULT hr = device->CreateTexture2D(&desc, &d, &texture);
	if (FAILED(hr))
		return DEVICE_FAIL;

	ID3D11ShaderResourceView* srv;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture, &srvDesc, &srv);
	if (FAILED(hr))
		return DEVICE_FAIL;

	texture->Release();
	shaderResourceViews[id] = srv;
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyTexture(const Utilz::GUID& id)
{
	auto exists = shaderResourceViews.find(id);
	if (exists == shaderResourceViews.end())
		return NOT_FOUND;

	exists->second->Release();
	shaderResourceViews.erase(id);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateRasterizerState(const Utilz::GUID& id, const RasterizerState& state)
{
	const auto exists = rasterizerStates.find(id);
	if (exists != rasterizerStates.end())
		return EXISTS;

	D3D11_RASTERIZER_DESC rd;
	rd.AntialiasedLineEnable = false;
	switch (state.cullMode)
	{
	case CullMode::CULL_BACK: rd.CullMode = D3D11_CULL_BACK; break;
	case CullMode::CULL_FRONT: rd.CullMode = D3D11_CULL_FRONT; break;
	case CullMode::CULL_NONE: rd.CullMode = D3D11_CULL_NONE; break;
	}
	switch (state.fillMode)
	{
	case FillMode::FILL_SOLID:		rd.FillMode = D3D11_FILL_SOLID; break;
	case FillMode::FILL_WIREFRAME:  rd.FillMode = D3D11_FILL_WIREFRAME; break;
	}
	switch (state.windingOrder)
	{
	case WindingOrder::CLOCKWISE:		 rd.FrontCounterClockwise = false; break;
	case WindingOrder::COUNTERCLOCKWISE: rd.FrontCounterClockwise = true; break;
	}
	rd.DepthBias = false;
	rd.DepthClipEnable = true;
	rd.DepthBiasClamp = 0;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;
	rd.SlopeScaledDepthBias = 0;

	ID3D11RasterizerState* rs;
	HRESULT hr = device->CreateRasterizerState(&rd, &rs);
	if (FAILED(hr))
		return DEVICE_FAIL;
	rasterizerStates[id] = rs;
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyRasterizerState(const Utilz::GUID& id)
{
	auto exists = rasterizerStates.find(id);
	if (exists == rasterizerStates.end())
		return NOT_FOUND;

	exists->second->Release();
	rasterizerStates.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateBlendState(const Utilz::GUID& id, const BlendState& state)
{
	const auto exists = blendStates.find(id);
	if (exists != blendStates.end())
		return EXISTS;

	D3D11_BLEND_DESC bd;
	bd.IndependentBlendEnable = false;

	D3D11_RENDER_TARGET_BLEND_DESC rtbd[8];
	rtbd[0].BlendEnable = state.enable;
	switch (state.blendOperation)
	{
	case BlendOperation::ADD: rtbd[0].BlendOp = D3D11_BLEND_OP_ADD; break;
	case BlendOperation::MAX: rtbd[0].BlendOp = D3D11_BLEND_OP_MAX; break;
	case BlendOperation::MIN: rtbd[0].BlendOp = D3D11_BLEND_OP_MIN; break;
	case BlendOperation::SUB: rtbd[0].BlendOp = D3D11_BLEND_OP_SUBTRACT; break;
	}

	switch (state.blendOperationAlpha)
	{
	case BlendOperation::ADD: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; break;
	case BlendOperation::MAX: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_MAX; break;
	case BlendOperation::MIN: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_MIN; break;
	case BlendOperation::SUB: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_SUBTRACT; break;
	}

	switch (state.dstBlend)
	{
	case Blend::BLEND_FACTOR:	rtbd[0].DestBlend = D3D11_BLEND_BLEND_FACTOR; break;
	case Blend::DEST_ALPHA:		rtbd[0].DestBlend = D3D11_BLEND_DEST_ALPHA; break;
	case Blend::DEST_COLOR:		rtbd[0].DestBlend = D3D11_BLEND_DEST_COLOR; break;
	case Blend::INV_DEST_ALPHA:	rtbd[0].DestBlend = D3D11_BLEND_INV_DEST_ALPHA; break;
	case Blend::INV_DEST_COLOR:	rtbd[0].DestBlend = D3D11_BLEND_INV_DEST_COLOR; break;
	case Blend::INV_SRC_ALPHA:	rtbd[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; break;
	case Blend::INV_SRC_COLOR:	rtbd[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR; break;
	case Blend::ONE:			rtbd[0].DestBlend = D3D11_BLEND_ONE; break;
	case Blend::SRC_ALPHA:		rtbd[0].DestBlend = D3D11_BLEND_SRC_ALPHA; break;
	case Blend::SRC_COLOR:		rtbd[0].DestBlend = D3D11_BLEND_SRC_COLOR; break;
	case Blend::ZERO:			rtbd[0].DestBlend = D3D11_BLEND_ZERO; break;
	}

	switch (state.srcBlend)
	{
	case Blend::BLEND_FACTOR:	rtbd[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR; break;
	case Blend::DEST_ALPHA:		rtbd[0].SrcBlend = D3D11_BLEND_DEST_ALPHA; break;
	case Blend::DEST_COLOR:		rtbd[0].SrcBlend = D3D11_BLEND_DEST_COLOR; break;
	case Blend::INV_DEST_ALPHA:	rtbd[0].SrcBlend = D3D11_BLEND_INV_DEST_ALPHA; break;
	case Blend::INV_DEST_COLOR:	rtbd[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR; break;
	case Blend::INV_SRC_ALPHA:	rtbd[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA; break;
	case Blend::INV_SRC_COLOR:	rtbd[0].SrcBlend = D3D11_BLEND_INV_SRC_COLOR; break;
	case Blend::ONE:			rtbd[0].SrcBlend = D3D11_BLEND_ONE; break;
	case Blend::SRC_ALPHA:		rtbd[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; break;
	case Blend::SRC_COLOR:		rtbd[0].SrcBlend = D3D11_BLEND_SRC_COLOR; break;
	case Blend::ZERO:			rtbd[0].SrcBlend = D3D11_BLEND_ZERO; break;
	}

	switch (state.dstBlendAlpha)
	{
	case Blend::BLEND_FACTOR:	rtbd[0].DestBlendAlpha = D3D11_BLEND_BLEND_FACTOR; break;
	case Blend::DEST_ALPHA:		rtbd[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA; break;
	case Blend::DEST_COLOR:		rtbd[0].DestBlendAlpha = D3D11_BLEND_DEST_COLOR; break;
	case Blend::INV_DEST_ALPHA:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA; break;
	case Blend::INV_DEST_COLOR:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_COLOR; break;
	case Blend::INV_SRC_ALPHA:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA; break;
	case Blend::INV_SRC_COLOR:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_COLOR; break;
	case Blend::ONE:			rtbd[0].DestBlendAlpha = D3D11_BLEND_ONE; break;
	case Blend::SRC_ALPHA:		rtbd[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA; break;
	case Blend::SRC_COLOR:		rtbd[0].DestBlendAlpha = D3D11_BLEND_SRC_COLOR; break;
	case Blend::ZERO:			rtbd[0].DestBlendAlpha = D3D11_BLEND_ZERO; break;
	}

	switch (state.srcBlendAlpha)
	{
	case Blend::BLEND_FACTOR:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_BLEND_FACTOR; break;
	case Blend::DEST_ALPHA:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA; break;
	case Blend::DEST_COLOR:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_DEST_COLOR; break;
	case Blend::INV_DEST_ALPHA:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA; break;
	case Blend::INV_DEST_COLOR:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_COLOR; break;
	case Blend::INV_SRC_ALPHA:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA; break;
	case Blend::INV_SRC_COLOR:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_COLOR; break;
	case Blend::ONE:			rtbd[0].SrcBlendAlpha = D3D11_BLEND_ONE; break;
	case Blend::SRC_ALPHA:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA; break;
	case Blend::SRC_COLOR:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_SRC_COLOR; break;
	case Blend::ZERO:			rtbd[0].SrcBlendAlpha = D3D11_BLEND_ZERO; break;
	}
	rtbd[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	bd.AlphaToCoverageEnable = false;
	bd.RenderTarget[0] = rtbd[0];

	ID3D11BlendState* blendState;
	HRESULT hr = device->CreateBlendState(&bd, &blendState);
	if (FAILED(hr))
		return DEVICE_FAIL;

	blendStates[id] = blendState;
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyBlendState(const Utilz::GUID& id)
{
	auto exists = blendStates.find(id);
	if (exists == blendStates.end())
		return NOT_FOUND;

	exists->second->Release();
	blendStates.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateDepthStencilState(const Utilz::GUID& id, const DepthStencilState& state)
{
	const auto exists = depthStencilStates.find(id);
	if (exists != depthStencilStates.end())
		return EXISTS;
	D3D11_DEPTH_STENCIL_DESC dsd;
	dsd.DepthEnable = state.enableDepth;
	switch (state.comparisonOperation)
	{
	case ComparisonOperation::EQUAL:			dsd.DepthFunc = D3D11_COMPARISON_EQUAL; break;
	case ComparisonOperation::GREATER:			dsd.DepthFunc = D3D11_COMPARISON_GREATER; break;
	case ComparisonOperation::GREATER_EQUAL:	dsd.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
	case ComparisonOperation::LESS:				dsd.DepthFunc = D3D11_COMPARISON_LESS; break;
	case ComparisonOperation::LESS_EQUAL:		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; break;
	case ComparisonOperation::NO_COMPARISON:	dsd.DepthFunc = D3D11_COMPARISON_NEVER; break;
	}

	dsd.DepthWriteMask = state.enableDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

	dsd.StencilEnable = true;
	dsd.StencilReadMask = 0xFF;
	dsd.StencilWriteMask = 0xFF;

	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	ID3D11DepthStencilState* dss;
	HRESULT hr = device->CreateDepthStencilState(&dsd, &dss);
	if (FAILED(hr))
		throw std::exception("Failed to create depth stencil state");

	depthStencilStates[id] = dss;
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyDepthStencilState(const Utilz::GUID& id)
{
	auto exists = depthStencilStates.find(id);
	if (exists == depthStencilStates.end())
		return NOT_FOUND;

	exists->second->Release();
	depthStencilStates.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateSamplerState(const Utilz::GUID& id, const SamplerState& state)
{
	const auto exists = samplerStates.find(id);
	if (exists != samplerStates.end())
		return EXISTS;
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));

	switch (state.addressU)
	{
	case AddressingMode::WRAP:		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; break;
	case AddressingMode::CLAMP:		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; break;
	case AddressingMode::MIRROR:	sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR; break;
	}
	switch (state.addressV)
	{
	case AddressingMode::WRAP:		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; break;
	case AddressingMode::CLAMP:		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP; break;
	case AddressingMode::MIRROR:	sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR; break;
	}
	switch (state.addressW)
	{
	case AddressingMode::WRAP:		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; break;
	case AddressingMode::CLAMP:		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP; break;
	case AddressingMode::MIRROR:	sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR; break;
	}
	switch (state.filter)
	{
	case Filter::ANISOTROPIC:	sd.Filter = D3D11_FILTER_ANISOTROPIC; break;
	case Filter::LINEAR:		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; break;
	case Filter::POINT:			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; break;
	}
	sd.BorderColor[0] = 0.0f; sd.BorderColor[1] = 0.0f; sd.BorderColor[2] = 0.0f; sd.BorderColor[3] = 0.0f;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	sd.MaxAnisotropy = state.maxAnisotropy;
	ID3D11SamplerState* samplerState;
	HRESULT hr = device->CreateSamplerState(&sd, &samplerState);
	if (FAILED(hr))
		return DEVICE_FAIL;

	samplerStates[id] = samplerState;
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroySamplerState(const Utilz::GUID& id)
{
	auto exists = samplerStates.find(id);
	if (exists == samplerStates.end())
		return NOT_FOUND;

	exists->second->Release();
	samplerStates.erase(exists);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateRenderTarget(const Utilz::GUID& id, const RenderTarget& target)
{
	const auto exists = shaderResourceViews.find(id);
	if (exists != shaderResourceViews.end())
		return EXISTS;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = target.width;
	desc.Height = target.height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	switch (target.format)
	{
	case TextureFormat::R32G32B32A32_FLOAT: desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	case TextureFormat::R8G8B8A8_UNORM:		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
	}

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	if (target.bindAsShaderResource) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	if (target.bindAsUnorderedAccess) desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	ID3D11Texture2D* texture;
	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(hr))
		return DEVICE_FAIL;

	if (target.bindAsShaderResource)
	{
		ID3D11ShaderResourceView* srv;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(texture, &srvDesc, &srv);
		if (FAILED(hr))
			return DEVICE_FAIL;
		shaderResourceViews[id] = srv;
	}


	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	rtvd.Format = desc.Format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	ID3D11RenderTargetView* rtv;
	hr = device->CreateRenderTargetView(texture, &rtvd, &rtv);
	if (FAILED(hr))
		return DEVICE_FAIL;

	renderTargetViews[id] = { rtv };
	renderTargetViews[id].clearColor[0] = target.clearColor[0];
	renderTargetViews[id].clearColor[1] = target.clearColor[1];
	renderTargetViews[id].clearColor[2] = target.clearColor[2];
	renderTargetViews[id].clearColor[3] = target.clearColor[3];

	texture->Release();
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyRenderTarget(const Utilz::GUID& id)
{
	auto rtv = renderTargetViews.find(id);
	if (rtv == renderTargetViews.end())
		return NOT_FOUND;
	rtv->second.rtv->Release();
	renderTargetViews.erase(rtv);

	auto srv = shaderResourceViews.find(id);
	if (srv == shaderResourceViews.end())
		return NOT_FOUND;
	srv->second->Release();
	shaderResourceViews.erase(srv);
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateDepthStencilView(const Utilz::GUID& id, size_t width, size_t height, bool bindAsTexture)
{
	const auto find = depthStencilViews.find(id);
	if (find != depthStencilViews.end())
	{
		if (bindAsTexture)
		{
			auto const findSRV = shaderResourceViews.find(id);
			if (findSRV == shaderResourceViews.end())
			{
				ID3D11Texture2D* texture;

				find->second->GetResource((ID3D11Resource**)&texture);

				D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
				srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvd.Texture2D.MostDetailedMip = 0;
				srvd.Texture2D.MipLevels = 1;
				ID3D11ShaderResourceView* srv;
				auto hr = device->CreateShaderResourceView(texture, &srvd, &srv);
				if (FAILED(hr))
					return DEVICE_FAIL;
				shaderResourceViews[id] = srv;
				texture->Release();
			}
		}
		return EXISTS;
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (bindAsTexture) desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	dsvd.Flags = 0;

	ID3D11Texture2D* texture;

	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(hr))
		return DEVICE_FAIL;

	ID3D11DepthStencilView* dsv;
	hr = device->CreateDepthStencilView(texture, &dsvd, &dsv);
	if (FAILED(hr))
		return DEVICE_FAIL;
	depthStencilViews[id] = dsv;

	if (bindAsTexture)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;
		ID3D11ShaderResourceView* srv;
		hr = device->CreateShaderResourceView(texture, &srvd, &srv);
		if (FAILED(hr))
			return DEVICE_FAIL;
		shaderResourceViews[id] = srv;
	}

	texture->Release();
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateDepthStencilViewCube(const Utilz::GUID& id, size_t width, size_t height,
	bool bindAsTexture)
{
	const auto exists = depthStencilViews.find(id);
	if(exists != depthStencilViews.end())
	{
		DestroyDepthStencilView(id);
		depthStencilViews.erase(exists);
	}

	D3D11_TEXTURE2D_DESC td;
	td.Width = width;
	td.Height = height;
	td.Format = DXGI_FORMAT_R32_TYPELESS;
	td.ArraySize = 6;
	td.MipLevels = 1;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	td.CPUAccessFlags = 0;

	ID3D11Texture2D* texture;
	HRESULT hr = device->CreateTexture2D(&td, nullptr, &texture);
	if (FAILED(hr))
		return DEVICE_FAIL;


	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvd.Texture2DArray.ArraySize = 6;
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	
	ID3D11DepthStencilView* dsv;
	hr = device->CreateDepthStencilView(texture, &dsvd, &dsv);
	if (FAILED(hr))
		return DEVICE_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.TextureCube.MipLevels = 1;
	srvd.TextureCube.MostDetailedMip = 0;

	ID3D11ShaderResourceView* srv;
	hr = device->CreateShaderResourceView(texture, &srvd, &srv);
	if (FAILED(hr))
		return DEVICE_FAIL;

	depthStencilViews[id] = dsv;
	shaderResourceViews[id] = srv;
	texture->Release();
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyDepthStencilView(const Utilz::GUID& id)
{
	auto dsv = depthStencilViews.find(id);
	if (dsv == depthStencilViews.end())
		return NOT_FOUND;
	dsv->second->Release();
	depthStencilViews.erase(dsv);

	auto srv = shaderResourceViews.find(id);
	if (srv != shaderResourceViews.end())
	{
		srv->second->Release();
		shaderResourceViews.erase(srv);
	}
	return SUCCESS;
}

int SE::Graphics::PipelineHandler::CreateUnorderedAccessView(const Utilz::GUID & id, const UnorderedAccessView & view)
{
	const auto findUAV = unorderedAccessViews.find(id);
	if (findUAV != unorderedAccessViews.end())
	{
		return EXISTS;
	}

	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));

	td.Width = view.width;
	td.Height = view.height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	switch (view.format)
	{
	case TextureFormat::R32G32B32A32_FLOAT: td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	case TextureFormat::R8G8B8A8_UNORM:		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
	}
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	ID3D11Texture2D* texture;
	auto hr = device->CreateTexture2D(&td, nullptr, &texture);
	if (FAILED(hr))
		return DEVICE_FAIL;

	

	if (view.bindAsShaderResource)
	{
		ID3D11ShaderResourceView* srv;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = td.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(texture, &srvDesc, &srv);
		if (FAILED(hr))
			return DEVICE_FAIL;
		shaderResourceViews[id] = srv;
	}


	D3D11_UNORDERED_ACCESS_VIEW_DESC description;
	ZeroMemory(&description, sizeof(description));
	description.Format = td.Format;
	description.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	description.Texture2D.MipSlice = 0;

	ID3D11UnorderedAccessView* unorderedAccessView;
	hr = device->CreateUnorderedAccessView(texture, &description, &unorderedAccessView);
	if (FAILED(hr))
		return DEVICE_FAIL;

	unorderedAccessViews[id] = { unorderedAccessView };
	unorderedAccessViews[id].clearColor[0] = view.clearColor[0];
	unorderedAccessViews[id].clearColor[1] = view.clearColor[1];
	unorderedAccessViews[id].clearColor[2] = view.clearColor[2];
	unorderedAccessViews[id].clearColor[3] = view.clearColor[3];

	texture->Release();

	return SUCCESS;
}

int SE::Graphics::PipelineHandler::DestroyUnorderedAccessView(const Utilz::GUID & id)
{
	auto find = unorderedAccessViews.find(id);
	if (find == unorderedAccessViews.end())
		return NOT_FOUND;
	find->second.uav->Release();
	unorderedAccessViews.erase(find);

	auto srv = shaderResourceViews.find(id);
	if (srv != shaderResourceViews.end())
	{
		srv->second->Release();
		shaderResourceViews.erase(srv);
	}
	return SUCCESS;
}

void SE::Graphics::PipelineHandler::SetPipeline(const Pipeline& pipeline)
{
	StartProfile;
	uint32_t offset = 0;
	SetInputAssemblerStage(pipeline.IAStage);
	SetVertexShaderStage(pipeline.VSStage);
	SetGeometryShaderStage(pipeline.GSStage);
	if (pipeline.SOStage.streamOutTarget != currentPipeline.SOStage.streamOutTarget)
	{
		deviceContext->SOSetTargets(1, &vertexBuffers[pipeline.SOStage.streamOutTarget].buffer, &offset);
		currentPipeline.SOStage.streamOutTarget = pipeline.SOStage.streamOutTarget;
	}
	SetRasterizerStage(pipeline.RStage);
	//ForcedSetOutputMergerStage(pipeline.OMStage);
	SetPixelShaderStage(pipeline.PSStage);
	SetOutputMergerStage(pipeline.OMStage);
	//ForcedSetOutputMergerStage(pipeline.OMStage);
	SetComputeShaderStage(pipeline.CSStage);
	StopProfile;
}

void SE::Graphics::PipelineHandler::SetPipelineForced(const Pipeline& pipeline)
{
	StartProfile;



	uint32_t offset = 0;
	ForcedSetInputAssemblerStage(pipeline.IAStage);
	ForcedSetVertexShaderStage(pipeline.VSStage);
	ForcedSetGeometryShaderStage(pipeline.GSStage);
	if (pipeline.SOStage.streamOutTarget != Utilz::GUID())
		deviceContext->SOSetTargets(1, &vertexBuffers[pipeline.SOStage.streamOutTarget].buffer, &offset);
	else
	{
		deviceContext->SOSetTargets(0, nullptr, nullptr);
	}
	currentPipeline.SOStage.streamOutTarget = pipeline.SOStage.streamOutTarget;
	ForcedSetRasterizerStage(pipeline.RStage);
	ForcedSetPixelShaderStage(pipeline.PSStage);
	ForcedSetOutputMergerStage(pipeline.OMStage);
	ForcedSetComputeShaderStage(pipeline.CSStage);
	StopProfile;
}

void SE::Graphics::PipelineHandler::ClearAllRenderTargets()
{
	for(auto& rtv : renderTargetViews)
	{
		if(rtv.second.rtv)
		{
			deviceContext->ClearRenderTargetView(rtv.second.rtv, rtv.second.clearColor);
		}
	}
	for (auto& uav : unorderedAccessViews)
	{
		if (uav.second.uav)
		{
			//deviceContext->ClearUnorderedAccessViewFloat(uav.second.uav, uav.second.clearColor);
		}
	}
	for (auto& dsv : depthStencilViews) {

		if (dsv.second)
		{
			deviceContext->ClearDepthStencilView(dsv.second, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}
}

void SE::Graphics::PipelineHandler::SetInputAssemblerStage(const InputAssemblerStage& pIA)
{
	StartProfile;
	auto& cIA = currentPipeline.IAStage;
	if (pIA.topology != cIA.topology)
	{
		switch (pIA.topology)
		{
		case PrimitiveTopology::LINE_LIST:		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST); break;
		case PrimitiveTopology::LINE_STRIP:		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP); break;
		case PrimitiveTopology::POINT_LIST:		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); break;
		case PrimitiveTopology::TRIANGLE_LIST:	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); break;
		case PrimitiveTopology::TRIANGLE_STRIP:	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); break;
		}
		cIA.topology = pIA.topology;
	}
	if (pIA.indexBuffer != cIA.indexBuffer)
	{
		auto ib = indexBuffers.find(pIA.indexBuffer);
		if (ib != indexBuffers.end())
		{
			DXGI_FORMAT f = ib->second.stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			deviceContext->IASetIndexBuffer(ib->second.buffer, f, 0);
			cIA.indexBuffer = pIA.indexBuffer;
		}
		
	}
	if (pIA.vertexBuffer != cIA.vertexBuffer)
	{
		auto vb = vertexBuffers.find(pIA.vertexBuffer);
		if (vb != vertexBuffers.end())
		{
			uint32_t offset = 0;
			deviceContext->IASetVertexBuffers(0, 1, &vb->second.buffer, &vb->second.stride, &offset);
			cIA.vertexBuffer = pIA.vertexBuffer;
		}		
	}
	if (pIA.inputLayout != cIA.inputLayout)
	{
		auto il = inputLayouts.find(pIA.inputLayout);
		if (il != inputLayouts.end())
		{
			deviceContext->IASetInputLayout(il->second);
			cIA.inputLayout = pIA.inputLayout;
		}
	}

	StopProfile;
}

#undef min
#include <algorithm>

void SE::Graphics::PipelineHandler::SetVertexShaderStage(const ShaderStage& vss)
{
	StartProfile;
	auto& c = currentPipeline.VSStage;

	if (vss.shader != c.shader)
	{

		_ASSERT_EXPR(vertexShaders.find(vss.shader) != vertexShaders.end(), "Create vertex shader has not been called for vertex shader in pipeline.");
		const auto& shader = vertexShaders[vss.shader];
		deviceContext->VSSetShader(shader.shader, nullptr, 0); // Yes, bind it
		c.shader = vss.shader;

		for (auto& cbg : shader.constantBuffers) // Any constant buffers
		{
			auto& cb = constantBuffers[cbg];
			auto& binding = shaderAndResourceNameToBindSlot[vss.shader + cbg];
			deviceContext->VSSetConstantBuffers(binding, 1, &cb);
		}

	}
/*
	for (int i = 0; i < vss.textureCount; ++i)
	{
		if (vss.textures[i] != c.textures[i] || vss.textureBindings[i] != c.textureBindings[i])
		{
			auto srv = shaderResourceViews.find(vss.textures[i]);
			if (srv != shaderResourceViews.end())
			{
				const auto bindSlotID = vss.shader + vss.textureBindings[i];
				const auto bind = shaderAndResourceNameToBindSlot.find(bindSlotID);
				if (bind != shaderAndResourceNameToBindSlot.end())
				{
					deviceContext->VSSetShaderResources(bind->second, 1, &srv->second);
				}
			}
			c.textures[i] = vss.textures[i];
			c.textureBindings[i] = vss.textureBindings[i];
		}
	}
	c.textureCount = vss.textureCount;
	ID3D11SamplerState* samplers[ShaderStage::maxSamplers] = { nullptr };
	for (int i = 0; i < vss.samplerCount; ++i)
	{
		if (vss.samplers[i] != c.samplers[i])
		{
			const auto samp = samplerStates.find(vss.samplers[i]);
			if (samp != samplerStates.end())
				samplers[i] = samp->second;
			c.samplers[i] = vss.samplers[i];
		}
	}
	c.samplerCount = vss.samplerCount;
	if (vss.samplerCount)
		deviceContext->VSSetSamplers(0, vss.samplerCount, samplers);*/

	StopProfile;
}

void SE::Graphics::PipelineHandler::SetGeometryShaderStage(const ShaderStage& gss)
{
	StartProfile;
	auto& c = currentPipeline.GSStage;

	if (gss.shader != c.shader)
	{
		_ASSERT_EXPR(geometryShaders.find(gss.shader) != geometryShaders.end(), "Create geometry shader has not been called for geometry shader in pipeline.");
		const auto& shader = geometryShaders[gss.shader];
		deviceContext->GSSetShader(shader.shader, nullptr, 0);
		c.shader = gss.shader;
		for (auto& cbg : shader.constantBuffers) // Any constant buffers
		{
			auto& cb = constantBuffers[cbg];
			auto& binding = shaderAndResourceNameToBindSlot[gss.shader + cbg];
			deviceContext->GSSetConstantBuffers(binding, 1, &cb);
		}
	}
	/*for (int i = 0; i < gss.constantBufferCount; ++i)
	{
		if (gss.constantBuffers[i] != c.constantBuffers[i])
		{
			auto cb = constantBuffers.find(gss.constantBuffers[i]);
			if (cb != constantBuffers.end())
			{
				const auto cbid = gss.shader + gss.constantBuffers[i];
				auto bind = shaderAndResourceNameToBindSlot.find(cbid);
				if (bind != shaderAndResourceNameToBindSlot.end())
				{
					deviceContext->GSSetConstantBuffers(bind->second, 1, &cb->second);
				}
				else
				{
					bind = shaderAndResourceNameToBindSlot.find(gss.constantBuffers[i]);
					if (bind != shaderAndResourceNameToBindSlot.end())
						deviceContext->GSSetConstantBuffers(bind->second, 1, &cb->second);
				}
				c.constantBuffers[i] = gss.constantBuffers[i];
			}
		}
	}
	c.constantBufferCount = gss.constantBufferCount;*/
	//for (int i = 0; i < gss.textureCount; ++i)
	//{
	//	if (gss.textures[i] != c.textures[i] || gss.textureBindings[i] != c.textureBindings[i])
	//	{
	//		auto srv = shaderResourceViews.find(gss.textures[i]);
	//		if (srv != shaderResourceViews.end())
	//		{
	//			const auto bindSlotID = gss.shader + gss.textureBindings[i];
	//			const auto bind = shaderAndResourceNameToBindSlot.find(bindSlotID);
	//			if (bind != shaderAndResourceNameToBindSlot.end())
	//			{
	//				deviceContext->GSSetShaderResources(bind->second, 1, &srv->second);
	//			}
	//		}
	//		c.textures[i] = gss.textures[i];
	//		c.textureBindings[i] = gss.textureBindings[i];
	//	}
	//}
	//c.textureCount = gss.textureCount;
	//ID3D11SamplerState* samplers[ShaderStage::maxSamplers] = { nullptr };
	//for (int i = 0; i < gss.samplerCount; ++i)
	//{
	//	if (gss.samplers[i] != c.samplers[i])
	//	{
	//		const auto samp = samplerStates.find(gss.samplers[i]);
	//		if (samp != samplerStates.end())
	//			samplers[i] = samp->second;
	//		c.samplers[i] = gss.samplers[i];
	//	}
	//}
	//c.samplerCount = gss.samplerCount;
	//if (gss.samplerCount)
	//	deviceContext->GSSetSamplers(0, gss.samplerCount, samplers);
	StopProfile;
}

void SE::Graphics::PipelineHandler::SetRasterizerStage(const RasterizerStage& rs)
{
	StartProfile;
	auto& c = currentPipeline.RStage;
	if (rs.rasterizerState != c.rasterizerState)
	{
		const auto rast = rasterizerStates.find(rs.rasterizerState);
		if (rast != rasterizerStates.end())
		{
			deviceContext->RSSetState(rast->second);
			c.rasterizerState = rs.rasterizerState;
		}
	}
	if (rs.viewport != c.viewport)
	{
		const auto vp = viewports.find(rs.viewport);
		if (vp != viewports.end())
		{
			deviceContext->RSSetViewports(1, &vp->second);
			c.viewport = rs.viewport;
		}
	}
	StopProfile;
}

void SE::Graphics::PipelineHandler::SetPixelShaderStage(const ShaderStage& pss)
{
	StartProfile;
	auto& c = currentPipeline.PSStage;

	if (pss.shader != c.shader)
	{
		_ASSERT_EXPR(pixelShaders.find(pss.shader) != pixelShaders.end(), "Create pixel shader has not been called for pixel shader in pipeline.");
		const auto& shader = pixelShaders[pss.shader];
		deviceContext->PSSetShader(shader.shader, nullptr, 0);
		c.shader = pss.shader;

		for (auto& cbg : shader.constantBuffers) // Any constant buffers
		{
			auto& cb = constantBuffers[cbg];
			auto& binding = shaderAndResourceNameToBindSlot[pss.shader + cbg];
			deviceContext->PSSetConstantBuffers(binding, 1, &cb);
		}
	}
	
	for (int i = 0; i < pss.textureCount; ++i)
	{
		//if (pss.textures[i] != c.textures[i] || pss.textureBindings[i] != c.textureBindings[i])
		{
			auto srv = shaderResourceViews.find(pss.textures[i]);
			if (srv != shaderResourceViews.end())
			{
				const auto bindSlotID = pss.shader + pss.textureBindings[i];
				const auto bind = shaderAndResourceNameToBindSlot.find(bindSlotID);
				if (bind != shaderAndResourceNameToBindSlot.end())
				{
					deviceContext->PSSetShaderResources(bind->second, 1, &srv->second);
				}
			}
			c.textures[i] = pss.textures[i];
			c.textureBindings[i] = pss.textureBindings[i];
		}
	}
	c.textureCount = pss.textureCount;
	ID3D11SamplerState* samplers[ShaderStage::maxSamplers] = { nullptr };
	bool samplerChanged = false;
	for (int i = 0; i < pss.samplerCount; ++i)
	{
		if (pss.samplers[i] != c.samplers[i])
		{
			const auto samp = samplerStates.find(pss.samplers[i]);
			if (samp != samplerStates.end())
				samplers[i] = samp->second;
			c.samplers[i] = pss.samplers[i];
			samplerChanged = true;
		}
	}
	c.samplerCount = pss.samplerCount;
	if (samplerChanged)
		deviceContext->PSSetSamplers(0, pss.samplerCount, samplers);
	StopProfile;
}

void SE::Graphics::PipelineHandler::SetOutputMergerStage(const OutputMergerStage& oms)
{
	StartProfile;
	auto& c = currentPipeline.OMStage;
	ID3D11RenderTargetView* renderTargets[OutputMergerStage::maxRenderTargets] = { nullptr };
	bool changed = false;
	for (int i = 0; i < oms.renderTargetCount; ++i)
	{
		if (oms.renderTargets[i] != c.renderTargets[i])
		{
			changed = true;
		}
		const auto rtv = renderTargetViews.find(oms.renderTargets[i]);
		if (rtv != renderTargetViews.end())
			renderTargets[i] = rtv->second.rtv;
		else
			renderTargets[i] = nullptr;
		c.renderTargets[i] = oms.renderTargets[i];
	}
	for (int i = oms.renderTargetCount; i < Graphics::OutputMergerStage::maxRenderTargets; i++)
		c.renderTargets[i] = Utilz::GUID();

	c.renderTargetCount = oms.renderTargetCount;
	ID3D11DepthStencilView* depthview = nullptr;
	if (oms.depthStencilView != c.depthStencilView)
	{
		changed = true;
	}
	const auto dsv = depthStencilViews.find(oms.depthStencilView);
	if (dsv != depthStencilViews.end())
		depthview = dsv->second;
	c.depthStencilView = oms.depthStencilView;
	

	if (changed)
		deviceContext->OMSetRenderTargets(oms.renderTargetCount, renderTargets, depthview);

	if (oms.blendState != c.blendState)
	{
		const auto bs = blendStates.find(oms.blendState);
		if (bs != blendStates.end())
		{
			float blend[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			deviceContext->OMSetBlendState(bs->second, blend, 0xFF);
			c.blendState = oms.blendState;
		}
	}

	if (oms.depthStencilState != c.depthStencilState)
	{
		const auto dss = depthStencilStates.find(oms.depthStencilState);
		if (dss != depthStencilStates.end())
		{
			deviceContext->OMSetDepthStencilState(dss->second, 0);
		}
	}
	StopProfile;
}

void SE::Graphics::PipelineHandler::SetComputeShaderStage(const ShaderStage & css)
{
	auto& c = currentPipeline.CSStage;

	if (css.shader != c.shader)
	{
		_ASSERT_EXPR(computeShaders.find(css.shader) != computeShaders.end(), "Create Compute shader has not been called for compute shader in pipeline.");
		const auto& shader = computeShaders[css.shader];
		deviceContext->CSSetShader(shader.shader, nullptr, 0);
		c.shader = css.shader;

		for (auto& cbg : shader.constantBuffers) // Any constant buffers
		{
			auto& cb = constantBuffers[cbg];
			auto& binding = shaderAndResourceNameToBindSlot[css.shader + cbg];
			deviceContext->CSSetConstantBuffers(binding, 1, &cb);
		}
	}

	

	ID3D11UnorderedAccessView* uavs[css.maxUAVs] = { nullptr };
	bool changed = false;
	for (uint8_t j = 0; j < css.uavCount; j++)
	{		
		auto i = css.maxUAVs - j - 1;
		if (changed || css.uavs[i] != c.uavs[i])
		{
			changed = true;
			auto const find = unorderedAccessViews.find(css.uavs[i]);
			if(find != unorderedAccessViews.end())
				uavs[i] = find->second.uav;
			c.uavs[i] = css.uavs[i];
		}
			
	}
	c.uavCount = css.uavCount;
	if(changed)
		deviceContext->CSSetUnorderedAccessViews(0, css.maxUAVs, uavs, NULL);

	for (int i = 0; i < css.textureCount; ++i)
	{
		if (css.textures[i] != c.textures[i] || css.textureBindings[i] != c.textureBindings[i])
		{
			auto srv = shaderResourceViews.find(css.textures[i]);
			if (srv != shaderResourceViews.end())
			{
				const auto bindSlotID = css.shader + css.textureBindings[i];
				const auto bind = shaderAndResourceNameToBindSlot.find(bindSlotID);
				if (bind != shaderAndResourceNameToBindSlot.end())
				{
					deviceContext->PSSetShaderResources(bind->second, 1, &srv->second);
				}
			}
			c.textures[i] = css.textures[i];
			c.textureBindings[i] = css.textureBindings[i];
		}
	}
	c.textureCount = css.textureCount;
}

void SE::Graphics::PipelineHandler::ForcedSetInputAssemblerStage(const InputAssemblerStage& pIA)
{
	StartProfile;
	auto& cIA = currentPipeline.IAStage;
	switch (pIA.topology)
	{
	case PrimitiveTopology::LINE_LIST:		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST); break;
	case PrimitiveTopology::LINE_STRIP:		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP); break;
	case PrimitiveTopology::POINT_LIST:		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); break;
	case PrimitiveTopology::TRIANGLE_LIST:	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); break;
	case PrimitiveTopology::TRIANGLE_STRIP:	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); break;
	}
	cIA.topology = pIA.topology;


	auto ib = indexBuffers.find(pIA.indexBuffer);
	if (ib != indexBuffers.end())
	{
		DXGI_FORMAT f = ib->second.stride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		deviceContext->IASetIndexBuffer(ib->second.buffer, f, 0);
		cIA.indexBuffer = pIA.indexBuffer;
	}

	auto vb = vertexBuffers.find(pIA.vertexBuffer);
	if (vb != vertexBuffers.end())
	{
		uint32_t offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &vb->second.buffer, &vb->second.stride, &offset);
		cIA.vertexBuffer = pIA.vertexBuffer;
	}

	auto il = inputLayouts.find(pIA.inputLayout);
	if (il != inputLayouts.end())
	{
		deviceContext->IASetInputLayout(il->second);
		cIA.inputLayout = pIA.inputLayout;
	}
	StopProfile;
}

void SE::Graphics::PipelineHandler::ForcedSetVertexShaderStage(const ShaderStage& vss)
{
	StartProfile;
	auto& c = currentPipeline.VSStage;

	_ASSERT_EXPR(vertexShaders.find(vss.shader) != vertexShaders.end(), "Create vertex shader has not been called for vertex shader in pipeline.");
	const auto& shader = vertexShaders[vss.shader];
	deviceContext->VSSetShader(shader.shader, nullptr, 0); // Yes, bind it
	c.shader = vss.shader;

	for (auto& cbg : shader.constantBuffers) // Any constant buffers
	{
		auto& cb = constantBuffers[cbg];
		auto& binding = shaderAndResourceNameToBindSlot[vss.shader + cbg];
		deviceContext->VSSetConstantBuffers(binding, 1, &cb);
	}
	StopProfile;
}

void SE::Graphics::PipelineHandler::ForcedSetGeometryShaderStage(const ShaderStage& gss)
{
	StartProfile;
	auto& c = currentPipeline.GSStage;

	_ASSERT_EXPR(geometryShaders.find(gss.shader) != geometryShaders.end(), "Create geometry shader has not been called for geometry shader in pipeline.");
	const auto& shader = geometryShaders[gss.shader];
	deviceContext->GSSetShader(shader.shader, nullptr, 0);
	c.shader = gss.shader;
	for (auto& cbg : shader.constantBuffers) // Any constant buffers
	{
		auto& cb = constantBuffers[cbg];
		auto& binding = shaderAndResourceNameToBindSlot[gss.shader + cbg];
		deviceContext->GSSetConstantBuffers(binding, 1, &cb);
	}
	StopProfile;
}

void SE::Graphics::PipelineHandler::ForcedSetRasterizerStage(const RasterizerStage& rs)
{
	StartProfile;
	auto& c = currentPipeline.RStage;
	const auto rast = rasterizerStates.find(rs.rasterizerState);
	if (rast != rasterizerStates.end())
	{
		deviceContext->RSSetState(rast->second);
		c.rasterizerState = rs.rasterizerState;
	}

	const auto vp = viewports.find(rs.viewport);
	if (vp != viewports.end())
	{
		deviceContext->RSSetViewports(1, &vp->second);
		c.viewport = rs.viewport;
	}
	StopProfile;
}

void SE::Graphics::PipelineHandler::ForcedSetPixelShaderStage(const ShaderStage& pss)
{
	StartProfile;
	auto& c = currentPipeline.PSStage;


	_ASSERT_EXPR(pixelShaders.find(pss.shader) != pixelShaders.end(), "Create pixel shader has not been called for pixel shader in pipeline.");
	const auto& shader = pixelShaders[pss.shader];
	deviceContext->PSSetShader(shader.shader, nullptr, 0);
	c.shader = pss.shader;

	for (auto& cbg : shader.constantBuffers) // Any constant buffers
	{
		auto& cb = constantBuffers[cbg];
		auto& binding = shaderAndResourceNameToBindSlot[pss.shader + cbg];
		deviceContext->PSSetConstantBuffers(binding, 1, &cb);
	}

	for (int i = 0; i < pss.textureCount; ++i)
	{
		auto srv = shaderResourceViews.find(pss.textures[i]);
		if (srv != shaderResourceViews.end())
		{
			const auto bindSlotID = pss.shader + pss.textureBindings[i];
			const auto bind = shaderAndResourceNameToBindSlot.find(bindSlotID);
			if (bind != shaderAndResourceNameToBindSlot.end())
			{
				deviceContext->PSSetShaderResources(bind->second, 1, &srv->second);
			}
		}
		c.textures[i] = pss.textures[i];
		c.textureBindings[i] = pss.textureBindings[i];
	}
	c.textureCount = pss.textureCount;
	ID3D11SamplerState* samplers[ShaderStage::maxSamplers] = { nullptr };
	
	for (int i = 0; i < pss.samplerCount; ++i)
	{
		const auto samp = samplerStates.find(pss.samplers[i]);
		if (samp != samplerStates.end())
			samplers[i] = samp->second;
		c.samplers[i] = pss.samplers[i];
	}
	c.samplerCount = pss.samplerCount;
	if (pss.samplerCount)
		deviceContext->PSSetSamplers(0, pss.samplerCount, samplers);
	StopProfile;
}

void SE::Graphics::PipelineHandler::ForcedSetOutputMergerStage(const OutputMergerStage& oms)
{
	StartProfile;
	auto& c = currentPipeline.OMStage;
	ID3D11RenderTargetView* renderTargets[OutputMergerStage::maxRenderTargets] = { nullptr };
	for (int i = 0; i < oms.renderTargetCount; ++i)
	{
		const auto rtv = renderTargetViews.find(oms.renderTargets[i]);
		if (rtv != renderTargetViews.end())
			renderTargets[i] = rtv->second.rtv;
		else
			renderTargets[i] = nullptr;
		c.renderTargets[i] = oms.renderTargets[i];
	}
	c.renderTargetCount = oms.renderTargetCount;
	for (int i = oms.renderTargetCount; i < Graphics::OutputMergerStage::maxRenderTargets; i++)
		c.renderTargets[i] = Utilz::GUID();

	ID3D11DepthStencilView* depthview = nullptr;

	const auto dsv = depthStencilViews.find(oms.depthStencilView);
	if (dsv != depthStencilViews.end())
		depthview = dsv->second;
	c.depthStencilView = oms.depthStencilView;



	deviceContext->OMSetRenderTargets(oms.renderTargetCount, renderTargets, depthview);


	const auto bs = blendStates.find(oms.blendState);
	if (bs != blendStates.end())
	{
		float blend[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		deviceContext->OMSetBlendState(bs->second, blend, 0xFF);
		c.blendState = oms.blendState;
	}


	const auto dss = depthStencilStates.find(oms.depthStencilState);
	if (dss != depthStencilStates.end())
	{
		deviceContext->OMSetDepthStencilState(dss->second, 0);
		c.depthStencilState = oms.depthStencilState;
	}
	StopProfile;
}

void SE::Graphics::PipelineHandler::ForcedSetComputeShaderStage(const ShaderStage & css)
{
	auto& c = currentPipeline.CSStage;

	_ASSERT_EXPR(computeShaders.find(css.shader) != computeShaders.end(), "Create Compute shader has not been called for compute shader in pipeline.");
	const auto& shader = computeShaders[css.shader];
	deviceContext->CSSetShader(shader.shader, nullptr, 0);
	c.shader = css.shader;
	
	for (auto& cbg : shader.constantBuffers) // Any constant buffers
	{
		auto& cb = constantBuffers[cbg];
		auto& binding = shaderAndResourceNameToBindSlot[css.shader + cbg];
		deviceContext->CSSetConstantBuffers(binding, 1, &cb);
	}
	
	ID3D11UnorderedAccessView* uavs[css.maxUAVs] = { nullptr };
	for (uint8_t i = 0; i < css.uavCount; i++)
	{
		auto const find = unorderedAccessViews.find(css.uavs[i]);
		if (find != unorderedAccessViews.end())
			uavs[i] = find->second.uav;
		c.uavs[i] = css.uavs[i];		
	}
	c.uavCount = css.uavCount;
	deviceContext->CSSetUnorderedAccessViews(0, css.maxUAVs, uavs, NULL);

	for (int i = 0; i < css.textureCount; ++i)
	{
		auto srv = shaderResourceViews.find(css.textures[i]);
		if (srv != shaderResourceViews.end())
		{
			const auto bindSlotID = css.shader + css.textureBindings[i];
			const auto bind = shaderAndResourceNameToBindSlot.find(bindSlotID);
			if (bind != shaderAndResourceNameToBindSlot.end())
			{
				deviceContext->CSSetShaderResources(bind->second, 1, &srv->second);
			}
		}
		c.textures[i] = css.textures[i];
		c.textureBindings[i] = css.textureBindings[i];
	}
	c.textureCount = css.textureCount;
}
