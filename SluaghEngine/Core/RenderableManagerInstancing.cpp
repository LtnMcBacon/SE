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

void SE::Core::RenderableManagerInstancing::AddEntity(const Entity & entity, Graphics::RenderJob & job, Graphics::RenderGroup group)
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
			if (pipelineToRenderBucket[bucketAndIndexInBucket.bucket]->pipeline.id == job.pipeline.id)
				ProfileReturnVoid;
	}

	DirectX::XMFLOAT4X4 transform;

	if (findEntity != entityToBucketAndIndexInBucket.end()) // The entity is in another bucket.
		pipelineToRenderBucket[findEntity->second.bucket]->RemoveFromBucket(this, bucketAndIndexInBucket.index, &transform);

	bucket->AddEntity(entity, transform, bucketAndIndexInBucket);



	
	job.instanceCount = bucket->transforms.size();
	
	if (findBucket == pipelineToRenderBucket.end()) // This is a new bucket.
		bucket->jobID = renderer->AddRenderJob(job, group);
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
		pipelineToRenderBucket[findEntity->second.bucket]->RemoveFromBucket(this, findEntity->second.index, nullptr);
		entityToBucketAndIndexInBucket.erase(entity);
	}


	StopProfile;
}

void SE::Core::RenderableManagerInstancing::UpdateTransform(const Entity & entity, const DirectX::XMFLOAT4X4 & transform)
{
	StartProfile;

	auto find = entityToBucketAndIndexInBucket.find(entity);

	if(find != entityToBucketAndIndexInBucket.end()){

		DirectX::XMFLOAT4X4 transposed;
		DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&transform)));
		pipelineToRenderBucket[find->second.bucket]->transforms[find->second.index] = transposed;

	}

	StopProfile;
}

SE::Core::RenderableManagerInstancing::RenderBucket * SE::Core::RenderableManagerInstancing::CreateBucket(Graphics::RenderJob & job)
{
	StartProfile;
	auto bucket = new RenderBucket(job.pipeline);
	//	job.maxInstances = 256; Set from the outside
	auto hax = job.specialHaxxor;
	job.mappingFunc.push_back([this, bucket, hax](auto a, auto b) {
		renderer->GetPipelineHandler()->UpdateConstantBuffer(hax, &bucket->transforms[a], sizeof(DirectX::XMFLOAT4X4) * b);
	});
	ProfileReturnConst(bucket);
}

void SE::Core::RenderableManagerInstancing::RenderBucket::AddEntity(const Entity & entity, const DirectX::XMFLOAT4X4 & transform, BucketAndID & bucketAndID)
{
	bucketAndID = { pipeline.id, transforms.size() };
	indexToEntity.push_back(entity);
	transforms.push_back(transform);
}

void SE::Core::RenderableManagerInstancing::RenderBucket::RemoveFromBucket(RenderableManagerInstancing* rm, size_t index, DirectX::XMFLOAT4X4 * transform)
{
	StartProfile;
	const auto last = transforms.size() - 1;

	// Switch the last entity in the bucket to the removed slot
	if (transform)
		*transform = transforms[index];
	transforms[index] = transforms[last];
	indexToEntity[index] = indexToEntity[last];
	rm->entityToBucketAndIndexInBucket[indexToEntity[last]].index = index;
	transforms.pop_back();
	indexToEntity.pop_back();
	if (transforms.size() == 0)
	{
		rm->renderer->RemoveRenderJob(jobID);
		rm->pipelineToRenderBucket.erase(pipeline.id);
		delete this;
	}
	else
	{
		rm->renderer->ChangeRenderJob(jobID, [](Graphics::RenderJob& job)
		{
			job.instanceCount--;
		});
	}
	StopProfile;
}
