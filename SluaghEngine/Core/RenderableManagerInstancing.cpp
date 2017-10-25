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
	//job.pipeline.SetID();

	const auto findBucket = pipelineToRenderBucket.find(job.pipeline.id);
	auto& bucket = pipelineToRenderBucket[job.pipeline.id];


	const auto findEntity = entityToBucketAndIndexInBucket.find(entity);
	auto& bucketAndIndexInBucket = entityToBucketAndIndexInBucket[entity];
	if (findBucket == pipelineToRenderBucket.end()) // This is a new bucket.
	{
		bucket = new RenderBucket(job.pipeline);
		bucket->pipeline = job.pipeline;
		job.maxInstances = 256;
		job.mappingFunc.push_back([this, bucket](auto a, auto b) {
			renderer->GetPipelineHandler()->UpdateConstantBuffer("OncePerObject", &bucket->transforms[a], sizeof(DirectX::XMFLOAT4X4) * b);
		});
	}

	if (findEntity != entityToBucketAndIndexInBucket.end()) // The entity is in another bucket.
		RemoveFromBucket(bucketAndIndexInBucket);

	bucketAndIndexInBucket = { job.pipeline.id, bucket->transforms.size() };
	bucket->indexToEntity.push_back(entity);
	bucket->transforms.push_back({});



	
	job.instanceCount = bucket->transforms.size();
	

	if (findBucket == pipelineToRenderBucket.end()) // This is a new bucket.
		bucket->jobID = renderer->AddRenderJob(job, Graphics::RenderGroup::SECOND_PASS);
	else
		renderer->ChangeRenderJob(bucket->jobID, job);

	StopProfile;
}

void SE::Core::RenderableManagerInstancing::RemoveEntity(const Entity & entity)
{
	StartProfile;
	auto findEntity = entityToBucketAndIndexInBucket.find(entity);
	if (findEntity != entityToBucketAndIndexInBucket.end())
	{
		RemoveFromBucket(findEntity->second);
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

void SE::Core::RenderableManagerInstancing::RemoveFromBucket(const BucketAndID& bucketAndID)
{
	auto& currentBucket = *pipelineToRenderBucket[bucketAndID.bucket];
	const auto last = currentBucket.transforms.size() - 1;

	// Switch the last entity in the bucket to the removed slot
	currentBucket.transforms[bucketAndID.index] = currentBucket.transforms[last];
	currentBucket.indexToEntity[bucketAndID.index] = currentBucket.indexToEntity[last];
	entityToBucketAndIndexInBucket[currentBucket.indexToEntity[last]].index = bucketAndID.index;
	currentBucket.transforms.pop_back();
	currentBucket.indexToEntity.pop_back();
	if (currentBucket.transforms.size() == 0)
	{
		renderer->RemoveRenderJob(currentBucket.jobID);
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
