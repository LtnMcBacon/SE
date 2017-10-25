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
	if (findBucketAndID == entityToRenderBucketAndID.end()) // Entity is not in any bucket.
	{
		for (auto& bucket : renderBuckets)
		{
			if (bucket.pipeline == job.pipeline) // The job fits in an already existing bucket.
			{
				
			}
		}


	}
}
