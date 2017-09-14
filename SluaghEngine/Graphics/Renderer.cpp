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

	return 0;
}

void SE::Graphics::Renderer::Shutdown()
{
	staticVertexBufferHandler->Shutdown();
	materialHandler->Shutdown();
	device->Shutdown();
	
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

	device->Present();

	ProfileReturnConst(0);
}

int SE::Graphics::Renderer::CreateVertexBuffer(void * data, size_t size)
{
	StartProfile;
	int handle = 0;
	staticVertexBufferHandler->CreateVertexBuffer(data, size, &handle);
	ProfileReturnConst(handle);
}

