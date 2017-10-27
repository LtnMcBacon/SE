#include "RenderableManagerInstancing.h"
#include <Profiler.h>


SE::Core::RenderableManagerInstancing::RenderableManagerInstancing(Graphics::IRenderer* renderer) : renderer(renderer)
{
}


SE::Core::RenderableManagerInstancing::~RenderableManagerInstancing()
{
	for (auto& b : pipelineToRenderBucket)
		delete b.second;
}

void SE::Core::RenderableManagerInstancing::AddEntity(const Entity & entity, Graphics::RenderJob & job)
{
	StartProfile;
	job.pipeline.SetID();

	const auto findBucket = pipelineToRenderBucket.find(job.pipeline.id);
	auto& bucket = pipelineToRenderBucket[job.pipeline.id];

	const auto findEntity = entityToBucketAndIndexInBucket.find(entity);
	auto& bucketAndIndexInBucket = entityToBucketAndIndexInBucket[entity];
	if (findBucket == pipelineToRenderBucket.end()) // This is a new bucket.
	{
		bucket = CreateBucket(job);		
	}
	else
	{
		if(findEntity != entityToBucketAndIndexInBucket.end())
			if (bucket->pipeline.id == job.pipeline.id)
				ProfileReturnVoid;
	}

	DirectX::XMFLOAT4X4 transform;

	if (findEntity != entityToBucketAndIndexInBucket.end()) // The entity is in another bucket.
		RemoveFromBucket(bucketAndIndexInBucket, &transform);

	bucketAndIndexInBucket = { job.pipeline.id, bucket->transforms.size() };
	bucket->indexToEntity.push_back(entity);

	//DirectX::XMFLOAT4X4 transposed;
	//DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&transform)));
	bucket->transforms.push_back(transform);



	
	job.instanceCount = bucket->transforms.size();
	

	if (findBucket == pipelineToRenderBucket.end()) // This is a new bucket.
		bucket->jobID = renderer->AddRenderJob(job, Graphics::RenderGroup::SECOND_PASS);
	else
		renderer->ChangeRenderJob(bucket->jobID, [&job](Graphics::RenderJob& ejob) {
		ejob.instanceCount = job.instanceCount;
	});

	StopProfile;
}

void SE::Core::RenderableManagerInstancing::RemoveEntity(const Entity & entity)
{
	StartProfile;
	auto findEntity = entityToBucketAndIndexInBucket.find(entity);
	if (findEntity != entityToBucketAndIndexInBucket.end())
	{
		RemoveFromBucket(findEntity->second, nullptr);
		entityToBucketAndIndexInBucket.erase(entity);
	}


	StopProfile;
}

void SE::Core::RenderableManagerInstancing::UpdateTransform(const Entity & entity, const DirectX::XMFLOAT4X4 & transform)
{
	auto& bai = entityToBucketAndIndexInBucket[entity];
	DirectX::XMFLOAT4X4 transposed;
	DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&transform)));
	pipelineToRenderBucket[bai.bucket]->transforms[bai.index] = transposed;
}

SE::Core::RenderableManagerInstancing::RenderBucket * SE::Core::RenderableManagerInstancing::CreateBucket(Graphics::RenderJob & job)
{
	auto bucket = new RenderBucket(job.pipeline);
	bucket->pipeline = job.pipeline;
	//	job.maxInstances = 256; Set from the outside
	auto hax = job.specialHaxxor;
	job.mappingFunc.push_back([this, bucket, hax](auto a, auto b) {
		renderer->GetPipelineHandler()->UpdateConstantBuffer(hax, &bucket->transforms[a], sizeof(DirectX::XMFLOAT4X4) * b);
	});
	return bucket;
}

void SE::Core::RenderableManagerInstancing::RemoveFromBucket(const BucketAndID& bucketAndID, DirectX::XMFLOAT4X4* transform)
{
	auto& currentBucket = *pipelineToRenderBucket[bucketAndID.bucket];
	const auto last = currentBucket.transforms.size() - 1;

	// Switch the last entity in the bucket to the removed slot
	if(transform)
		*transform = currentBucket.transforms[bucketAndID.index];
	currentBucket.transforms[bucketAndID.index] = currentBucket.transforms[last];
	currentBucket.indexToEntity[bucketAndID.index] = currentBucket.indexToEntity[last];
	entityToBucketAndIndexInBucket[currentBucket.indexToEntity[last]].index = bucketAndID.index;
	currentBucket.transforms.pop_back();
	currentBucket.indexToEntity.pop_back();
	if (currentBucket.transforms.size() == 0)
	{
		renderer->RemoveRenderJob(currentBucket.jobID);
		delete pipelineToRenderBucket[bucketAndID.bucket];
		pipelineToRenderBucket.erase(bucketAndID.bucket);
	}	
	else
	{
		renderer->ChangeRenderJob(currentBucket.jobID, [](Graphics::RenderJob& job)
		{
			job.instanceCount--;
		});
		// TODO: Change instance count for job.
	}

	
}
