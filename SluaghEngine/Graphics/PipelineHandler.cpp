#include "PipelineHandler.h"

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
