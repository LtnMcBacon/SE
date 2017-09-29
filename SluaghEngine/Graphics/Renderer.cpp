#include <Renderer.h>
#include <Profiler.h>
#include <ResourceHandler\IResourceHandler.h>

#undef min
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
	TargetOffset t = { {true, false, false},{2, 0, 0} };
	hr = graphicResourceHandler->CreateConstantBuffer(sizeof(DirectX::XMFLOAT4X4) * maxDrawInstances, t, &instancedTransformsConstantBufferHandle);
	if(FAILED(hr))
	{
		throw std::exception("Could not create OncePerObject constant buffer.\n");
	}
	hr = graphicResourceHandler->CreateConstantBuffer(sizeof(DirectX::XMFLOAT4X4), t, &singleTransformConstantBuffer);
	if (FAILED(hr))
	{
		throw std::exception("Could not create singleTransformConstantBuffer.\n");
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
	delete device;
}

int SE::Graphics::Renderer::EnableRendering(const RenderObjectInfo & handles)
{
	StartProfile;
	int32_t bucketIndex = -1;
	const size_t renderBucketCount = renderBuckets.size();
	for (size_t i = 0; i < renderBucketCount; ++i)
	{
		if (renderBuckets[i].stateInfo - handles == 0)
		{
			bucketIndex = i;
			break;
		}
	}
	if(bucketIndex < 0)
	{
		bucketIndex = renderBuckets.size();
		TargetOffset t = { {true, false, false},{2, 0, 0} };
		renderBuckets.push_back({ handles, {}, {} });	
	}
	const size_t transformIndex = renderBuckets[bucketIndex].transforms.size();
	DirectX::XMFLOAT4X4 identityMatrix;
	DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());
	renderBuckets[bucketIndex].transforms.push_back(identityMatrix);
	
	uint32_t jobID;
	if (freeJobIndices.size())
	{
		jobID = freeJobIndices.top();
		freeJobIndices.pop();
	}
	else
	{
		jobID = jobIDToBucketAndTransformIndex.size();
		jobIDToBucketAndTransformIndex.push_back({ 0,0 });
	}
	
	const BucketAndTransformIndex bucketAndTransformIndex = { bucketIndex, transformIndex };
	jobIDToBucketAndTransformIndex[jobID] = bucketAndTransformIndex;
	renderBuckets[bucketIndex].jobsInBucket.push_back(jobID);
	ProfileReturnConst(jobID);
}

int Graphics::Renderer::DisableRendering(uint32_t jobID)
{
	StartProfile;
	const uint32_t bucketIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].bucketIndex;
	const uint32_t transformIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].transformIndex;

	auto& bucketOfRemoved = renderBuckets[bucketIndexOfRemoved];

	bucketOfRemoved.transforms[transformIndexOfRemoved] = bucketOfRemoved.transforms.back();
	bucketOfRemoved.transforms.pop_back();
	const uint32_t jobThatReplacedOld = bucketOfRemoved.jobsInBucket.back();
	bucketOfRemoved.jobsInBucket[transformIndexOfRemoved] = jobThatReplacedOld;
	bucketOfRemoved.jobsInBucket.pop_back();

	jobIDToBucketAndTransformIndex[jobThatReplacedOld].transformIndex = transformIndexOfRemoved;
	jobIDToBucketAndTransformIndex[jobThatReplacedOld].bucketIndex = bucketIndexOfRemoved;

	freeJobIndices.push(jobID);


	ProfileReturnConst(0);
}

int Graphics::Renderer::AddLineRenderJob(const LineRenderJob& lineJob)
{
	StartProfile;
	uint32_t lineJobID;
	if(freeLineRenderJobIndices.size())
	{
		lineJobID = freeLineRenderJobIndices.top();
		freeLineRenderJobIndices.pop();
		lineRenderJobs[lineJobID] = lineJob;
	}
	else
	{
		lineJobID = lineRenderJobs.size();
		lineRenderJobs.push_back(lineJob);
	}
	DirectX::XMMATRIX trans = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&lineRenderJobs[lineJobID].transform);
	DirectX::XMFLOAT4X4 transposed;
	DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(trans));
	lineRenderJobs[lineJobID].transform = transposed;
	ProfileReturnConst(lineJobID);
}

