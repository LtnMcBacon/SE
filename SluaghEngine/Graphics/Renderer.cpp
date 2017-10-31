#include "Renderer.h"
#include <Profiler.h>
#include <ResourceHandler\IResourceHandler.h>
#include "GPUTimeCluster.h"
#include <Utilz\CPUTimeCluster.h>
#include "PipelineHandler.h"
#include <algorithm>
#undef min



#ifdef _DEBUG
#pragma comment(lib, "DirectXTKD.lib")
#else
#pragma comment(lib, "DirectXTK.lib")
#endif

SE::Graphics::Renderer::Renderer() 
{
	device = nullptr;
	graphicResourceHandler = nullptr;
	memMeasure.Init();
}

SE::Graphics::Renderer::~Renderer()
{
}

int SE::Graphics::Renderer::Initialize(const InitializationInfo& initInfo)
{
	StartProfile;
	_ASSERT(initInfo.window);
	this->initInfo = initInfo;
	device = new DeviceManager();
	HRESULT hr = device->Init((HWND)initInfo.window);
	if (FAILED(hr))
		return hr;

	graphicResourceHandler = new GraphicResourceHandler(device->GetDevice(), device->GetDeviceContext());
	
	pipelineHandler = new PipelineHandler(device->GetDevice(), device->GetDeviceContext(),device->GetRTV(), device->GetDepthStencil());
	pipelineHandler->AddExistingRenderTargetView("backbuffer", device->GetRTV());
	pipelineHandler->AddExistingDepthStencilView("backbuffer", device->GetDepthStencil());
	pipelineHandler->AddExisitingShaderResourceView("backbufferdepth", device->GetDepthStencilSRV());
	//secPipelineHandler = new PipelineHandler(device->GetDevice(), device->GetSecondaryDeviceContext(), nullptr, nullptr);

	Graphics::Viewport vp;
	vp.width = device->GetTexDesc().Width;
	vp.height = device->GetTexDesc().Height;
	vp.maxDepth = 1.0f;
	vp.minDepth = 0.0f;
	vp.topLeftX = 0.0f;
	vp.topLeftY = 0.0f;

	pipelineHandler->CreateViewport(Utilz::GUID(), vp);


	spriteBatch = new DirectX::SpriteBatch(device->GetDeviceContext());

	//animationSystem = new AnimationSystem();

	

	graphicResourceHandler->CreateSamplerState();

	

	gpuTimer = new GPUTimeCluster(device->GetDevice(), device->GetDeviceContext());

	ProfileReturnConst( 0);
}

void SE::Graphics::Renderer::Shutdown()
{

	delete spriteBatch;
	fonts.clear();
	delete pipelineHandler;
	graphicResourceHandler->Shutdown();
	

	delete gpuTimer;
	delete graphicResourceHandler;
	device->Shutdown();
	delete device;
}

uint32_t SE::Graphics::Renderer::AddRenderJob(const RenderJob& job, RenderGroup group)
{
	const uint32_t idPart = jobIDCounter;
	const auto jobID = ((jobIDCounter++) | (static_cast<uint8_t>(group) << JOB_ID_BITS));

	InternalRenderJob j = { job, idPart };
	jobIDToIndex[idPart] = jobGroups[static_cast<uint8_t>(group)].size();
	jobGroups[static_cast<uint8_t>(group)].push_back(j);
	return jobID;
}


void SE::Graphics::Renderer::RemoveRenderJob(uint32_t jobID)
{
	//Which entry in the jobGroups map
	const uint8_t jobGroup = (jobID >> JOB_ID_BITS) & JOB_GROUP_MASK;
	//Which entry in jobIDToIndex map
	const uint32_t idPart = (jobID & JOB_ID_MASK);
	//Which index in the entry in the jobgroups map
	const uint32_t indexInMap = jobIDToIndex[idPart];
	//The ID part of the jobID that will move places inside the vector
	const uint32_t replacementID = jobGroups[jobGroup].back().jobID;
	//The index of the job that will move places
	const uint32_t replacementIndex = jobIDToIndex[replacementID];
	jobGroups[jobGroup][indexInMap] = jobGroups[jobGroup][replacementIndex];
	jobGroups[jobGroup].pop_back();
	jobIDToIndex[replacementID] = indexInMap;
	jobIDToIndex.erase(idPart);
}

