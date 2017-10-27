#include "Renderer.h"
#include <Profiler.h>
#include <ResourceHandler\IResourceHandler.h>
#include "GPUTimeCluster.h"
#include <Utilz\CPUTimeCluster.h>
#include "PipelineHandler.h"
#undef min

SE::Graphics::Renderer::Renderer() 
{
	oncePerFrameBufferID = -1;
	device = nullptr;
	graphicResourceHandler = nullptr;
	//animationSystem = nullptr;
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

	dev = device->GetDevice();
	devContext = device->GetDeviceContext();
	graphicResourceHandler = new GraphicResourceHandler(device->GetDevice(), device->GetDeviceContext());
	
	pipelineHandler = new PipelineHandler(device->GetDevice(), device->GetDeviceContext(),device->GetRTV(), device->GetDepthStencil());
	secPipelineHandler = new PipelineHandler(device->GetDevice(), device->GetSecondaryDeviceContext(), nullptr, nullptr);

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(device->GetDeviceContext());

	//animationSystem = new AnimationSystem();

	oncePerFrameBufferID = graphicResourceHandler->CreateConstantBuffer(sizeof(OncePerFrameConstantBuffer));
	if (oncePerFrameBufferID < 0)
	{
		throw std::exception("Could not create OncePerFrameConstantBuffer");
	}

	
	singleTransformConstantBuffer = graphicResourceHandler->CreateConstantBuffer(sizeof(DirectX::XMFLOAT4X4));
	if (singleTransformConstantBuffer < 0)
	{
		throw std::exception("Could not create singleTransformConstantBuffer.\n");
	}

	

	graphicResourceHandler->CreateSamplerState();

	lightBufferID = graphicResourceHandler->CreateConstantBuffer(sizeof(DirectX::XMFLOAT4) + sizeof(LightData) * lightBufferSize);
	if (lightBufferID < 0)
	{
		throw std::exception("Could not create LightDataBuffer");
	}

	gpuTimer = new GPUTimeCluster(device->GetDevice(), device->GetDeviceContext());

	running = true;
	//myThread = std::thread(&Renderer::Frame, this);

	materialBufferID = graphicResourceHandler->CreateConstantBuffer(sizeof(Graphics::MaterialAttributes));
	if (materialBufferID < 0)
	{
		throw std::exception("Could not create LightDataBuffer");
	}

	cameraBufferID = graphicResourceHandler->CreateConstantBuffer(sizeof(DirectX::XMFLOAT4));
	if (cameraBufferID < 0)
	{
		throw std::exception("Could not create CameraBuffer");
	}
	ProfileReturnConst( 0);
}

void SE::Graphics::Renderer::Shutdown()
{
	running = false;

//	if (myThread.joinable())
		//myThread.join();

	delete secPipelineHandler;
	delete pipelineHandler;
	graphicResourceHandler->Shutdown();
	device->Shutdown();

	delete gpuTimer;
	delete graphicResourceHandler;
//	delete animationSystem;
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

int SE::Graphics::Renderer::EnableRendering(const RenderObjectInfo & handles)
{
	StartProfile;
	uint32_t jobID;
	/*renderJobLock.lock();

	
	if (freeJobIndices.size())
	{
		jobID = freeJobIndices.top();
		freeJobIndices.pop();
	}
	else
	{
		jobID = static_cast<uint32_t>(jobIDToBucketAndTransformIndex.size());
		jobIDToBucketAndTransformIndex.push_back({ 0,0 });
	}
	renderJobLock.unlock();

	newJobs.push({ jobID, handles });

	ProfileReturnConst(jobID);*/



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
	const uint32_t transformIndex = static_cast<uint32_t>(renderBuckets[bucketIndex].transforms.size());
	DirectX::XMFLOAT4X4 identityMatrix;
	DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());
	renderBuckets[bucketIndex].transforms.push_back(identityMatrix);
	
	const uint32_t animationIndex = static_cast<uint32_t>(renderBuckets[bucketIndex].animationJob.size());
	renderBuckets[bucketIndex].animationJob.push_back(handles.animationJob);


//	uint32_t jobID;
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
	
	const BucketAndTransformIndex bucketAndTransformIndex = { static_cast<uint32_t>(bucketIndex), transformIndex, animationIndex};
	jobIDToBucketAndTransformIndex[jobID] = bucketAndTransformIndex;
	renderBuckets[bucketIndex].jobsInBucket.push_back(jobID);
//	renderJobLock.unlock();
	ProfileReturnConst(jobID);
}

int SE::Graphics::Renderer::DisableRendering(uint32_t jobID)
{
	StartProfile;
	//removeJobs.push(jobID);
	//ProfileReturnConst(0);


	//renderJobLock.lock();
	const uint32_t bucketIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].bucketIndex;
	const uint32_t transformIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].transformIndex;
	const uint32_t animationIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].animationIndex;

	auto& bucketOfRemoved = renderBuckets[bucketIndexOfRemoved];

	bucketOfRemoved.transforms[transformIndexOfRemoved] = bucketOfRemoved.transforms.back();
	bucketOfRemoved.transforms.pop_back();

	bucketOfRemoved.animationJob[animationIndexOfRemoved] = bucketOfRemoved.animationJob.back();
	bucketOfRemoved.animationJob.pop_back();

	const uint32_t jobThatReplacedOld = bucketOfRemoved.jobsInBucket.back();
	bucketOfRemoved.jobsInBucket[transformIndexOfRemoved] = jobThatReplacedOld;
	bucketOfRemoved.jobsInBucket.pop_back();

	jobIDToBucketAndTransformIndex[jobThatReplacedOld].transformIndex = transformIndexOfRemoved;
	jobIDToBucketAndTransformIndex[jobThatReplacedOld].animationIndex = animationIndexOfRemoved;
	jobIDToBucketAndTransformIndex[jobThatReplacedOld].bucketIndex = bucketIndexOfRemoved;

	freeJobIndices.push(jobID);

