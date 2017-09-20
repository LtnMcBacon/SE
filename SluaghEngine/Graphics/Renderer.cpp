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

	graphicResourceHandler = new GraphicResourceHandler(device->GetDevice(), device->GetDeviceContext());

	int shaderID[2];
	hr = graphicResourceHandler->CreateVertexShader(device->GetDevice(), &shaderID[0]);
	if (FAILED(hr))
		return -1;

	hr = graphicResourceHandler->CreatePixelShader(device->GetDevice(), &shaderID[1]);
	if (FAILED(hr))
		return -1;
	
	cam.SetPosition(0.0f, 1.0f, -2.0f);
	cam.Update(0.01f);
	TargetOffset off;
	off.shaderTarget[0] = true;
	off.shaderTarget[1] = true;
	off.shaderTarget[2] = true;
	off.offset[0] = 0;
	off.offset[1] = 0;
	off.offset[2] = 0;

	hr = graphicResourceHandler->CreateConstantBuffer(sizeof(OncePerFrameConstantBuffer), off, &oncePerFrameBufferID);
	if (FAILED(hr))
	{
		throw "omg";
	}

	graphicResourceHandler->BindConstantBuffer(oncePerFrameBufferID);


	return 0;
}

void SE::Graphics::Renderer::Shutdown()
{
	graphicResourceHandler->Shutdown();
	device->Shutdown();
	
	delete graphicResourceHandler;
	delete device;
}

int SE::Graphics::Renderer::EnableRendering(const RenderObjectInfo & handles)
{
	renderJobs.push_back(handles);
	return 0;
}

int SE::Graphics::Renderer::DisableRendering(const RenderObjectInfo & handles)
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

	DirectX::XMStoreFloat4x4(&wo, DirectX::XMMatrixTranspose(cam.ViewProj()));
	graphicResourceHandler->SetConstantBuffer(&wo, oncePerFrameBufferID);

	graphicResourceHandler->SetMaterial(0, 0);

	device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	for (auto& job : renderJobs)
	{
		graphicResourceHandler->SetVertexBuffer(job.bufferHandle);
		graphicResourceHandler->BindConstantBuffer(job.transformHandle);
		device->GetDeviceContext()->Draw(graphicResourceHandler->GetVertexCount(job.bufferHandle), 0);
	}




	device->Present();

	ProfileReturnConst(0);
}

int SE::Graphics::Renderer::CreateVertexBuffer(void * data, size_t vertexCount, size_t stride)
{
	StartProfile;
	int handle = 0;
	auto result = graphicResourceHandler->CreateVertexBuffer(data, vertexCount, stride, &handle);
	if(result)
		ProfileReturnConst(result);
	ProfileReturnConst(handle);
}

void SE::Graphics::Renderer::DestroyVertexBuffer(int bufferHandle)
{
	StartProfile;
	graphicResourceHandler->RemoveVertexBuffer(bufferHandle);
	StopProfile;
}

int Graphics::Renderer::CreateTexture(void* data, const TextureDesc& description)
{
	StartProfile;
	/*

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

	ID3D11Texture2D* texture;
	D3D11_SUBRESOURCE_DATA d;
	d.pSysMem = data;
	d.SysMemPitch = description.width * 4;
	d.SysMemSlicePitch = 0;
	HRESULT hr = S_OK;
	hr = device->GetDevice()->CreateTexture2D(&desc, &d, &texture);
	
	ID3D11ShaderResourceView* srv;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	hr = device->GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv);

	
	texture->Release();
	*/


	ProfileReturnConst(0);
}


int SE::Graphics::Renderer::CreateTransform()
{
	StartProfile;
	int handle;
	TargetOffset off;
	off.shaderTarget[0] = true;
	off.shaderTarget[1] = true;
	off.shaderTarget[2] = true;
	off.offset[0] = 1;
	off.offset[1] = 1;
	off.offset[2] = 1;
	auto hr = graphicResourceHandler->CreateConstantBuffer(sizeof(OncePerObjectConstantBuffer), off, &handle);
	if (FAILED(hr))
		ProfileReturnConst(hr);
	ProfileReturnConst(handle);
}

void SE::Graphics::Renderer::DestroyTransform(int transformHandle)
{
}

int SE::Graphics::Renderer::UpdateTransform(int transformHandle, float* transform)
{
	StartProfile;
	DirectX::XMMATRIX trans = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)transform);
	DirectX::XMFLOAT4X4 transposed;
	DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(trans));
	graphicResourceHandler->SetConstantBuffer(&transposed, transformHandle);
	ProfileReturnConst(0);
}