void SE::Graphics::Renderer::ChangeRenderJob(uint32_t jobID, const RenderJob& newJob)
{
	const uint8_t jobGroup = (jobID >> JOB_ID_BITS) & JOB_GROUP_MASK;
	const uint32_t idPart = (jobID & JOB_ID_MASK);
	const uint32_t indexInMap = jobIDToIndex[idPart];
	jobGroups[jobGroup][indexInMap].job = newJob;
}

void SE::Graphics::Renderer::ChangeRenderJob(uint32_t jobID, const std::function<void(RenderJob&job)>& callback)
{
	const uint8_t jobGroup = (jobID >> JOB_ID_BITS) & JOB_GROUP_MASK;
	const uint32_t idPart = (jobID & JOB_ID_MASK);
	const uint32_t indexInMap = jobIDToIndex[idPart];
	callback(jobGroups[jobGroup][indexInMap].job);
}

size_t SE::Graphics::Renderer::EnableTextRendering(const TextGUI& handles)
{
	StartProfile;
	int job = (int)renderTextJobs.size();
	renderTextJobs.push_back(handles);
	if (!renderTextJobs[job].anchor)
	{
		D3D11_TEXTURE2D_DESC	gBB_Desc = device->GetTexDesc();
		size_t height = gBB_Desc.Height;
		size_t width = gBB_Desc.Width;
		renderTextJobs[job].scale = DirectX::XMFLOAT2(renderTextJobs[job].scale.x * width, renderTextJobs[job].scale.y * height);
		renderTextJobs[job].pos = DirectX::XMFLOAT2(renderTextJobs[job].pos.x * width, renderTextJobs[job].pos.y * height);
	}
	ProfileReturn(job);
	return 0;
}

size_t SE::Graphics::Renderer::DisableTextRendering(const size_t & jobID)
{
	StartProfile;
	size_t job = renderTextJobs.size() - 1;
	renderTextJobs[jobID] = renderTextJobs[job];
	renderTextJobs.pop_back();
	ProfileReturn(job);
}

size_t SE::Graphics::Renderer::EnableTextureRendering(const GUITextureInfo & handles)
{
	StartProfile;
	int job = (int)renderTextureJobs.size();
	renderTextureJobs.push_back(handles);
	if (!renderTextureJobs[job].anchor)
	{
		D3D11_TEXTURE2D_DESC	gBB_Desc = device->GetTexDesc();
		float height = gBB_Desc.Height;
		float width = gBB_Desc.Width;
		if (renderTextureJobs[job].rect)
		{
			renderTextureJobs[job].rect->bottom = renderTextureJobs[job].rect->bottom * height;
			renderTextureJobs[job].rect->top = renderTextureJobs[job].rect->top * height;
			renderTextureJobs[job].rect->right = renderTextureJobs[job].rect->right * width;
			renderTextureJobs[job].rect->left = renderTextureJobs[job].rect->left * width;
		}
		renderTextureJobs[job].scale = DirectX::XMFLOAT2(renderTextureJobs[job].scale.x * width, renderTextureJobs[job].scale.y * height);
		renderTextureJobs[job].pos = DirectX::XMFLOAT2(renderTextureJobs[job].pos.x * width, renderTextureJobs[job].pos.y * height);
	}
	ProfileReturn(int(job));
}

size_t SE::Graphics::Renderer::DisableTextureRendering(const size_t & jobID)
{
	StartProfile;
	size_t job = renderTextureJobs.size() - 1;
	renderTextureJobs[jobID] = renderTextureJobs[job];
	renderTextureJobs.pop_back();
	ProfileReturn(int(job));
}