int Graphics::Renderer::RemoveLineRenderJob(uint32_t lineJobID)
{
	StartProfile;
	freeLineRenderJobIndices.push(lineJobID);
	lineRenderJobs[lineJobID].verticesToDrawCount = 0;
	ProfileReturnConst(0);
}

int Graphics::Renderer::UpdateLineRenderJobTransform(uint32_t lineJobID, float* transform)
{
	StartProfile;
	DirectX::XMMATRIX trans = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)transform);
	DirectX::XMFLOAT4X4 transposed;
	DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(trans));
	lineRenderJobs[lineJobID].transform = transposed;
	ProfileReturnConst(0);
}

int Graphics::Renderer::UpdateLineRenderJobRange(uint32_t lineJobID, uint32_t startVertex, uint32_t vertexCount)
{
	StartProfile;

	lineRenderJobs[lineJobID].firstVertex = startVertex;
	lineRenderJobs[lineJobID].verticesToDrawCount = vertexCount;

	ProfileReturnConst(0);
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

	graphicResourceHandler->BindConstantBuffer(instancedTransformsConstantBufferHandle);
	RenderObjectInfo previousJob;
	previousJob.textureCount = 0;
	for (int i = 0; i < RenderObjectInfo::maxTextureBinds; ++i)
	{
		previousJob.textureHandles[i] = -1;
		previousJob.textureBindings[i] = -1;
	}
	previousJob.bufferHandle = -1;
	previousJob.pixelShader = -1;
	previousJob.topology = RenderObjectInfo::PrimitiveTopology::TRIANGLE_LIST;
	previousJob.vertexShader = -1;
	for(auto& bucket : renderBuckets)
	{
		const RenderObjectInfo& job = bucket.stateInfo;
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
		for (int i = 0; i < job.textureCount; ++i)
			if(previousJob.textureHandles[i] != job.textureHandles[i] || previousJob.textureBindings[i] != job.textureBindings[i])
				graphicResourceHandler->BindShaderResourceView(job.textureHandles[i], job.textureBindings[i]);

		const size_t instanceCount = bucket.transforms.size();
		for(int i = 0; i < instanceCount; i += maxDrawInstances)
		{
			const size_t instancesToDraw = std::min(bucket.transforms.size() - i, (size_t)maxDrawInstances);
			const size_t mapSize = sizeof(DirectX::XMFLOAT4X4) * instancesToDraw;
			graphicResourceHandler->UpdateConstantBuffer(&bucket.transforms[i], mapSize, instancedTransformsConstantBufferHandle);
			device->GetDeviceContext()->DrawInstanced(graphicResourceHandler->GetVertexCount(bucket.stateInfo.bufferHandle), instancesToDraw, 0, 0);
		}
		previousJob = job;
	}
	/********** Render line jobs ************/

	device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	graphicResourceHandler->BindConstantBuffer(singleTransformConstantBuffer);
	for(auto& lineJob : lineRenderJobs)
	{
		if (lineJob.verticesToDrawCount == 0)
			continue;
		graphicResourceHandler->UpdateConstantBuffer(&lineJob.transform, sizeof(lineJob.transform), singleTransformConstantBuffer);
		graphicResourceHandler->SetMaterial(lineJob.vertexShaderHandle, lineJob.pixelShaderHandle);
		graphicResourceHandler->SetVertexBuffer(lineJob.vertexBufferHandle);
		device->GetDeviceContext()->Draw(lineJob.verticesToDrawCount, lineJob.firstVertex);
	}
	
	/********END render line jobs************/


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

int Graphics::Renderer::UpdateTransform(uint32_t jobID, float* transform)
{
	StartProfile;
	DirectX::XMMATRIX trans = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)transform);
	DirectX::XMFLOAT4X4 transposed;
	DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(trans));
	
	const size_t bucketIndex = jobIDToBucketAndTransformIndex[jobID].bucketIndex;
	const size_t transformIndex = jobIDToBucketAndTransformIndex[jobID].transformIndex;

	renderBuckets[bucketIndex].transforms[transformIndex] = transposed;

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

