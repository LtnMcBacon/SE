#include <Renderer.h>
#include <Profiler.h>


using namespace SE;

SE::Graphics::Renderer::Renderer()
{
}

SE::Graphics::Renderer::~Renderer()
{
}

int SE::Graphics::Renderer::Initialize(void * window)
{
	_ASSERT(window);
	device = new DeviceManager();
	HRESULT hr = device->Init((HWND)window);
	if (FAILED(hr))
		return -1;

	materialHandler = new MaterialHandler(device->GetDevice(), device->GetDeviceContext());
	hr = materialHandler->Init();
	if (FAILED(hr))
		return -1;

	staticVertexBufferHandler = new StaticVertexBufferHandler(device->GetDevice(), device->GetDeviceContext());
	constantBufferHandler = new ConstantBufferHandler(device->GetDevice(), device->GetDeviceContext());

	
	cam.SetPosition(0.0f, 1.0f, -2.0f);
	cam.Update(0.01f);
	TargetOffset off;
	off.shaderTarget[0] = true;
	off.shaderTarget[1] = true;
	off.shaderTarget[2] = true;
	off.offset[0] = 0;
	off.offset[1] = 0;
	off.offset[2] = 0;

	hr = constantBufferHandler->AddConstantBuffer(sizeof(OncePerFrameConstantBuffer), off, &oncePerFrameBufferID);
	if (FAILED(hr))
	{
		throw "omg";
	}
	DirectX::XMFLOAT4X4 vp;
	DirectX::XMStoreFloat4x4(&vp, cam.ViewProj());
	constantBufferHandler->SetConstantBuffer(&vp, oncePerFrameBufferID);


	off.offset[0] = 1;
	off.offset[1] = 1;
	off.offset[2] = 1;
	hr = constantBufferHandler->AddConstantBuffer(sizeof(OncePerObjectConstantBuffer), off, &oncePerObjectBufferID);
	if (FAILED(hr))
	{
		throw "omg2";
	}


	return 0;
}

void SE::Graphics::Renderer::Shutdown()
{
	constantBufferHandler->Shutdown();
	staticVertexBufferHandler->Shutdown();
	materialHandler->Shutdown();
	device->Shutdown();
	
	delete constantBufferHandler;
	delete staticVertexBufferHandler;
	delete materialHandler;
	delete device;
}

int SE::Graphics::Renderer::CreateRenderObject(const Entity & entity, const RenderObjectInfo & info)
{
	return 0;
}

int SE::Graphics::Renderer::DestroyRenderObject(const Entity & entity)
{
	return 0;
}

int SE::Graphics::Renderer::EnableRendering(const Entity & entity)
{
	return 0;
}

int SE::Graphics::Renderer::DisableRendering(const Entity & entity)
{
	return 0;
}

int SE::Graphics::Renderer::UpdateTranslation(const Entity & entity, float * transform)
{
	return 0;
}

int SE::Graphics::Renderer::UpdateView(float * viewMatrix)
{
	return 0;
}

int SE::Graphics::Renderer::Render() {
	StartProfile;
	// clear the back buffer
	float clearColor[] = { 0, 0, 1, 1 };


	cam.Update(0.01f);

	ID3D11RenderTargetView* views[] = { device->GetRTV() };
	device->GetDeviceContext()->OMSetRenderTargets(1, views, device->GetDepthStencil());

	// Clear the primary render target view using the specified color
	device->GetDeviceContext()->ClearRenderTargetView(
	device->GetRTV(), 
	clearColor);

	// Clear the standard depth stencil view
	device->GetDeviceContext()->ClearDepthStencilView(
	device->GetDepthStencil(), 
	D3D11_CLEAR_DEPTH,  
	1.0f, 
	0);
	DirectX::XMFLOAT4X4 wo;
	DirectX::XMStoreFloat4x4(&wo, DirectX::XMMatrixIdentity());
	constantBufferHandler->SetConstantBuffer(&wo, oncePerObjectBufferID);

	DirectX::XMStoreFloat4x4(&wo, DirectX::XMMatrixTranspose(cam.ViewProj()));
	constantBufferHandler->SetConstantBuffer(&wo, oncePerFrameBufferID);

	materialHandler->SetMaterial();
	staticVertexBufferHandler->SetVertexBuffer(0);

	device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device->GetDeviceContext()->Draw(staticVertexBufferHandler->GetVertexCount(0), 0);

	device->Present();

	ProfileReturnConst(0);
}

int SE::Graphics::Renderer::CreateVertexBuffer(void * data, size_t vertexCount, size_t stride)
{
	StartProfile;
	int handle = 0;
	staticVertexBufferHandler->CreateVertexBuffer(data, vertexCount, stride, &handle);
	ProfileReturnConst(handle);
}

