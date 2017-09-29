#include <Renderer.h>
#include <Profiler.h>
#include <ResourceHandler\IResourceHandler.h>

using namespace SE;

SE::Graphics::Renderer::Renderer()
{
}

SE::Graphics::Renderer::~Renderer()
{
}

int SE::Graphics::Renderer::Initialize(void * window)
{
	StartProfile;
	_ASSERT(window);
	device = new DeviceManager();
	HRESULT hr = device->Init((HWND)window);
	if (FAILED(hr))
		return -1;

	graphicResourceHandler = new GraphicResourceHandler(device->GetDevice(), device->GetDeviceContext());
	
	spriteBatch = std::make_unique<DirectX::SpriteBatch>(device->GetDeviceContext());

	animationSystem = new AnimationSystem();
	
	TargetOffset off;
	off.shaderTarget[0] = true;
	off.shaderTarget[1] = true;
	off.shaderTarget[2] = true;
	off.offset[0] = 1;
	off.offset[1] = 1;
	off.offset[2] = 1;

	hr = graphicResourceHandler->CreateConstantBuffer(sizeof(OncePerFrameConstantBuffer), off, &oncePerFrameBufferID);
	if (FAILED(hr))
	{
		throw std::exception("Could not create OncePerFrameConstantBuffer");
	}

	graphicResourceHandler->BindConstantBuffer(oncePerFrameBufferID);

	graphicResourceHandler->CreateSamplerState();

	ProfileReturnConst( 0);
}

void SE::Graphics::Renderer::Shutdown()
{
	graphicResourceHandler->Shutdown();
	device->Shutdown();
	
	delete graphicResourceHandler;
	delete animationSystem;
	delete device;
}

int SE::Graphics::Renderer::EnableRendering(const RenderObjectInfo & handles)
{
	renderJobs.push_back(handles);

	int insertion = renderJobs.size() - 1;
	const int prior = insertion - 1;

	int minChange = 0;
	if (renderJobs.size() >= 2)
		minChange = handles - renderJobs[prior];

	for (int i = 0; i <= prior; i++)
	{
		int change = renderJobs[i] - handles;
		if (change <= minChange)
		{
			minChange = change;
			insertion = i;
		}
	}

	if (insertion != prior)
	{
		for (int i = prior + 1; i > insertion; i--)
		{
			renderJobs[i] = renderJobs[i - 1];
		}
		renderJobs[insertion] = handles;
	}

	return 0;
}

int SE::Graphics::Renderer::DisableRendering(const RenderObjectInfo & handles)
{
	const int size = renderJobs.size();
	int at = -1;
	for(size_t i = 0; i < size; ++i)
	{
		if(handles - renderJobs[i] == 0)
		{
			at = i;
			break;
		}
	}
	if (at >= 0)
	{
		for (int i = at; i < size - 1; ++i)
			renderJobs[i] = renderJobs[i + 1];
		renderJobs.pop_back();
	}
	return 0;
}

int SE::Graphics::Renderer::EnableTextRendering(const TextGUI& handles)
{
	renderTextJobs.push_back(handles);
	return 0;
}

int SE::Graphics::Renderer::DisableTextRendering(const TextGUI& handles)
{
	const int size = renderTextJobs.size();
	int at = -1;
	for (size_t i = 0; i < size; ++i)
	{
		if (handles.hashString == renderTextJobs[i].hashString)
		{
			at = i;
			break;
		}
	}
	if (at >= 0)
	{
		for (int i = at; i < size - 1; ++i)
			renderTextJobs[i] = renderTextJobs[i + 1];
		renderTextJobs.pop_back();
	}
	return 0;
}

int SE::Graphics::Renderer::EnableTextureRendering(const GUITextureInfo & handles)
{
	renderTextureJobs.push_back(handles);
	return 0;
}

int SE::Graphics::Renderer::DisableTextureRendering(const GUITextureInfo & handles)
{
	const int size = renderTextureJobs.size();
	int at = -1;
	for (size_t i = 0; i < size; ++i)
	{
		if (handles == renderTextureJobs[i])
		{
			at = i;
			break;
		}
	}
	if (at >= 0)
	{
		for (int i = at; i < size - 1; ++i)
			renderTextureJobs[i] = renderTextureJobs[i + 1];
		renderTextureJobs.pop_back();
	}
	return 0;
}

int SE::Graphics::Renderer::UpdateView(float * viewMatrix)
{
	DirectX::XMFLOAT4X4 wo;
	DirectX::XMStoreFloat4x4(&wo, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)viewMatrix)));
	graphicResourceHandler->SetConstantBuffer(&wo, oncePerFrameBufferID);

	return 0;
}