//	renderJobLock.unlock();
	ProfileReturnConst(0);
}



int SE::Graphics::Renderer::UpdateRenderingBuffer(uint32_t jobID, const RenderObjectInfo& handles)
{
	StartProfile;

//	updateJobs.push({ jobID, handles });

//	ProfileReturnConst(0);



	//renderJobLock.lock();
	const uint32_t bucketIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].bucketIndex;
	const uint32_t transformIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].transformIndex;
	const uint32_t animationIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].animationIndex;

	auto& bucketOfRemoved = renderBuckets[bucketIndexOfRemoved];

	DirectX::XMFLOAT4X4 transform = bucketOfRemoved.transforms[transformIndexOfRemoved];
	bucketOfRemoved.transforms[transformIndexOfRemoved] = bucketOfRemoved.transforms.back();
	bucketOfRemoved.transforms.pop_back();

	bucketOfRemoved.animationJob[animationIndexOfRemoved] = bucketOfRemoved.animationJob.back();
	bucketOfRemoved.animationJob.pop_back();

	const uint32_t jobThatReplacedOld = bucketOfRemoved.jobsInBucket.back();
	bucketOfRemoved.jobsInBucket[transformIndexOfRemoved] = jobThatReplacedOld;
	bucketOfRemoved.jobsInBucket.pop_back();

	jobIDToBucketAndTransformIndex[jobThatReplacedOld].transformIndex = transformIndexOfRemoved;
	jobIDToBucketAndTransformIndex[jobThatReplacedOld].animationIndex = animationIndexOfRemoved;
	jobIDToBucketAndTransformIndex[jobThatReplacedOld].bucketIndex = bucketIndexOfRemoved;

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
	if (bucketIndex < 0)
	{
		bucketIndex = renderBuckets.size();
		TargetOffset t = { { true, false, false },{ 2, 0, 0 } };
		renderBuckets.push_back({ handles,{},{} });
	}
	const size_t transformIndex = renderBuckets[bucketIndex].transforms.size();
	renderBuckets[bucketIndex].transforms.push_back(transform);


	const uint32_t animationIndex = static_cast<uint32_t>(renderBuckets[bucketIndex].animationJob.size());
	renderBuckets[bucketIndex].animationJob.push_back(handles.animationJob);


	const BucketAndTransformIndex bucketAndTransformIndex = { bucketIndex, transformIndex, animationIndex };
	jobIDToBucketAndTransformIndex[jobID] = bucketAndTransformIndex;
	renderBuckets[bucketIndex].jobsInBucket.push_back(jobID);

//	renderJobLock.unlock();
	ProfileReturnConst( 0);
}


int SE::Graphics::Renderer::AddLineRenderJob(const LineRenderJob& lineJob)
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

int SE::Graphics::Renderer::RemoveLineRenderJob(uint32_t lineJobID)
{
	StartProfile;
	freeLineRenderJobIndices.push(lineJobID);
	lineRenderJobs[lineJobID].verticesToDrawCount = 0;
	ProfileReturnConst(0);
}

int SE::Graphics::Renderer::UpdateLineRenderJobTransform(uint32_t lineJobID, float* transform)
{
	StartProfile;
	DirectX::XMMATRIX trans = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)transform);
	DirectX::XMFLOAT4X4 transposed;
	DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(trans));
	lineRenderJobs[lineJobID].transform = transposed;
	ProfileReturnConst(0);
}

int SE::Graphics::Renderer::UpdateLineRenderJobRange(uint32_t lineJobID, uint32_t startVertex, uint32_t vertexCount)
{
	StartProfile;

	lineRenderJobs[lineJobID].firstVertex = startVertex;
	lineRenderJobs[lineJobID].verticesToDrawCount = vertexCount;

	ProfileReturnConst(0);
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
		if (renderTextureJobs[job].rect != nullptr)
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

int SE::Graphics::Renderer::EnableLightRendering(const LightData & handles)
{
	StartProfile;
	//lightLock.lock();
	int job = (int)renderLightJobs.size();
	renderLightJobs.push_back(handles);
	//lightLock.unlock();
	
	ProfileReturn(job);
}

int SE::Graphics::Renderer::DisableLightRendering(size_t ID)
{
	StartProfile;
	//lightLock.lock();
	size_t job = renderLightJobs.size() - 1;
	renderLightJobs[ID] = renderLightJobs[job];
	renderLightJobs.pop_back();
	//lightLock.unlock();
	ProfileReturn(int(job));
}

int SE::Graphics::Renderer::UpdateLightPos(const DirectX::XMFLOAT3& pos, size_t ID)
{
	renderLightJobs[ID].pos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, renderLightJobs[ID].pos.w);
	return 0;
}

int SE::Graphics::Renderer::UpdateView(float * viewMatrix, const DirectX::XMFLOAT4& cameraPos)
{
	DirectX::XMStoreFloat4x4(&newViewProjTransposed, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)viewMatrix)));
	newCameraPos = cameraPos;

	return 0;
}

