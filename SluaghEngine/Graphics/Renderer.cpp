#include <Renderer.h>

using namespace SE;

SE::Graphics::Renderer::Renderer()
{
}

SE::Graphics::Renderer::~Renderer()
{
}

long SE::Graphics::Renderer::Initialize(void * window)
{
	_ASSERT(window);
	device = new DeviceManager();
	HRESULT hr = device->Init((HWND)window);
	if (FAILED(hr))
		return -1;

	//materialHandler = new MaterialHandler(device->GetDevice(), device->GetDeviceContext());
//	hr = materialHandler->Init();
	//if (FAILED(hr))
	//	return -1;

	return 0;
}

void SE::Graphics::Renderer::Shutdown()
{
//	materialHandler->Shutdown();
	device->Shutdown();
	
	
	//delete materialHandler;
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

int SE::Graphics::Renderer::CreateVertexBuffer(void * data, size_t size)
{
	return 0;
}