int SE::Graphics::Renderer::Render() 
{
	StartProfile;
	auto devContext = device->GetDeviceContext();
	/******************General Jobs*********************/
	cpuTimer.Start(CREATE_ID_HASH("RenderJob-CPU"));
	gpuTimer->Start(CREATE_ID_HASH("RenderJob-GPU"));
	
	bool first = true;
	for (auto& group : jobGroups)
	{
		first = true;
		for (auto& j : group.second)
		{
			int32_t drawn = 0;
			if (first)
			{
				pipelineHandler->SetPipelineForced(j.job.pipeline);
				first = false;
			}
			else
			{
				pipelineHandler->SetPipeline(j.job.pipeline);
			}
			if (j.job.indexCount == 0 && j.job.instanceCount == 0 && j.job.vertexCount != 0)
			{
				for(auto& mf : j.job.mappingFunc)
					mf(drawn, 1);
				device->GetDeviceContext()->Draw(j.job.vertexCount, j.job.vertexOffset);
			}
			else if (j.job.indexCount != 0 && j.job.instanceCount == 0)
			{
				for (auto& mf : j.job.mappingFunc)
					mf(drawn, 1);
				device->GetDeviceContext()->DrawIndexed(j.job.indexCount, j.job.indexOffset, j.job.vertexOffset);
			}
			else if (j.job.indexCount == 0 && j.job.instanceCount != 0)
			{
				while (drawn < j.job.instanceCount)
				{
				
					const uint32_t toDraw = std::min(j.job.maxInstances, j.job.instanceCount - drawn);
					for (auto& mf : j.job.mappingFunc)
						mf(drawn, toDraw);
					device->GetDeviceContext()->DrawInstanced(j.job.vertexCount, toDraw, j.job.vertexOffset, j.job.instanceOffset);
					drawn += toDraw;
				}
			}
			else if (j.job.indexCount != 0 && j.job.instanceCount != 0)
			{
				while (drawn < j.job.instanceCount)
				{		
					const uint32_t toDraw = std::min(j.job.maxInstances, j.job.instanceCount - drawn);
					for (auto& mf : j.job.mappingFunc)
						mf(drawn, toDraw);
					device->GetDeviceContext()->DrawIndexedInstanced(j.job.indexCount, toDraw, j.job.indexOffset, j.job.vertexOffset, j.job.instanceOffset);
					drawn += toDraw;
				}
			}
			else if (j.job.vertexCount == 0)
			{
				for (auto& mf : j.job.mappingFunc)
					mf(drawn, 0);
				device->GetDeviceContext()->DrawAuto();
			}
		}
		ID3D11ShaderResourceView* nullSRVS[8] = { nullptr };
		ID3D11RenderTargetView* nullRTVS[8] = { nullptr };

		device->GetDeviceContext()->OMSetRenderTargets(8, nullRTVS, nullptr);
		device->GetDeviceContext()->PSSetShaderResources(0, 8, nullSRVS);
	}
	{
		ID3D11RenderTargetView* backbuf = device->GetRTV();
		ID3D11DepthStencilView* depthsv = device->GetDepthStencil();
		device->GetDeviceContext()->OMSetRenderTargets(1, &backbuf, depthsv);
	}
	gpuTimer->Stop(CREATE_ID_HASH("RenderJob-GPU"));
	cpuTimer.Stop(CREATE_ID_HASH("RenderJob-CPU"));
	/*****************End General Jobs******************/


	//********* Render sprite overlays ********/
	cpuTimer.Start(CREATE_ID_HASH("GUIJob-CPU"));
	gpuTimer->Start(CREATE_ID_HASH("GUIJob-GPU"));
	if (renderTextureJobs.size() && renderTextJobs.size())
	{
		spriteBatch->Begin(DirectX::SpriteSortMode_BackToFront, device->GetBlendState());
		for (auto& job : renderTextureJobs)
		{
			spriteBatch->Draw(graphicResourceHandler->GetShaderResourceView(job.textureID), job.pos,(tagRECT*)job.rect, XMLoadFloat4(&job.colour), job.rotation, job.origin, job.scale, (DirectX::SpriteEffects)job.effect, job.layerDepth);
		}

		for (auto& job : renderTextJobs)
		{
			fonts[job.fontID].DrawString(spriteBatch, job.text.c_str(), job.pos, XMLoadFloat4(&job.colour), job.rotation, job.origin, job.scale, (DirectX::SpriteEffects)job.effect, job.layerDepth);
		}
		spriteBatch->End();
	}
	gpuTimer->Stop(CREATE_ID_HASH("GUIJob-GPU"));
	cpuTimer.Stop(CREATE_ID_HASH("GUIJob-CPU"));

	device->SetDepthStencilStateAndRS();
	device->SetBlendTransparencyState(0);


	////********* Apply bloom post-processing ********/
	//if (bloom)
	//{
	//	cpuTimer.Start(CREATE_ID_HASH("Bloom-CPU"));
	//	gpuTimer->Start(CREATE_ID_HASH("Bloom-GPU"));


	//	ID3D11ShaderResourceView* shaderResourceViews[] = {
	//		graphicResourceHandler->GetShaderResourceView(bloomShaderResourceViewHandles[0]),
	//		graphicResourceHandler->GetShaderResourceView(bloomShaderResourceViewHandles[1]),
	//		graphicResourceHandler->GetShaderResourceView(bloomShaderResourceViewHandles[2])
	//	};
	//	ID3D11UnorderedAccessView* unorderedAccessViews[] = {
	//		graphicResourceHandler->GetUnorderedAccessView(bloomUnorderedAccessViewHandles[0]),
	//		graphicResourceHandler->GetUnorderedAccessView(bloomUnorderedAccessViewHandles[1])
	//	};


	//	ID3D11RenderTargetView* nullRTVs[] = { nullptr, nullptr };
	//	device->GetDeviceContext()->OMSetRenderTargets(2, nullRTVs, NULL);

	//	graphicResourceHandler->SetCompute(bloomHorizontalHandle);
	//	device->GetDeviceContext()->CSSetShaderResources(0, 1, &shaderResourceViews[1]);
	//	device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &unorderedAccessViews[0], NULL);
	//	device->GetDeviceContext()->Dispatch(32, 45, 1);

	//	graphicResourceHandler->SetCompute(bloomVerticalHandle);
	//	device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &unorderedAccessViews[1], NULL);
	//	device->GetDeviceContext()->CSSetShaderResources(0, 1, &shaderResourceViews[2]);
	//	device->GetDeviceContext()->CSSetShaderResources(1, 1, &shaderResourceViews[0]);
	//	device->GetDeviceContext()->Dispatch(32, 40, 1);


	//	ID3D11ShaderResourceView* nullSRVs[3] = { nullptr, nullptr, nullptr };
	//	ID3D11UnorderedAccessView* nullUAVs[2] = { nullptr, nullptr };
	//	device->GetDeviceContext()->CSSetShaderResources(0, 3, nullSRVs);
	//	device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 2, nullUAVs, NULL);


	//	device->GetDeviceContext()->CopyResource(device->GetBackBufferTexture(), graphicResourceHandler->GetBloomBufferTexture());


	//	gpuTimer->Stop(CREATE_ID_HASH("Bloom-GPU"));
	//	cpuTimer.Stop(CREATE_ID_HASH("Bloom-CPU"));
	//}
	//******* END Apply bloom post-processing ******/

	ProfileReturnConst(0);
}