int SE::Graphics::Renderer::Render() {
	StartProfile;

	//currentEntityTimePos += 1;

	//animationSystem->UpdateAnimation(0, 0, currentEntityTimePos);

	//If threading is enabled.
	//AddNewRenderJobs();
	//UpdateRenderJobs();
	//UpdateTransforms();
	//RemoveRenderJobs();

	// clear the back buffer
	//float clearColor[] = { 0, 0, 1, 1 };



	//Update the pointlights in the scene and bind them to the pixel shader stage.
	//lightLock.lock();
	const size_t lightMappingSize = sizeof(DirectX::XMFLOAT4) + sizeof(LightData) * renderLightJobs.size();
	LightDataBuffer lightBufferData;
	graphicResourceHandler->UpdateConstantBuffer<LightDataBuffer>(lightBufferID, [this](LightDataBuffer* data) {
		data->size.x = renderLightJobs.size();
		memcpy(data->data, renderLightJobs.data(), +sizeof(LightData) * renderLightJobs.size());
	});
	//lightLock.unlock();
	graphicResourceHandler->BindConstantBuffer(GraphicResourceHandler::ShaderStage::PIXEL, lightBufferID, 2);
	// SetLightBuffer end
	
	DirectX::XMFLOAT4 cameraBuffer;
	graphicResourceHandler->UpdateConstantBuffer<DirectX::XMFLOAT4>(cameraBufferID, [this](DirectX::XMFLOAT4* data) {
		memcpy(data, &newCameraPos, sizeof(DirectX::XMFLOAT4));
	});
	graphicResourceHandler->BindConstantBuffer(GraphicResourceHandler::ShaderStage::PIXEL, cameraBufferID, 3);


	cpuTimer.Start(CREATE_ID_HASH("Rendering-CPU"));
	gpuTimer->Start(CREATE_ID_HASH("Rendering-GPU"));
	//The previousJob is necessary to see what state changes need to be performed when rendering
	//the next bucket.
	
	RenderObjectInfo previousJob;
	previousJob.textureCount = 0;
	for (int i = 0; i < RenderObjectInfo::maxTextureBinds; ++i)
	{
		previousJob.textureHandles[i] = -1;
		previousJob.textureBindings[i] = -1;
	}
	previousJob.bufferHandle = -1;
	previousJob.pixelShader = -1;
	previousJob.topology = RenderObjectInfo::PrimitiveTopology::NONE;
	previousJob.vertexShader = -1;
	previousJob.skeletonIndex = -1;
	previousJob.fillSolid = 1;
	previousJob.transparency = 0;
	previousJob.matIndex = -1;

	device->SetBlendTransparencyState(0);
	graphicResourceHandler->UpdateConstantBuffer(&newViewProjTransposed, sizeof(newViewProjTransposed), oncePerFrameBufferID);

	//First render all opaque geometry, then render partially transparent geometry.
	std::vector<size_t> transparentIndices;
	for(auto iteration = 0; iteration < renderBuckets.size(); iteration++)
	{
		if (renderBuckets[iteration].stateInfo.transparency == 0)
		{
			RenderABucket(renderBuckets[iteration], previousJob);
			previousJob = renderBuckets[iteration].stateInfo;
		}
		else
			transparentIndices.push_back(iteration);
	}
	for (auto iteration = 0; iteration < transparentIndices.size(); iteration++)
	{
		RenderABucket(renderBuckets[transparentIndices[iteration]], previousJob);
		previousJob = renderBuckets[transparentIndices[iteration]].stateInfo;
	}
	gpuTimer->Stop(CREATE_ID_HASH("Rendering-GPU"));
	cpuTimer.Stop(CREATE_ID_HASH("Rendering-CPU"));

	/////********** Render line jobs (primarily for debugging) ************/

	//gpuTimer->Start("LineJob-GPU");
	//cpuTimer.Start("LineJob-CPU");
	//device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//graphicResourceHandler->BindVSConstantBuffer(oncePerFrameBufferID, 1);
	//graphicResourceHandler->BindVSConstantBuffer(singleTransformConstantBuffer, 2);
	//for(auto& lineJob : lineRenderJobs)
	//{
	//	if (lineJob.verticesToDrawCount == 0)
	//		continue;
	//	graphicResourceHandler->UpdateConstantBuffer(&lineJob.transform, sizeof(lineJob.transform), singleTransformConstantBuffer);
	//	graphicResourceHandler->SetMaterial(lineJob.vertexShaderHandle, lineJob.pixelShaderHandle);
	//	graphicResourceHandler->SetVertexBuffer(lineJob.vertexBufferHandle);
	//	device->GetDeviceContext()->Draw(lineJob.verticesToDrawCount, lineJob.firstVertex);
	//}
	//cpuTimer.Stop("LineJob-CPU");
	//gpuTimer->Stop("LineJob-GPU");

	///********END render line jobs************/

	/******************General Jobs*********************/
	cpuTimer.Start(CREATE_ID_HASH("RenderJob-CPU"));
	gpuTimer->Start(CREATE_ID_HASH("RenderJob-GPU"));
	
	bool first = true;
	for (auto& group : jobGroups)
	{
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
				devContext->Draw(j.job.vertexCount, j.job.vertexOffset);
			}
			else if (j.job.indexCount != 0 && j.job.instanceCount == 0)
			{
				for (auto& mf : j.job.mappingFunc)
					mf(drawn, 1);
				devContext->DrawIndexed(j.job.indexCount, j.job.indexOffset, j.job.vertexOffset);
			}
			else if (j.job.indexCount == 0 && j.job.instanceCount != 0)
			{
				while (drawn < j.job.instanceCount)
				{
				
					const uint32_t toDraw = std::min(j.job.maxInstances, j.job.instanceCount - drawn);
					for (auto& mf : j.job.mappingFunc)
						mf(drawn, toDraw);
					devContext->DrawInstanced(j.job.vertexCount, toDraw, j.job.vertexOffset, j.job.instanceOffset);
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
					devContext->DrawIndexedInstanced(j.job.indexCount, toDraw, j.job.indexOffset, j.job.vertexOffset, j.job.instanceOffset);
					drawn += toDraw;
				}
			}
			else if (j.job.vertexCount == 0)
			{
				for (auto& mf : j.job.mappingFunc)
					mf(drawn, 0);
				devContext->DrawAuto();
			}
		}
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
			spriteBatch->Draw(graphicResourceHandler->GetShaderResourceView(job.textureID), job.pos, job.rect, XMLoadFloat4(&job.colour), job.rotation, job.origin, job.scale, (DirectX::SpriteEffects)job.effect, job.layerDepth);
		}

		for (auto& job : renderTextJobs)
		{
			fonts[job.fontID].DrawString(spriteBatch.get(), job.text.c_str(), job.pos, XMLoadFloat4(&job.colour), job.rotation, job.origin, job.scale, (DirectX::SpriteEffects)job.effect, job.layerDepth);
		}
		spriteBatch->End();
	}
	gpuTimer->Stop(CREATE_ID_HASH("GUIJob-GPU"));
	cpuTimer.Stop(CREATE_ID_HASH("GUIJob-CPU"));

	device->SetDepthStencilStateAndRS();
	device->SetBlendTransparencyState(0);


	//********* Apply bloom post-processing ********/
	if (bloom)
	{
		cpuTimer.Start(CREATE_ID_HASH("Bloom-CPU"));
		gpuTimer->Start(CREATE_ID_HASH("Bloom-GPU"));


		ID3D11ShaderResourceView* shaderResourceViews[] = {
			graphicResourceHandler->GetShaderResourceView(bloomShaderResourceViewHandles[0]),
			graphicResourceHandler->GetShaderResourceView(bloomShaderResourceViewHandles[1]),
			graphicResourceHandler->GetShaderResourceView(bloomShaderResourceViewHandles[2])
		};
		ID3D11UnorderedAccessView* unorderedAccessViews[] = {
			graphicResourceHandler->GetUnorderedAccessView(bloomUnorderedAccessViewHandles[0]),
			graphicResourceHandler->GetUnorderedAccessView(bloomUnorderedAccessViewHandles[1])
		};


		ID3D11RenderTargetView* nullRTVs[] = { nullptr, nullptr };
		device->GetDeviceContext()->OMSetRenderTargets(2, nullRTVs, NULL);

		graphicResourceHandler->SetCompute(bloomHorizontalHandle);
		device->GetDeviceContext()->CSSetShaderResources(0, 1, &shaderResourceViews[1]);
		device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &unorderedAccessViews[0], NULL);
		device->GetDeviceContext()->Dispatch(32, 45, 1);

		graphicResourceHandler->SetCompute(bloomVerticalHandle);
		device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &unorderedAccessViews[1], NULL);
		device->GetDeviceContext()->CSSetShaderResources(0, 1, &shaderResourceViews[2]);
		device->GetDeviceContext()->CSSetShaderResources(1, 1, &shaderResourceViews[0]);
		device->GetDeviceContext()->Dispatch(32, 40, 1);


		ID3D11ShaderResourceView* nullSRVs[3] = { nullptr, nullptr, nullptr };
		ID3D11UnorderedAccessView* nullUAVs[2] = { nullptr, nullptr };
		device->GetDeviceContext()->CSSetShaderResources(0, 3, nullSRVs);
		device->GetDeviceContext()->CSSetUnorderedAccessViews(0, 2, nullUAVs, NULL);


		device->GetDeviceContext()->CopyResource(device->GetBackBufferTexture(), graphicResourceHandler->GetBloomBufferTexture());


		gpuTimer->Stop(CREATE_ID_HASH("Bloom-GPU"));
		cpuTimer.Stop(CREATE_ID_HASH("Bloom-CPU"));
	}
	//******* END Apply bloom post-processing ******/

	ID3D11RenderTargetView* views[] = { device->GetRTV() };
	device->GetDeviceContext()->OMSetRenderTargets(1, views, device->GetDepthStencil());

	ProfileReturnConst(0);
}