int SE::Graphics::Renderer::Render() {
	StartProfile;
	// clear the back buffer
	float clearColor[] = { 0, 0, 1, 1 };


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



	
	device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	RenderObjectInfo previousJob;
	previousJob.textureCount = 0;
	for (int i = 0; i < RenderObjectInfo::maxTextureBinds; ++i)
	{
		previousJob.textureHandles[i] = -1;
		previousJob.textureBindings[i] = -1;
	}
	previousJob.bufferHandle = -1;
	previousJob.pixelShader = -1;
	previousJob.transformHandle = -1;
	previousJob.vertexShader = -1;
	for (auto& job : renderJobs)
	{
		if (previousJob.topology != job.topology)
		{
			switch (job.topology)
			{
				case RenderObjectInfo::PrimitiveTopology::LINE_LIST:
				{
					device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
					break;
				}
				case RenderObjectInfo::PrimitiveTopology::POINT_LIST:
				{
					device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
					break;
				}
				case RenderObjectInfo::PrimitiveTopology::TRIANGLE_LIST:
				{
					device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					break;
				}
				case RenderObjectInfo::PrimitiveTopology::LINE_STRIP:
				{
					device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
					break;
				}
				case RenderObjectInfo::PrimitiveTopology::TRIANGLE_STRIP:
				{
					device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
					break;
				}
			}
		}
		if(previousJob.pixelShader != job.pixelShader || previousJob.vertexShader != job.vertexShader)
			graphicResourceHandler->SetMaterial(job.vertexShader, job.pixelShader);
		if(previousJob.bufferHandle != job.bufferHandle)
			graphicResourceHandler->SetVertexBuffer(job.bufferHandle);
		if(previousJob.transformHandle != job.transformHandle)
			graphicResourceHandler->BindConstantBuffer(job.transformHandle);
		for (int i = 0; i < job.textureCount; ++i)
			if(previousJob.textureHandles[i] != job.textureHandles[i] || previousJob.textureBindings[i] != job.textureBindings[i])
				graphicResourceHandler->BindShaderResourceView(job.textureHandles[i], job.textureBindings[i]);

		device->GetDeviceContext()->Draw(graphicResourceHandler->GetVertexCount(job.bufferHandle), 0);
		previousJob = job;
	}

	spriteBatch->Begin(DirectX::SpriteSortMode_Texture, device->GetBlendState());
	for (auto& job : renderTextureJobs)
	{
		spriteBatch->Draw(graphicResourceHandler->GetShaderResourceView(job.textureID), job.pos, job.rect, XMLoadFloat3(&job.colour), job.rotation, job.origin, job.scale, job.effect, job.layerDepth);
	}
	spriteBatch->End();

	spriteBatch->Begin();
	for (auto& job : renderTextJobs)
	{
		fonts[job.fontID].DrawString(spriteBatch.get(), job.text.c_str(), job.pos, XMLoadFloat3(&job.colour), job.rotation, job.origin, job.scale, job.effect, job.layerDepth);
	}
	spriteBatch->End();

	device->SetBlendState();

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
	ProfileReturn(graphicResourceHandler->CreateShaderResourceView(data, description))
}


int SE::Graphics::Renderer::CreateTransform()
{
	StartProfile;
	int handle;
	TargetOffset off;
	off.shaderTarget[0] = true;
	off.shaderTarget[1] = true;
	off.shaderTarget[2] = true;
	off.offset[0] = 2;
	off.offset[1] = 2;
	off.offset[2] = 2;
	auto hr = graphicResourceHandler->CreateConstantBuffer(sizeof(DirectX::XMFLOAT4X4), off, &handle);
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



int SE::Graphics::Renderer::CreatePixelShader(void* data, size_t size, ShaderSettings* reflection)
{
	int handle;
	auto hr = graphicResourceHandler->CreatePixelShader(device->GetDevice(),data, size, &handle, reflection );
	if (FAILED(hr))
		return hr;

	return handle;
}

int Graphics::Renderer::CreateDynamicVertexBuffer(size_t bytewidth, size_t vertexByteSize, void* initialData, size_t initialDataSize)
{
	StartProfile;
	ProfileReturn(graphicResourceHandler->CreateDynamicVertexBuffer(bytewidth, vertexByteSize, initialData, initialDataSize));
}

int Graphics::Renderer::UpdateDynamicVertexBuffer(int handle, void* data, size_t totalSize, size_t sizePerElement)
{
	StartProfile;
	ProfileReturn(graphicResourceHandler->UpdateDynamicVertexBuffer(handle, data, totalSize, sizePerElement));
}

int SE::Graphics::Renderer::CreateVertexShader(void * data, size_t size)
{
	int handle;
	auto hr = graphicResourceHandler->CreateVertexShader(device->GetDevice(),data, size, &handle);
	if (hr)
		return hr;
	return handle;
}

int SE::Graphics::Renderer::RetFontData(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	fonts.push_back(DirectX::SpriteFont(device->GetDevice(), (uint8_t*)data, size));
	ProfileReturn(0);
}

int SE::Graphics::Renderer::CreateTextFont(Utilz::GUID fontFile, ResourceHandler::IResourceHandler* resourceHandler)
{
	resourceHandler->LoadResource(fontFile, ResourceHandler::LoadResourceDelegate::Make<Renderer, &Renderer::RetFontData>(this));
	return fonts.size();
}

void SE::Graphics::Renderer::ResizeSwapChain(void* windowHandle)
{
	device->ResizeSwapChain((HWND)windowHandle);
}


int SE::Graphics::Renderer::CreateSkeleton(SE::Core::JointAttributes* jointData, size_t nrOfJoints) {

	int handle;
	auto hr = animationSystem->AddSkeleton(jointData, nrOfJoints, &handle);
	if (hr)
		return hr;
	return handle;
}

int SE::Graphics::Renderer::CreateAnimation(DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints, size_t skeletonIndex) {

	int handle;
	auto hr = animationSystem->AddAnimation(matrices, nrOfKeyframes, nrOfJoints, skeletonIndex, &handle);
	if (hr)
		return hr;
	return handle;
}

