#include "PipelineHandler.h"
#include "Profiler.h"
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
	if (exists == indexBuffers.end())
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