int SE::Graphics::Renderer::BeginFrame()
{

	pipelineHandler->ClearAllRenderTargets();

	// clear the back buffer
	float clearColor[] = { 0, 0, 1, 1 };
	
	if (!bloom)
	{
		ID3D11RenderTargetView* views[] = { device->GetRTV() };
		device->GetDeviceContext()->OMSetRenderTargets(1, views, device->GetDepthStencil());
	}
	else
	{
		float blackClearColor[] = { 0, 0, 0, 1 };

		ID3D11RenderTargetView* views[] = { device->GetRTV(), graphicResourceHandler->GetRenderTargetView(0) };
		device->GetDeviceContext()->OMSetRenderTargets(2, views, device->GetDepthStencil());

		// Clear the secondary render target view using the specified color
		device->GetDeviceContext()->ClearRenderTargetView(views[1], blackClearColor);
	}

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

int SE::Graphics::Renderer::CreateTexture(void* data, const TextureDesc& description)
{
	StartProfile;
	ProfileReturn(graphicResourceHandler->CreateShaderResourceView(data, description))
}

int SE::Graphics::Renderer::UpdateTransform(uint32_t jobID, float* transform)
{
	StartProfile;
	//UpdateTransformStruct ut;
	//ut.jobID = jobID;
	//DirectX::XMMATRIX t = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)transform);
	//DirectX::XMStoreFloat4x4(&ut.transform, DirectX::XMMatrixTranspose(t));
	//updateTransforms.push(ut);
	//ProfileReturnConst(0);

//	renderJobLock.lock();
	DirectX::XMMATRIX trans = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)transform);
	DirectX::XMFLOAT4X4 transposed;
	DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(trans));
	
	const size_t bucketIndex = jobIDToBucketAndTransformIndex[jobID].bucketIndex;
	const size_t transformIndex = jobIDToBucketAndTransformIndex[jobID].transformIndex;

 	renderBuckets[bucketIndex].transforms[transformIndex] = transposed;
	//renderJobLock.unlock();
	ProfileReturnConst(0);
}
//
//int SE::Graphics::Renderer::UpdateBoneTransform(uint32_t jobID, float* transforms, size_t nrOfJoints) {
//
//	StartProfile;
//
//	auto& indices = jobIDToBucketAndTransformIndex[jobID];
//
//	auto& bucket = renderBuckets[indices.bucketIndex];
//
//	bucket.gBoneTransforms.resize(nrOfJoints);
//
//	memcpy(&bucket.gBoneTransforms[indices.boneIndex], transforms, nrOfJoints * sizeof(DirectX::XMFLOAT4X4));
//
//	ProfileReturnConst(0);
//}

