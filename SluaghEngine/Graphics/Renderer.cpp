#include "Renderer.h"
#include <Profiler.h>
#include <ResourceHandler\IResourceHandler.h>

#undef min



SE::Graphics::Renderer::Renderer()
{
	oncePerFrameBufferID = -1;
	device = nullptr;
	graphicResourceHandler = nullptr;
	animationSystem = nullptr;
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
	currentEntityTimePos = 0;

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
	StartProfile;
	renderJobLock.lock();
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
	renderJobLock.unlock();
	ProfileReturnConst(jobID);
}

int SE::Graphics::Renderer::DisableRendering(uint32_t jobID)
{
	StartProfile;
	renderJobLock.lock();
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

	renderJobLock.unlock();
	ProfileReturnConst(0);
}



int SE::Graphics::Renderer::UpdateRenderingBuffer(uint32_t jobID, const RenderObjectInfo& handles)
{
	StartProfile;
	renderJobLock.lock();
	const uint32_t bucketIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].bucketIndex;
	const uint32_t transformIndexOfRemoved = jobIDToBucketAndTransformIndex[jobID].transformIndex;

	auto& bucketOfRemoved = renderBuckets[bucketIndexOfRemoved];

	DirectX::XMFLOAT4X4 transform = bucketOfRemoved.transforms[transformIndexOfRemoved];
	bucketOfRemoved.transforms[transformIndexOfRemoved] = bucketOfRemoved.transforms.back();
	bucketOfRemoved.transforms.pop_back();
	const uint32_t jobThatReplacedOld = bucketOfRemoved.jobsInBucket.back();
	bucketOfRemoved.jobsInBucket[transformIndexOfRemoved] = jobThatReplacedOld;
	bucketOfRemoved.jobsInBucket.pop_back();

	jobIDToBucketAndTransformIndex[jobThatReplacedOld].transformIndex = transformIndexOfRemoved;
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


	const BucketAndTransformIndex bucketAndTransformIndex = { bucketIndex, transformIndex };
	jobIDToBucketAndTransformIndex[jobID] = bucketAndTransformIndex;
	renderBuckets[bucketIndex].jobsInBucket.push_back(jobID);

	renderJobLock.unlock();
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

int SE::Graphics::Renderer::EnableLightRendering(const LightData & handles)
{
	StartProfile;
	renderLightJobs.push_back(handles);
	ProfileReturn(renderLightJobs.size() - 1);
}

int SE::Graphics::Renderer::DisableLightRendering(size_t ID)
{
	StartProfile;
	renderLightJobs[ID] = renderLightJobs[renderLightJobs.size() - 1];
	renderLightJobs.pop_back();
	ProfileReturn(renderLightJobs.size());
}

int SE::Graphics::Renderer::UpdateLightPos(const DirectX::XMFLOAT3& pos, size_t ID)
{
	renderLightJobs[ID].pos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, renderLightJobs[ID].pos.w);
	return 0;
}

int SE::Graphics::Renderer::UpdateView(float * viewMatrix)
{
	DirectX::XMFLOAT4X4 wo;
	DirectX::XMStoreFloat4x4(&wo, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)viewMatrix)));
	graphicResourceHandler->UpdateConstantBuffer(&wo, sizeof(wo), oncePerFrameBufferID);

	return 0;
}