int SE::Graphics::Renderer::BeginFrame()
{

	pipelineHandler->ClearAllRenderTargets();

	// clear the back buffer
	float clearColor[] = { 0, 0, 1, 1 };
	
	//if (!bloom)
	{
		ID3D11RenderTargetView* views[] = { device->GetRTV() };
		device->GetDeviceContext()->OMSetRenderTargets(1, views, device->GetDepthStencil());
	}
	//else
	//{
	//	float blackClearColor[] = { 0, 0, 0, 1 };

	//	ID3D11RenderTargetView* views[] = { device->GetRTV(), graphicResourceHandler->GetRenderTargetView(0) };
	//	device->GetDeviceContext()->OMSetRenderTargets(2, views, device->GetDepthStencil());

	//	// Clear the secondary render target view using the specified color
	//	device->GetDeviceContext()->ClearRenderTargetView(views[1], blackClearColor);
	//}

	// Clear the primary render target view using the specified color
	device->GetDeviceContext()->ClearRenderTargetView(device->GetRTV(), clearColor);

	// Clear the standard depth stencil view
	device->GetDeviceContext()->ClearDepthStencilView(device->GetDepthStencil(),D3D11_CLEAR_DEPTH,1.0f,	0);
	errorLog.clear();
	return 0;
}

int SE::Graphics::Renderer::EndFrame()
{
	StartProfile;
	device->Present();
	ProfileReturnConst( 0);
}

void SE::Graphics::Renderer::GetDeviceInfo(void * destination, size_t size)
{
	struct RetStruct
	{
		ID3D11Device* dev;
		ID3D11DeviceContext* devcon;
	};
	_ASSERT(size == sizeof(RetStruct));
	(static_cast<RetStruct*>(destination))->dev = device->GetDevice();
	(static_cast<RetStruct*>(destination))->devcon = device->GetDeviceContext();
}