int SE::Graphics::Renderer::CreatePixelShader(void* data, size_t size, ShaderSettings* reflection)
{
	int handle;
	auto hr = graphicResourceHandler->CreatePixelShader(device->GetDevice(),data, size, &handle, reflection );
	if (FAILED(hr))
		return hr;

	return handle;
}

int SE::Graphics::Renderer::CreateDynamicVertexBuffer(size_t bytewidth, size_t vertexByteSize, void* initialData, size_t initialDataSize)
{
	StartProfile;
	ProfileReturn(graphicResourceHandler->CreateDynamicVertexBuffer(bytewidth, vertexByteSize, initialData, initialDataSize));
}

int SE::Graphics::Renderer::UpdateDynamicVertexBuffer(int handle, void* data, size_t totalSize, size_t sizePerElement)
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

void SE::Graphics::Renderer::AddNewRenderJobs()
{
	StartProfile;
	while (!newJobs.wasEmpty())
	{
		auto& job = newJobs.top();

		int32_t bucketIndex = -1;
		const size_t renderBucketCount = renderBuckets.size();
		for (size_t i = 0; i < renderBucketCount; ++i)
		{
			if (renderBuckets[i].stateInfo - job.handles == 0)
			{
				bucketIndex = i;
				break;
			}
		}
		if (bucketIndex < 0)
		{
			bucketIndex = renderBuckets.size();
			TargetOffset t = { { true, false, false },{ 2, 0, 0 } };
			renderBuckets.push_back({ job.handles,{},{} });
		}
		const size_t transformIndex = renderBuckets[bucketIndex].transforms.size();
		DirectX::XMFLOAT4X4 identityMatrix;
		DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());
		renderBuckets[bucketIndex].transforms.push_back(identityMatrix);


		const BucketAndTransformIndex bucketAndTransformIndex = { bucketIndex, transformIndex };
		renderJobLock.lock();
		jobIDToBucketAndTransformIndex[job.jobID] = bucketAndTransformIndex;
		renderJobLock.unlock();
		renderBuckets[bucketIndex].jobsInBucket.push_back(job.jobID);
		
		newJobs.pop();
	}
	StopProfile;
}

void SE::Graphics::Renderer::UpdateRenderJobs()
{
	StartProfile;
	while (!updateJobs.wasEmpty())
	{
		auto& job = updateJobs.top();
		

		renderJobLock.lock();
		const uint32_t bucketIndexOfRemoved = jobIDToBucketAndTransformIndex[job.jobID].bucketIndex;
		const uint32_t transformIndexOfRemoved = jobIDToBucketAndTransformIndex[job.jobID].transformIndex;
		renderJobLock.unlock();

		auto& bucketOfRemoved = renderBuckets[bucketIndexOfRemoved];

		DirectX::XMFLOAT4X4 transform = bucketOfRemoved.transforms[transformIndexOfRemoved];
		bucketOfRemoved.transforms[transformIndexOfRemoved] = bucketOfRemoved.transforms.back();
		bucketOfRemoved.transforms.pop_back();
		const uint32_t jobThatReplacedOld = bucketOfRemoved.jobsInBucket.back();
		bucketOfRemoved.jobsInBucket[transformIndexOfRemoved] = jobThatReplacedOld;
		bucketOfRemoved.jobsInBucket.pop_back();

		renderJobLock.lock();
		jobIDToBucketAndTransformIndex[jobThatReplacedOld].transformIndex = transformIndexOfRemoved;
		jobIDToBucketAndTransformIndex[jobThatReplacedOld].bucketIndex = bucketIndexOfRemoved;
		renderJobLock.unlock();

		int32_t bucketIndex = -1;
		const size_t renderBucketCount = renderBuckets.size();
		for (size_t i = 0; i < renderBucketCount; ++i)
		{
			if (renderBuckets[i].stateInfo - job.handles == 0)
			{
				bucketIndex = i;
				break;
			}
		}
		if (bucketIndex < 0)
		{
			bucketIndex = renderBuckets.size();
			TargetOffset t = { { true, false, false },{ 2, 0, 0 } };
			renderBuckets.push_back({ job.handles,{},{} });
		}
		const size_t transformIndex = renderBuckets[bucketIndex].transforms.size();
		renderBuckets[bucketIndex].transforms.push_back(transform);


		const BucketAndTransformIndex bucketAndTransformIndex = { bucketIndex, transformIndex };
		renderJobLock.lock();
		jobIDToBucketAndTransformIndex[job.jobID] = bucketAndTransformIndex;
		renderJobLock.unlock();
		renderBuckets[bucketIndex].jobsInBucket.push_back(job.jobID);

		updateJobs.pop();

	}
	StopProfile;
}

