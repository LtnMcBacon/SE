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
	pipelineHandler = nullptr;
	secPipelineHandler = nullptr;
	spriteBatch = nullptr;
	gpuTimer = nullptr;
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
	pipelineHandler->AddExisitingShaderResourceView("backbuffer", device->GetSRV());
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

size_t SE::Graphics::Renderer::EnableTextRendering(const TextJob& handles)
{
	StartProfile;
	size_t job;
	if (freeTextJobs.size())
	{
		job = freeTextJobs.top();
		freeTextJobs.pop();
	}
	else
	{
		job = textJobToIndex.size();
		textJobToIndex.push_back(0);
	}
	textJobToIndex[job] = renderTextJobs.size();


	D3D11_TEXTURE2D_DESC	gBB_Desc = device->GetTexDesc();
	long height = gBB_Desc.Height;
	long width = gBB_Desc.Width;


	DirectX::XMFLOAT2 dim;
	auto jw = (float)handles.info.width;
	auto jh = (float)handles.info.height;
	DirectX::XMStoreFloat2(&dim, fonts[handles.fontID].MeasureString(handles.info.text.c_str()));
	DirectX::XMFLOAT2 scale = handles.info.scale;
	if (jw == -1)
		jw = dim.x;
	else
		scale.x = (jw / dim.x)*handles.info.scale.x;
	if (jh == -1)
		jh = dim.y;
	else
		scale.y = (jh / dim.y)*handles.info.scale.y;
	RECT rect;
	rect.left = handles.info.posX;
	rect.top = handles.info.posY;
	rect.right = rect.left + jw;
	rect.bottom = rect.top + jh;

	DirectX::XMFLOAT2 origin = { handles.info.anchor.x*dim.x, handles.info.anchor.y*dim.y };

	// Scale to screen
	if (!handles.info.absolute)
	{
		rect.left = (rect.left / (float)handles.originalScreenWidth)* width;
		rect.top = (rect.top / (float)handles.originalScreenHeight)*height;
		rect.right = (rect.right / (float)handles.originalScreenWidth)* width;
		rect.bottom = (rect.bottom / (float)handles.originalScreenHeight)*height;
		//origin = { job.info.anchor.x*(job.info.width / (float)job.originalScreenWidth)* width, job.info.anchor.y*(job.info.height / (float)job.originalScreenHeight)*height };
	}

	rect.left += width * handles.info.screenAnchor.x;
	rect.top += height * handles.info.screenAnchor.y;
	rect.right += width * handles.info.screenAnchor.x;
	rect.bottom += height * handles.info.screenAnchor.y;



	renderTextJobs.push_back({ handles, rect, origin,scale, job });
	ProfileReturn(job);
}

size_t SE::Graphics::Renderer::DisableTextRendering(const size_t & jobID)
{
	_ASSERT(jobID < textJobToIndex.size());
	StartProfile;

	size_t current = textJobToIndex[jobID];
	size_t last = renderTextJobs.size() - 1;

	renderTextJobs[current] = renderTextJobs[last]; 
	textJobToIndex[renderTextJobs[current].index] = current;

	renderTextJobs.pop_back();

	ProfileReturnConst(0);
}

