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
		bucket = new RenderBucket(job.pipeline);
		bucket->pipeline = job.pipeline;

	if (findEntity != entityToBucketAndIndexInBucket.end()) // The entity is in another bucket.
		RemoveFromBucket(bucketAndIndexInBucket);

	bucketAndIndexInBucket = { job.pipeline.id, bucket->transforms.size() };
	bucket->indexToEntity.push_back(entity);
	bucket->transforms.push_back({});
	

	
	job.maxInstances = 256;
	job.mappingFunc.push_back([bucket](auto a, auto b) {
		renderer->GetPipelineHandler()->UpdateConstantBuffer("OncePerObject", &bucket->transforms[a], sizeof(DirectX::XMFLOAT4X4) * b);
	});

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
}