void SE::Graphics::Renderer::RemoveRenderJobs()
{
	StartProfile;
	while (!removeJobs.wasEmpty())
	{
		auto& jobID = removeJobs.top();


		renderJobLock.lock();
		if (jobID >= jobIDToBucketAndTransformIndex.size())
			continue;

		const uint32_t bucketIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].bucketIndex;
		const uint32_t transformIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].transformIndex;
		renderJobLock.unlock();

		auto& bucketOfRemoved = renderBuckets[bucketIndexOfRemoved];

		bucketOfRemoved.transforms[transformIndexOfRemoved] = bucketOfRemoved.transforms.back();
		bucketOfRemoved.transforms.pop_back();
		const uint32_t jobThatReplacedOld = bucketOfRemoved.jobsInBucket.back();
		bucketOfRemoved.jobsInBucket[transformIndexOfRemoved] = jobThatReplacedOld;
		bucketOfRemoved.jobsInBucket.pop_back();

		renderJobLock.lock();
		jobIDToBucketAndTransformIndex[jobThatReplacedOld].transformIndex = transformIndexOfRemoved;
		jobIDToBucketAndTransformIndex[jobThatReplacedOld].bucketIndex = bucketIndexOfRemoved;
		renderJobLock.unlock();

		freeJobIndices.push(jobID);

		removeJobs.pop();
	}
	StopProfile;
}

void SE::Graphics::Renderer::UpdateTransforms()
{
	StartProfile;
	while (!updateTransforms.wasEmpty())
	{
		auto& job = updateTransforms.top();

		renderJobLock.lock();
		const size_t bucketIndex = jobIDToBucketAndTransformIndex[job.jobID].bucketIndex;
		const size_t transformIndex = jobIDToBucketAndTransformIndex[job.jobID].transformIndex;
		renderJobLock.unlock();

		renderBuckets[bucketIndex].transforms[transformIndex] = job.transform;


		updateTransforms.pop();
	}
	StopProfile;
}