int SE::Graphics::Renderer::CreateTexture(void* data, const TextureDesc& description)
{
	StartProfile;
	ProfileReturn(graphicResourceHandler->CreateShaderResourceView(data, description))
}

int SE::Graphics::Renderer::CreateTextFont(void * data, size_t size)
{
	fonts.push_back(DirectX::SpriteFont(device->GetDevice(), (uint8_t*)data, size));
	return fonts.size();
}

void SE::Graphics::Renderer::ResizeSwapChain(void* windowHandle)
{
	device->ResizeSwapChain((HWND)windowHandle);
}
//
//int SE::Graphics::Renderer::EnableBloom(int handleHorizontal, int handleVertical)
//{
//	bloomHorizontalHandle = handleHorizontal;
//	bloomVerticalHandle = handleVertical;
//
//	int texture2DHandles[3];
//
//
//	D3D11_TEXTURE2D_DESC texture2DDescription;
//	ZeroMemory(&texture2DDescription, sizeof(D3D11_TEXTURE2D_DESC));
//
//	texture2DDescription.Width = 1280;
//	texture2DDescription.Height = 720;
//	texture2DDescription.MipLevels = 1;
//	texture2DDescription.ArraySize = 1;
//	texture2DDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	texture2DDescription.SampleDesc.Count = 1;
//	texture2DDescription.SampleDesc.Quality = 0;
//	texture2DDescription.Usage = D3D11_USAGE_DEFAULT;
//	texture2DDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
//	texture2DDescription.CPUAccessFlags = 0;
//	texture2DDescription.MiscFlags = 0;
//
//	graphicResourceHandler->CreateTexture2D(texture2DDescription, texture2DHandles[0]);
//
//	texture2DDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
//
//	graphicResourceHandler->CreateTexture2D(texture2DDescription, texture2DHandles[1]);
//	graphicResourceHandler->CreateTexture2D(texture2DDescription, texture2DHandles[2], true);
//
//
//	graphicResourceHandler->CreateRenderTargetView(texture2DHandles[0]);
//
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription;
//	ZeroMemory(&shaderResourceViewDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
//
//	shaderResourceViewDescription.Format = texture2DDescription.Format;
//	shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	shaderResourceViewDescription.Texture2D.MostDetailedMip = 0;
//	shaderResourceViewDescription.Texture2D.MipLevels = 1;
//
//	graphicResourceHandler->CreateCustomShaderResourceView(shaderResourceViewDescription, 0, bloomShaderResourceViewHandles[0], device->GetBackBufferTexture());
//	graphicResourceHandler->CreateCustomShaderResourceView(shaderResourceViewDescription, texture2DHandles[0], bloomShaderResourceViewHandles[1]);
//	graphicResourceHandler->CreateCustomShaderResourceView(shaderResourceViewDescription, texture2DHandles[1], bloomShaderResourceViewHandles[2]);
//
//
//	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDescription;
//	ZeroMemory(&unorderedAccessViewDescription, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
//
//	unorderedAccessViewDescription.Format = shaderResourceViewDescription.Format;
//	unorderedAccessViewDescription.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
//	unorderedAccessViewDescription.Texture2D.MipSlice = 0;
//
//	graphicResourceHandler->CreateUnorderedAccessView(texture2DHandles[1], bloomUnorderedAccessViewHandles[0]);
//	graphicResourceHandler->CreateUnorderedAccessView(texture2DHandles[2], bloomUnorderedAccessViewHandles[1]);
//
//
//	bloom = true;
//	return 0;
//}
//
//int SE::Graphics::Renderer::DisableBloom()
//{
//	bloom = false;
//
//	return 0;
//}
bool SE::Graphics::Renderer::IsUnderLimit(size_t sizeToAdd)
{
	return memMeasure.GetVRam() + sizeToAdd <= initInfo.maxVRAMUsage;
}

bool SE::Graphics::Renderer::IsUnderLimit(size_t potential, size_t sizeToAdd)
{
	return memMeasure.GetVRam() + sizeToAdd <= initInfo.maxVRAMUsage + potential;
}

