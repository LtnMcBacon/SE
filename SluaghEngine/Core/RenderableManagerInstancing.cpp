#include "RenderableManagerInstancing.h"



SE::Core::RenderableManagerInstancing::RenderableManagerInstancing()
{
}


SE::Core::RenderableManagerInstancing::~RenderableManagerInstancing()
{
}

SE::Utilz::GUID SE::Core::RenderableManagerInstancing::AddEntity(const Entity & entity, const Graphics::RenderJob & job)
{
	//job.pipeline.SetID();

	const auto findBucket = pipelineToRenderBucket.find(job.pipeline.id);
	auto& bucket = pipelineToRenderBucket[job.pipeline.id];
	

	const auto findEntity = entityToBucketAndIndexInBucket.find(entity);
	auto& bucketAndIndexInBucket = entityToBucketAndIndexInBucket[entity];
	if (findBucket == pipelineToRenderBucket.end()) // This is a new bucket.
	{
		if (findEntity == entityToBucketAndIndexInBucket.end()) // The entity is not in another bucket.
		{
			bucket.pipeline = job.pipeline;
			bucketAndIndexInBucket = { job.pipeline.id, 0 };
			bucket.indexToEntity.push_back(entity);
			bucket.transforms.push_back({});

		}
		else // The entity is in another bucket.
		{

		}
	}
	else // This bucket existed.
	{
		if (findEntity == entityToBucketAndIndexInBucket.end()) // The entity is not in another bucket.
		{
			bucketAndIndexInBucket = { job.pipeline.id, bucket.indexToEntity.size() };
			bucket.indexToEntity.push_back(entity);
			bucket.transforms.push_back({});
		}
		else // The entity is in another bucket.
		{

		}
	}



	const auto findBucketAndID = entityToRenderBucketAndID.find(entity);
	auto& bucketAndID = entityToRenderBucketAndID[entity];
	if (findBucketAndID == entityToRenderBucketAndID.end()) // Entity is not in any bucket.
	{
		for(size_t i = 0; i< renderBuckets.size(); i++)
		{
			auto& bucket = renderBuckets[i];
			if (bucket.pipeline == job.pipeline) // The job fits in an already existing bucket.
			{
				bucketAndID.ID = bucket.transforms.size();
				bucket.transforms.push_back({});
				bucketAndID.bucket = i;

				goto Done;
			}
			
		}

		bucketAndID.bucket = renderBuckets.size();
		renderBuckets.push_back({ job.pipeline });
		
		bucketAndID.ID = 0;
		renderBuckets[bucketAndID.bucket].transforms.push_back({});
		
	}


Done:

	return;
}