void SE::Graphics::Renderer::RenderABucket(const RenderBucket& bucket, const RenderObjectInfo& previousJob)
{
	StartProfile;
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
		case RenderObjectInfo::PrimitiveTopology::NONE:
		{
			errorLog.push_back("A bucket with primitive topology NONE was present in the renderers job queue.");
			break;
		}
		}
	}
	if (previousJob.matIndex != job.matIndex)
	{
		Graphics::MaterialAttributes matDataBuffer;
		graphicResourceHandler->UpdateConstantBuffer<Graphics::MaterialAttributes>(materialBufferID, [&job](Graphics::MaterialAttributes* data) {
			memcpy(data, &job.material, sizeof(Graphics::MaterialAttributes));
		});
		graphicResourceHandler->BindConstantBuffer(GraphicResourceHandler::ShaderStage::PIXEL, materialBufferID, 4);
	}
	if (previousJob.fillSolid != job.fillSolid)
	{
		device->SetRasterStateFill(job.fillSolid);
	}
	if (previousJob.transparency != job.transparency)
	{
		device->SetBlendTransparencyState(job.transparency);
	}
	if (previousJob.pixelShader != job.pixelShader || previousJob.vertexShader != job.vertexShader)
		graphicResourceHandler->SetMaterial(job.vertexShader, job.pixelShader);
	if (previousJob.bufferHandle != job.bufferHandle)
		graphicResourceHandler->SetVertexBuffer(job.bufferHandle);
	for (int i = 0; i < job.textureCount; ++i)
		if (previousJob.textureHandles[i] != job.textureHandles[i] || previousJob.textureBindings[i] != job.textureBindings[i])
			graphicResourceHandler->BindShaderResourceView(job.textureHandles[i], job.textureBindings[i]);

	int bindSlot;
	auto viewProjHandle = graphicResourceHandler->GetVSConstantBufferByName(bucket.stateInfo.vertexShader, "OncePerFrame", &bindSlot);
	if (viewProjHandle < 0)
	{
		errorLog.push_back("Constant buffer: 'OncePerFrame' could not be found. Aborting job.\n");
		return;
	}
	graphicResourceHandler->BindVSConstantBuffer(oncePerFrameBufferID, bindSlot);

	auto oncePerObject = graphicResourceHandler->GetVSConstantBufferByName(bucket.stateInfo.vertexShader, "OncePerObject", &bindSlot);
	if (oncePerObject < 0)
	{
		errorLog.push_back("Constant buffer: 'OncePerObject' could not be found. Aborting job.\n");
		return;
	}
	graphicResourceHandler->BindVSConstantBuffer(oncePerObject, bindSlot);

	std::vector<DirectX::XMFLOAT4X4>identity;
	identity.resize(4);

	for (int i = 0; i < 4; i++) {

		DirectX::XMStoreFloat4x4(&identity[i], DirectX::XMMatrixIdentity());
	}

	if (job.type == RenderObjectInfo::JobType::STATIC) {

		const size_t instanceCount = bucket.transforms.size();

		/*int binsSlotInvers;
		int InversBufferHandle = graphicResourceHandler->GetVSConstantBufferByName(bucket.stateInfo.vertexShader, "InversWorld", &binsSlotInvers);
		graphicResourceHandler->BindVSConstantBuffer(InversBufferHandle, binsSlotInvers);*/

		/*std::vector<DirectX::XMFLOAT4X4> inversVec;
		for (int i = 0; i < bucket.transforms.size(); i++)
		{
			DirectX::XMMATRIX invers = DirectX::XMLoadFloat4x4(&bucket.transforms[i]);
			invers = DirectX::XMMatrixInverse(nullptr, invers);
			DirectX::XMFLOAT4X4 fInvers;
			DirectX::XMStoreFloat4x4(&fInvers, invers);
			inversVec.push_back(fInvers);
		}*/
		for (int i = 0; i < instanceCount; i += maxDrawInstances)
		{
			const size_t instancesToDraw = std::min(bucket.transforms.size() - i, (size_t)maxDrawInstances);
			const size_t mapSize = sizeof(DirectX::XMFLOAT4X4) * instancesToDraw;
			graphicResourceHandler->UpdateConstantBuffer(&bucket.transforms[i], mapSize, oncePerObject);
		//	graphicResourceHandler->UpdateConstantBuffer(&inversVec[i], mapSize, InversBufferHandle);
			device->GetDeviceContext()->DrawInstanced(graphicResourceHandler->GetVertexCount(bucket.stateInfo.bufferHandle), instancesToDraw, 0, 0);
		}

	}

	else if (job.type == RenderObjectInfo::JobType::SKINNED) {
	//	int boneBindslot;
	//	const int cBoneBufferIndex = graphicResourceHandler->GetVSConstantBufferByName(bucket.stateInfo.vertexShader, "VS_SKINNED_DATA", &boneBindslot);
	//	graphicResourceHandler->BindVSConstantBuffer(cBoneBufferIndex, boneBindslot);

	//	







	//	std::vector<DirectX::XMFLOAT4X4> inversVec;
	//	for (int i = 0; i < bucket.transforms.size(); i++)
	//	{
	//		DirectX::XMMATRIX invers = DirectX::XMLoadFloat4x4(&bucket.transforms[i]);
	//		invers = DirectX::XMMatrixInverse(nullptr, invers);
	//		DirectX::XMFLOAT4X4 fInvers;
	//		DirectX::XMStoreFloat4x4(&fInvers, invers);
	//		inversVec.push_back(fInvers);
	//	}

	///*	bucket.gBoneTransforms = animationSystem->GetSkeleton(0).jointArray;
	//	graphicResourceHandler->UpdateConstantBuffer(&bucket.gBoneTransforms[0], sizeof(DirectX::XMFLOAT4X4) * 4, cBoneBufferIndex);*/

	//


	//	// TODO: Change the hlsl code to fit.
	//	// TODO: Should be updated with the instanceCount and maxDrawInstances
	//	struct XMS
	//	{
	//		DirectX::XMFLOAT4X4 s[30];
	//	};

	//	//graphicResourceHandler->UpdateConstantBuffer<XMS>(cBoneBufferIndex, [this, &bucket, &job](auto data) {
	//	//	for (uint32_t i = 0; i < bucket.animationJob.size(); i++) // We need to update each animation
	//	//	{
	//	//		auto animationJobIndex = bucket.animationJob[i];
	//	//		if (animationJobIndex != -1) // If the renderjob has an animation job bound to it.
	//	//		{
	//	//			auto& ajob = jobIDToAnimationJob[animationJobIndex]; // Get the animation job from the renderjob in the bucket
	//	//			if (ajob.animating) // If the animation is playing
	//	//				ajob.timePos += ajob.speed; // TODO: Delta time.

	//	//			animationSystem->UpdateAnimation(ajob.animationHandle, job.skeletonIndex, ajob.timePos, &(data + i)->s[0]); // TODO: Make it so we don't have to recalculate the matricies if the animation hasn't changed.
	//	//		}
	//	//		else // This is a skinned mesh, however it does not have any animation. So just pass identity matricies.
	//	//		{
	//	//			// TODO:
	//	//		}
	//	//	}
	//	//});


	//	//bucket.gBoneTransforms = animationSystem->GetSkeleton(0).jointArray;
	////	graphicResourceHandler->UpdateConstantBuffer(&bucket.gBoneTransforms[0], sizeof(DirectX::XMFLOAT4X4) * 4, cBoneBufferIndex);

	//	const size_t instanceCount = bucket.transforms.size();
	//	for (int i = 0; i < instanceCount; i += 8)
	//	{
	//		const size_t instancesToDraw = std::min(bucket.transforms.size() - i, (size_t)8);
	//		const size_t mapSize = sizeof(DirectX::XMFLOAT4X4) * instancesToDraw;
	//		graphicResourceHandler->UpdateConstantBuffer(&bucket.transforms[i], mapSize, oncePerObject);
	//		device->GetDeviceContext()->DrawInstanced(graphicResourceHandler->GetVertexCount(bucket.stateInfo.bufferHandle), instancesToDraw, 0, 0);
	//	}
	}
}

void SE::Graphics::Renderer::Frame()
{
	while (running)
	{
		Render();
	}

}

