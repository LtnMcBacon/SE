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
		r.second->Release();
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
			//Can't get the size from the RBD, can't get bindslot from the SBD...	
			//Find the sbd with the same name to get the size.
			for (unsigned int i = 0; i < shaderDesc.ConstantBuffers; ++i)
			{
				D3D11_SHADER_BUFFER_DESC sbd;
				ID3D11ShaderReflectionConstantBuffer* srcb = reflection->GetConstantBufferByIndex(i);
				srcb->GetDesc(&sbd);
				if (sbd.Name == sibd.Name)
				{
					const auto cbExists = constantBuffers.find(sbd.Name);
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
							throw std::exception("Failed to create constant buffer.");
						constantBuffers[sbd.Name] = buffer;
					}

					const Utilz::GUID cbNameGuid(sbd.Name);
					const Utilz::GUID combined = id + cbNameGuid;
					shaderAndResourceNameToBindSlot[combined] = sibd.BindPoint;
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
	const auto exists = pixelShaders.find(id);
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
			shaderAndResourceNameToBindSlot[combinedGuid] = sibd.BindPoint;
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

void SE::Graphics::PipelineHandler::CreateConstantBuffer(const Utilz::GUID& id, size_t size, void* initialData)
{
	const auto exists = constantBuffers.find(id);
	if (exists != constantBuffers.end())
		return;

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = size;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA d;
	d.pSysMem = initialData;
	d.SysMemPitch = 0;
	d.SysMemSlicePitch = 0;

	ID3D11Buffer* buffer;
	HRESULT hr = device->CreateBuffer(&bd, &d, &buffer);
	if (FAILED(hr))
		throw std::exception("Failed to create constant buffer");

	constantBuffers[id] = buffer;
}

void SE::Graphics::PipelineHandler::UpdateConstantBuffer(const Utilz::GUID& id, void* data, size_t size)
{
	const auto exists = constantBuffers.find(id);
	if (exists == constantBuffers.end())
		return;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HRESULT hr = deviceContext->Map(exists->second, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (FAILED(hr))
		throw std::exception("Failed to map constant buffer");
	memcpy(mappedData.pData, data, size);
	deviceContext->Unmap(exists->second, 0);

}

void SE::Graphics::PipelineHandler::DestroyConstantBuffer(const Utilz::GUID& id)
{
	auto exists = constantBuffers.end();
	if (exists == constantBuffers.end())
		return;

	exists->second->Release();
	constantBuffers.erase(exists);
}

void SE::Graphics::PipelineHandler::CreateTexture(const Utilz::GUID& id, void* data, size_t width, size_t height)
{
	const auto exists = shaderResourceViews.find(id);
	if (exists != shaderResourceViews.end())
		return;

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
		throw std::exception("Failed to create texture.");

	ID3D11ShaderResourceView* srv;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture, &srvDesc, &srv);
	if (FAILED(hr))
		throw std::exception("Failed to create shader resource view");

	texture->Release();
	shaderResourceViews[id] = srv;
}

void SE::Graphics::PipelineHandler::DestroyTexture(const Utilz::GUID& id)
{
	auto exists = shaderResourceViews.find(id);
	if (exists == shaderResourceViews.end())
		return;

	exists->second->Release();
	shaderResourceViews.erase(id);
}

void SE::Graphics::PipelineHandler::CreateRasterizerState(const Utilz::GUID& id, const RasterizerState& state)
{
	const auto exists = rasterizerStates.find(id);
	if (exists != rasterizerStates.end())
		return;

	D3D11_RASTERIZER_DESC rd;
	rd.AntialiasedLineEnable = false;
	switch(state.cullMode)
	{
	case CullMode::CULL_BACK: rd.CullMode	= D3D11_CULL_BACK;
	case CullMode::CULL_FRONT: rd.CullMode	= D3D11_CULL_FRONT;
	case CullMode::CULL_NONE: rd.CullMode	= D3D11_CULL_NONE;
	}
	switch(state.fillMode)
	{
	case FillMode::FILL_SOLID:		rd.FillMode = D3D11_FILL_SOLID;
	case FillMode::FILL_WIREFRAME:  rd.FillMode = D3D11_FILL_WIREFRAME;
	}
	switch(state.windingOrder)
	{
	case WindingOrder::CLOCKWISE:		 rd.FrontCounterClockwise = false;
	case WindingOrder::COUNTERCLOCKWISE: rd.FrontCounterClockwise = true;
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
		throw std::exception("Failed to create rasterizer state");
	rasterizerStates[id] = rs;

}

void SE::Graphics::PipelineHandler::DestroyRasterizerState(const Utilz::GUID& id)
{
	auto exists = rasterizerStates.find(id);
	if (exists == rasterizerStates.end())
		return;

	exists->second->Release();
	rasterizerStates.erase(exists);
}

void SE::Graphics::PipelineHandler::CreateBlendState(const Utilz::GUID& id, const BlendState& state)
{
	const auto exists = blendStates.find(id);
	if (exists != blendStates.end())
		return;

	D3D11_BLEND_DESC bd;
	bd.IndependentBlendEnable = false;

	D3D11_RENDER_TARGET_BLEND_DESC rtbd[8];
	rtbd[0].BlendEnable = state.enable;
	switch (state.blendOperation)
	{
	case BlendOperation::ADD: rtbd[0].BlendOp = D3D11_BLEND_OP_ADD;
	case BlendOperation::MAX: rtbd[0].BlendOp = D3D11_BLEND_OP_MAX;
	case BlendOperation::MIN: rtbd[0].BlendOp = D3D11_BLEND_OP_MIN;
	case BlendOperation::SUB: rtbd[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
	}
	
	switch(state.blendOperationAlpha)
	{
	case BlendOperation::ADD: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	case BlendOperation::MAX: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	case BlendOperation::MIN: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
	case BlendOperation::SUB: rtbd[0].BlendOpAlpha = D3D11_BLEND_OP_SUBTRACT;
	}

	switch(state.dstBlend)
	{
	case Blend::BLEND_FACTOR:	rtbd[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;
	case Blend::DEST_ALPHA:		rtbd[0].DestBlend = D3D11_BLEND_DEST_ALPHA;
	case Blend::DEST_COLOR:		rtbd[0].DestBlend = D3D11_BLEND_DEST_COLOR;
	case Blend::INV_DEST_ALPHA:	rtbd[0].DestBlend = D3D11_BLEND_INV_DEST_ALPHA;
	case Blend::INV_DEST_COLOR:	rtbd[0].DestBlend = D3D11_BLEND_INV_DEST_COLOR;
	case Blend::INV_SRC_ALPHA:	rtbd[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	case Blend::INV_SRC_COLOR:	rtbd[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	case Blend::ONE:			rtbd[0].DestBlend = D3D11_BLEND_ONE;
	case Blend::SRC_ALPHA:		rtbd[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
	case Blend::SRC_COLOR:		rtbd[0].DestBlend = D3D11_BLEND_SRC_COLOR;
	case Blend::ZERO:			rtbd[0].DestBlend = D3D11_BLEND_ZERO;
	}

	switch (state.srcBlend)
	{
	case Blend::BLEND_FACTOR:	rtbd[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	case Blend::DEST_ALPHA:		rtbd[0].SrcBlend = D3D11_BLEND_DEST_ALPHA;
	case Blend::DEST_COLOR:		rtbd[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
	case Blend::INV_DEST_ALPHA:	rtbd[0].SrcBlend = D3D11_BLEND_INV_DEST_ALPHA;
	case Blend::INV_DEST_COLOR:	rtbd[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
	case Blend::INV_SRC_ALPHA:	rtbd[0].SrcBlend = D3D11_BLEND_INV_SRC_ALPHA;
	case Blend::INV_SRC_COLOR:	rtbd[0].SrcBlend = D3D11_BLEND_INV_SRC_COLOR;
	case Blend::ONE:			rtbd[0].SrcBlend = D3D11_BLEND_ONE;
	case Blend::SRC_ALPHA:		rtbd[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	case Blend::SRC_COLOR:		rtbd[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	case Blend::ZERO:			rtbd[0].SrcBlend = D3D11_BLEND_ZERO;
	}

	switch (state.dstBlendAlpha)
	{
	case Blend::BLEND_FACTOR:	rtbd[0].DestBlendAlpha = D3D11_BLEND_BLEND_FACTOR;
	case Blend::DEST_ALPHA:		rtbd[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	case Blend::DEST_COLOR:		rtbd[0].DestBlendAlpha = D3D11_BLEND_DEST_COLOR;
	case Blend::INV_DEST_ALPHA:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	case Blend::INV_DEST_COLOR:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_DEST_COLOR;
	case Blend::INV_SRC_ALPHA:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	case Blend::INV_SRC_COLOR:	rtbd[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_COLOR;
	case Blend::ONE:			rtbd[0].DestBlendAlpha = D3D11_BLEND_ONE;
	case Blend::SRC_ALPHA:		rtbd[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	case Blend::SRC_COLOR:		rtbd[0].DestBlendAlpha = D3D11_BLEND_SRC_COLOR;
	case Blend::ZERO:			rtbd[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	}
	
	switch (state.srcBlendAlpha)
	{
	case Blend::BLEND_FACTOR:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_BLEND_FACTOR;
	case Blend::DEST_ALPHA:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	case Blend::DEST_COLOR:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_DEST_COLOR;
	case Blend::INV_DEST_ALPHA:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	case Blend::INV_DEST_COLOR:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_COLOR;
	case Blend::INV_SRC_ALPHA:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	case Blend::INV_SRC_COLOR:	rtbd[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_COLOR;
	case Blend::ONE:			rtbd[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	case Blend::SRC_ALPHA:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	case Blend::SRC_COLOR:		rtbd[0].SrcBlendAlpha = D3D11_BLEND_SRC_COLOR;
	case Blend::ZERO:			rtbd[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	}
	rtbd[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	bd.AlphaToCoverageEnable = false;
	bd.RenderTarget[0] = rtbd[0];
	
	ID3D11BlendState* blendState;
	HRESULT hr = device->CreateBlendState(&bd, &blendState);
	if (FAILED(hr))
		throw std::exception("Failed to create blend state");

	blendStates[id] = blendState;
}

void SE::Graphics::PipelineHandler::DestroyBlendState(const Utilz::GUID& id)
{
	auto exists = blendStates.find(id);
	if (exists == blendStates.end())
		return;

	exists->second->Release();
	blendStates.erase(exists);
}

void SE::Graphics::PipelineHandler::CreateDepthStencilState(const Utilz::GUID& id, const DepthStencilState& state)
{
	const auto exists = depthStencilStates.find(id);
	if (exists != depthStencilStates.end())
		return;
	D3D11_DEPTH_STENCIL_DESC dsd;
	dsd.DepthEnable = state.enableDepth;
	switch(state.comparisonOperation)
	{
	case ComparisonOperation::EQUAL:			dsd.DepthFunc = D3D11_COMPARISON_EQUAL;
	case ComparisonOperation::GREATER:			dsd.DepthFunc = D3D11_COMPARISON_GREATER;
	case ComparisonOperation::GREATER_EQUAL:	dsd.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	case ComparisonOperation::LESS:				dsd.DepthFunc = D3D11_COMPARISON_LESS;
	case ComparisonOperation::LESS_EQUAL:		dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	case ComparisonOperation::NO_COMPARISON:	dsd.DepthFunc = D3D11_COMPARISON_NEVER;
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

}

void SE::Graphics::PipelineHandler::DestroyDepthStencilState(const Utilz::GUID& id)
{
	auto exists = depthStencilStates.find(id);
	if (exists == depthStencilStates.end())
		return;

	exists->second->Release();
	depthStencilStates.erase(exists);

}