size_t SE::Graphics::Renderer::EnableTextureRendering(const GUIJob & handles)
{
	StartProfile;
	int job = (int)renderTextureJobs.size();
	renderTextureJobs.push_back(handles);
	//if (!renderTextureJobs[job].anchor)
	//{
		//D3D11_TEXTURE2D_DESC	gBB_Desc = device->GetTexDesc();
		//long height = gBB_Desc.Height;
		//long width = gBB_Desc.Width;
		//if (renderTextureJobs[job].rect)
		//{
		//	renderTextureJobs[job].rect->bottom = renderTextureJobs[job].rect->bottom * height;
		//	renderTextureJobs[job].rect->top = renderTextureJobs[job].rect->top * height;
		//	renderTextureJobs[job].rect->right = renderTextureJobs[job].rect->right * width;
		//	renderTextureJobs[job].rect->left = renderTextureJobs[job].rect->left * width;
		//}
		/*renderTextureJobs[job].info.width = renderTextureJobs[job].info.width * width;
		renderTextureJobs[job].info.height = renderTextureJobs[job].info.height * height;
		renderTextureJobs[job].info.posX = renderTextureJobs[job].info.posX * width;
		renderTextureJobs[job].info.posY = renderTextureJobs[job].info.posY * height;*/
//	}
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
	// ReSharper disable once CppDeclaratorNeverUsed
	auto devContext = device->GetDeviceContext();
	/******************General Jobs*********************/
	cpuTimer.Start(("RenderJob-CPU"));
	gpuTimer->Start(("RenderJob-GPU"));
	
	
	for (auto& group : jobGroups)
	{
		bool first = true;
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
			else if (j.job.ThreadGroupCountX != 0 || j.job.ThreadGroupCountY != 0 || j.job.ThreadGroupCountZ != 0)
			{
				devContext->Dispatch(j.job.ThreadGroupCountX, j.job.ThreadGroupCountY, j.job.ThreadGroupCountZ);
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
		ID3D11UnorderedAccessView* nullUAVS[8] = { nullptr };
		ID3D11Buffer* nullBuffers[8] = { nullptr };
		UINT32 offset = 0;
		device->GetDeviceContext()->SOSetTargets(0, nullptr,nullptr);
		device->GetDeviceContext()->OMSetRenderTargets(8, nullRTVS, nullptr);
		device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 8, nullUAVS, nullptr);
		device->GetDeviceContext()->PSSetShaderResources(0, 8, nullSRVS);
		device->GetDeviceContext()->CSSetShaderResources(0, 8, nullSRVS);

	}
	{
		ID3D11RenderTargetView* backbuf = device->GetRTV();
		ID3D11DepthStencilView* depthsv = device->GetDepthStencil();
		device->GetDeviceContext()->OMSetRenderTargets(1, &backbuf, depthsv);
	}
	gpuTimer->Stop(("RenderJob-GPU"));
	cpuTimer.Stop(("RenderJob-CPU"));
	/*****************End General Jobs******************/


	//********* Render sprite overlays ********/
	cpuTimer.Start(("GUIJob-CPU"));
	gpuTimer->Start(("GUIJob-GPU"));
	if (renderTextureJobs.size() || renderTextJobs.size())
	{
		D3D11_TEXTURE2D_DESC	gBB_Desc = device->GetTexDesc();
		long height = gBB_Desc.Height;
		long width = gBB_Desc.Width;

		spriteBatch->Begin(DirectX::SpriteSortMode_BackToFront, device->GetBlendState());
		for (auto& job : renderTextureJobs)
		{

			RECT rect;
			rect.left = job.info.posX;
			rect.top = job.info.posY;
			rect.right = rect.left + job.info.width*job.info.scale.x;
			rect.bottom = rect.top + job.info.height*job.info.scale.y;

			DirectX::XMFLOAT2 origin = { job.info.anchor.x*job.info.width, job.info.anchor.y*job.info.height };

			// Scale to screen
			if (!job.info.absolute)
			{
				rect.left = (rect.left / (float)job.originalScreenWidth)* width;
				rect.top = (rect.top / (float)job.originalScreenHeight)*height;
				rect.right = (rect.right / (float)job.originalScreenWidth)* width;
				rect.bottom = (rect.bottom / (float)job.originalScreenHeight)*height;
				//origin = { job.info.anchor.x*(job.info.width / (float)job.originalScreenWidth)* width, job.info.anchor.y*(job.info.height / (float)job.originalScreenHeight)*height };
			}

			rect.left += width * job.info.screenAnchor.x;
			rect.top += height * job.info.screenAnchor.y;
			rect.right += width * job.info.screenAnchor.x;
			rect.bottom += height * job.info.screenAnchor.y;
		
			spriteBatch->Draw(graphicResourceHandler->GetShaderResourceView(job.textureID), rect, nullptr, XMLoadFloat4(&job.info.colour), job.info.rotation, origin, (DirectX::SpriteEffects)job.info.effect, job.info.layerDepth);
			//spriteBatch->Draw(graphicResourceHandler->GetShaderResourceView(job.textureID), job.pos,(tagRECT*)job.rect, XMLoadFloat4(&job.colour), job.rotation, job.origin, job.scale, (DirectX::SpriteEffects)job.effect, job.layerDepth);
		}

		for (auto& job : renderTextJobs)
		{
		
			fonts[job.job.fontID].DrawString(spriteBatch, job.job.info.text.c_str(), { (float)job.rect.left,(float)job.rect.top }, XMLoadFloat4(&job.job.info.colour), job.job.info.rotation, { job.origin.x, job.origin.y }, job.scale, (DirectX::SpriteEffects)job.job.info.effect, job.job.info.layerDepth);
		}
		spriteBatch->End();
	}
	gpuTimer->Stop(("GUIJob-GPU"));
	cpuTimer.Stop(("GUIJob-CPU"));

	device->SetDepthStencilStateAndRS();
	device->SetBlendTransparencyState(0);


	////********* Apply bloom post-processing ********/
	//if (bloom)
	//{
	//	cpuTimer.Start(("Bloom-CPU"));
	//	gpuTimer->Start(("Bloom-GPU"));


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


	//	gpuTimer->Stop(("Bloom-GPU"));
	//	cpuTimer.Stop(("Bloom-CPU"));
	//}
	//******* END Apply bloom post-processing ******/

	ProfileReturnConst(0);
}

int SE::Graphics::Renderer::BeginFrame()
{

	pipelineHandler->ClearAllRenderTargets();

	// clear the back buffer
	float clearColor[] = { 0, 0, 0, 1 };
	
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
	return fonts.size() - 1;
}

void SE::Graphics::Renderer::ResizeSwapChain(void* windowHandle)
{
	device->ResizeSwapChain((HWND)windowHandle);
	pipelineHandler->AddExistingRenderTargetView("backbuffer", device->GetRTV());
	pipelineHandler->AddExisitingShaderResourceView("backbuffer", device->GetSRV());
	pipelineHandler->AddExistingDepthStencilView("backbuffer", device->GetDepthStencil());
	pipelineHandler->AddExisitingShaderResourceView("backbufferdepth", device->GetDepthStencilSRV());
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