int SE::Graphics::Renderer::CreateComputeShader(void * data, size_t size)
{
	int handle;
	auto hr = graphicResourceHandler->CreateComputeShader(device->GetDevice(), data, size, &handle);
	if (FAILED(hr))
		return hr;

	return handle;
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
//int SE::Graphics::Renderer::CreateSkeleton(JointAttributes* jointData, size_t nrOfJoints) {
//
//	int handle;
//	auto hr = animationSystem->AddSkeleton(jointData, nrOfJoints, &handle);
//	if (hr)
//		return hr;
//	return handle;
//}
//
//int SE::Graphics::Renderer::CreateAnimation(DirectX::XMFLOAT4X4* matrices, size_t nrOfKeyframes, size_t nrOfJoints) {
//
//	int handle;
//	auto hr = animationSystem->AddAnimation(matrices, nrOfKeyframes, nrOfJoints, &handle);
//	if (hr)
//		return hr;
//	return handle;
//}
//
//int SE::Graphics::Renderer::StartAnimation(const AnimationJobInfo & info)
//{
//	int job = -1;
//	if (freeAnimationJobIndicies.size())
//	{
//		job = freeAnimationJobIndicies.top();
//		freeAnimationJobIndicies.pop();
//	}
//	else
//	{
//		job = static_cast<int>(jobIDToAnimationJob.size());
//		jobIDToAnimationJob.push_back(info);
//	}
//	
//	return job;
//}
//
//void SE::Graphics::Renderer::StopAnimation(int job)
//{
//	_ASSERT_EXPR(job < static_cast<int>(jobIDToAnimationJob.size()), "AnimationJob out of range");
//	freeAnimationJobIndicies.push(job);
//}
//
//void SE::Graphics::Renderer::UpdateAnimation(int job, const AnimationJobInfo & info)
//{
//	_ASSERT_EXPR(job < static_cast<int>(jobIDToAnimationJob.size()), "AnimationJob out of range");
//	jobIDToAnimationJob[static_cast<size_t>(job)] = info;
//}
//
//void SE::Graphics::Renderer::SetAnimationSpeed(int job, float speed)
//{
//	_ASSERT_EXPR(job < static_cast<int>(jobIDToAnimationJob.size()), "AnimationJob out of range");
//	jobIDToAnimationJob[static_cast<size_t>(job)].speed = speed;
//}
//
//void SE::Graphics::Renderer::SetKeyFrame(int job, float keyframe)
//{
//	_ASSERT_EXPR(job < static_cast<int>(jobIDToAnimationJob.size()), "AnimationJob out of range");
//	jobIDToAnimationJob[static_cast<size_t>(job)].timePos = keyframe;
//	jobIDToAnimationJob[static_cast<size_t>(job)].animating = false;
//}
//
//void SE::Graphics::Renderer::StartAnimation(int job)
//{
//	_ASSERT_EXPR(job < static_cast<int>(jobIDToAnimationJob.size()), "AnimationJob out of range");
//	jobIDToAnimationJob[static_cast<size_t>(job)].animating = true;
//}
//
//void SE::Graphics::Renderer::PauseAnimation(int job)
//{
//	_ASSERT_EXPR(job < static_cast<int>(jobIDToAnimationJob.size()), "AnimationJob out of range");
//	jobIDToAnimationJob[static_cast<size_t>(job)].animating = false;
//}

int SE::Graphics::Renderer::EnableBloom(int handleHorizontal, int handleVertical)
{
	bloomHorizontalHandle = handleHorizontal;
	bloomVerticalHandle = handleVertical;

	int texture2DHandles[3];


	D3D11_TEXTURE2D_DESC texture2DDescription;
	ZeroMemory(&texture2DDescription, sizeof(D3D11_TEXTURE2D_DESC));

	texture2DDescription.Width = 1280;
	texture2DDescription.Height = 720;
	texture2DDescription.MipLevels = 1;
	texture2DDescription.ArraySize = 1;
	texture2DDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2DDescription.SampleDesc.Count = 1;
	texture2DDescription.SampleDesc.Quality = 0;
	texture2DDescription.Usage = D3D11_USAGE_DEFAULT;
	texture2DDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texture2DDescription.CPUAccessFlags = 0;
	texture2DDescription.MiscFlags = 0;

	graphicResourceHandler->CreateTexture2D(texture2DDescription, texture2DHandles[0]);

	texture2DDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	graphicResourceHandler->CreateTexture2D(texture2DDescription, texture2DHandles[1]);
	graphicResourceHandler->CreateTexture2D(texture2DDescription, texture2DHandles[2], true);


	graphicResourceHandler->CreateRenderTargetView(texture2DHandles[0]);


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription;
	ZeroMemory(&shaderResourceViewDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceViewDescription.Format = texture2DDescription.Format;
	shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDescription.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDescription.Texture2D.MipLevels = 1;

	graphicResourceHandler->CreateCustomShaderResourceView(shaderResourceViewDescription, 0, bloomShaderResourceViewHandles[0], device->GetBackBufferTexture());
	graphicResourceHandler->CreateCustomShaderResourceView(shaderResourceViewDescription, texture2DHandles[0], bloomShaderResourceViewHandles[1]);
	graphicResourceHandler->CreateCustomShaderResourceView(shaderResourceViewDescription, texture2DHandles[1], bloomShaderResourceViewHandles[2]);


	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDescription;
	ZeroMemory(&unorderedAccessViewDescription, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

	unorderedAccessViewDescription.Format = shaderResourceViewDescription.Format;
	unorderedAccessViewDescription.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	unorderedAccessViewDescription.Texture2D.MipSlice = 0;

	graphicResourceHandler->CreateUnorderedAccessView(texture2DHandles[1], bloomUnorderedAccessViewHandles[0]);
	graphicResourceHandler->CreateUnorderedAccessView(texture2DHandles[2], bloomUnorderedAccessViewHandles[1]);


	bloom = true;
	return 0;
}

int SE::Graphics::Renderer::DisableBloom()
{
	bloom = false;

	return 0;
}
bool SE::Graphics::Renderer::IsUnderLimit(size_t sizeToAdd)
{
	return memMeasure.GetVRam() + sizeToAdd <= initInfo.maxVRAMUsage;
}

bool SE::Graphics::Renderer::IsUnderLimit(size_t potential, size_t sizeToAdd)
{
	return memMeasure.GetVRam() + sizeToAdd <= initInfo.maxVRAMUsage + potential;
}

