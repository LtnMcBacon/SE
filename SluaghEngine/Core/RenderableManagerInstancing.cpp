#include "RenderableManagerInstancing.h"



SE::Core::RenderableManagerInstancing::RenderableManagerInstancing()
{
}


SE::Core::RenderableManagerInstancing::~RenderableManagerInstancing()
{
}

void SE::Core::RenderableManagerInstancing::AddEntity(const Entity & entity, const Graphics::RenderJob & job)
{
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