int SE::Graphics::Renderer::Render() {
	StartProfile;

	currentEntityTimePos += 1;

	animationSystem->UpdateAnimation(0, 0, currentEntityTimePos);

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

	// SetLightBuffer Start
	const size_t lightMappingSize = sizeof(DirectX::XMFLOAT4) + sizeof(LightData) * renderLightJobs.size();
	LightDataBuffer lightBufferData;
	lightBufferData.size.x = renderLightJobs.size();

	for (int lightNr = 0; lightNr < renderLightJobs.size(); lightNr++)
	{
		lightBufferData.data[lightNr] = renderLightJobs[lightNr];
	}
	//graphicResourceHandler->UpdateConstantBuffer(&lightBufferData, lightMappingSize, lightBufferID);
	//graphicResourceHandler->BindConstantBuffer(GraphicResourceHandler::ShaderStage::PIXEL, lightBufferID, 2);
	// SetLightBuffer end
	

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

	device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	renderJobLock.lock();
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

		int bindSlot;
		auto viewProjHandle = graphicResourceHandler->GetVSConstantBufferByName(bucket.stateInfo.vertexShader, "OncePerFrame", &bindSlot);
		graphicResourceHandler->BindVSConstantBuffer(oncePerFrameBufferID, bindSlot);

		auto oncePerObject = graphicResourceHandler->GetVSConstantBufferByName(bucket.stateInfo.vertexShader, "OncePerObject", &bindSlot);
		graphicResourceHandler->BindVSConstantBuffer(oncePerObject, bindSlot);

		if(job.type == RenderObjectInfo::JobType::STATIC){

			const size_t instanceCount = bucket.transforms.size();
			
			int binsSlotInvers;
			int InversBufferHandle = graphicResourceHandler->GetVSConstantBufferByName(bucket.stateInfo.vertexShader, "InversWorld", &binsSlotInvers);
			graphicResourceHandler->BindVSConstantBuffer(InversBufferHandle, binsSlotInvers);

			std::vector<DirectX::XMFLOAT4X4> inversVec;
			for (int i = 0; i < bucket.transforms.size(); i++)
			{
				DirectX::XMMATRIX invers = DirectX::XMLoadFloat4x4(&bucket.transforms[i]);
				invers = DirectX::XMMatrixInverse(nullptr, invers);
				DirectX::XMFLOAT4X4 fInvers;
				DirectX::XMStoreFloat4x4(&fInvers, invers);
				inversVec.push_back(fInvers);
			}
			for (int i = 0; i < instanceCount; i += maxDrawInstances)
			{
				const size_t instancesToDraw = std::min(bucket.transforms.size() - i, (size_t)maxDrawInstances);
				const size_t mapSize = sizeof(DirectX::XMFLOAT4X4) * instancesToDraw;
				graphicResourceHandler->UpdateConstantBuffer(&bucket.transforms[i], mapSize, oncePerObject);
				graphicResourceHandler->UpdateConstantBuffer(&inversVec[i], mapSize, InversBufferHandle);
				device->GetDeviceContext()->DrawInstanced(graphicResourceHandler->GetVertexCount(bucket.stateInfo.bufferHandle), instancesToDraw, 0, 0);
			}

		}

		else if (job.type == RenderObjectInfo::JobType::SKINNED) {

			int boneBindslot;
			const int cBoneBufferIndex = graphicResourceHandler->GetVSConstantBufferByName(bucket.stateInfo.vertexShader, "VS_SKINNED_DATA", &boneBindslot);
			graphicResourceHandler->BindVSConstantBuffer(cBoneBufferIndex, boneBindslot);

			
			std::vector<DirectX::XMFLOAT4X4> inversVec;
			for (int i = 0; i < bucket.transforms.size(); i++)
			{
				DirectX::XMMATRIX invers = DirectX::XMLoadFloat4x4(&bucket.transforms[i]);
				invers = DirectX::XMMatrixInverse(nullptr, invers);
				DirectX::XMFLOAT4X4 fInvers;
				DirectX::XMStoreFloat4x4(&fInvers, invers);
				inversVec.push_back(fInvers);
			}

			bucket.gBoneTransforms = animationSystem->GetSkeleton(0).jointArray;
			graphicResourceHandler->UpdateConstantBuffer(&bucket.gBoneTransforms[0], sizeof(DirectX::XMFLOAT4X4) * 4, cBoneBufferIndex);

			const size_t instanceCount = bucket.transforms.size();
			for (int i = 0; i < instanceCount; i += maxDrawInstances)
			{
				const size_t instancesToDraw = std::min(bucket.transforms.size() - i, (size_t)maxDrawInstances);
				const size_t mapSize = sizeof(DirectX::XMFLOAT4X4) * instancesToDraw;	
				graphicResourceHandler->UpdateConstantBuffer(&bucket.transforms[i], mapSize, oncePerObject);
				device->GetDeviceContext()->DrawInstanced(graphicResourceHandler->GetVertexCount(bucket.stateInfo.bufferHandle), instancesToDraw, 0, 0);
			}
		}

		
		previousJob = job;
	}
	renderJobLock.unlock();

	///********** Render line jobs ************/

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
	//
	///********END render line jobs************/


	//if (renderTextureJobs.size())
	//{
	//	spriteBatch->Begin(DirectX::SpriteSortMode_Texture, device->GetBlendState());
	//	for (auto& job : renderTextureJobs)
	//	{
	//		spriteBatch->Draw(graphicResourceHandler->GetShaderResourceView(job.textureID), job.pos, job.rect, XMLoadFloat3(&job.colour), job.rotation, job.origin, job.scale, job.effect, job.layerDepth);
	//	}
	//	spriteBatch->End();
	//}
	//
	//if (renderTextJobs.size())
	//{
	//	spriteBatch->Begin();
	//	for (auto& job : renderTextJobs)
	//	{
	//		fonts[job.fontID].DrawString(spriteBatch.get(), job.text.c_str(), job.pos, XMLoadFloat3(&job.colour), job.rotation, job.origin, job.scale, job.effect, job.layerDepth);
	//	}
	//	spriteBatch->End();
	//}

	device->SetDepthStencilStateAndRS();
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

int SE::Graphics::Renderer::CreateTexture(void* data, const TextureDesc& description)
{
	StartProfile;
	ProfileReturn(graphicResourceHandler->CreateShaderResourceView(data, description))
}


int SE::Graphics::Renderer::UpdateTransform(uint32_t jobID, float* transform)
{
	StartProfile;
	renderJobLock.lock();
	DirectX::XMMATRIX trans = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)transform);
	DirectX::XMFLOAT4X4 transposed;
	DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(trans));
	
	const size_t bucketIndex = jobIDToBucketAndTransformIndex[jobID].bucketIndex;
	const size_t transformIndex = jobIDToBucketAndTransformIndex[jobID].transformIndex;

	renderBuckets[bucketIndex].transforms[transformIndex] = transposed;
	renderJobLock.unlock();
	ProfileReturnConst(0);
}

int SE::Graphics::Renderer::UpdateBoneTransform(uint32_t jobID, float* transforms, size_t nrOfJoints) {

	StartProfile;

	auto& indices = jobIDToBucketAndTransformIndex[jobID];

	auto& bucket = renderBuckets[indices.bucketIndex];

	bucket.gBoneTransforms.resize(nrOfJoints);

	memcpy(&bucket.gBoneTransforms[indices.boneIndex], transforms, nrOfJoints * sizeof(DirectX::XMFLOAT4X4));

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

int SE::Graphics::Renderer::RetFontData(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	fonts.push_back(DirectX::SpriteFont(device->GetDevice(), (uint8_t*)data, size));
	ProfileReturn(0);
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


int SE::Graphics::Renderer::CreateSkeleton(JointAttributes* jointData, size_t nrOfJoints) {

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

