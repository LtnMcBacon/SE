#include "AnimationSystem.h"
#include "AnimationShadowSystem.h"
#include <Profiler.h>
SE::Core::AnimationShadowSystem::JointMatrices  SE::Core::AnimationShadowSystem::mats;
SE::Core::AnimationShadowSystem::AnimationShadowSystem(Graphics::IRenderer* renderer, AnimationSystem* animSys) : RenderableManagerInstancing(renderer), animSys(animSys)
{
	DirectX::XMFLOAT4X4 id;
	DirectX::XMStoreFloat4x4(&id, DirectX::XMMatrixIdentity());
	for (auto i = 0; i < 30; i++)
		mats.jointMatrix[i] = id;
}

SE::Core::AnimationShadowSystem::~AnimationShadowSystem()
{
}

void SE::Core::AnimationShadowSystem::AddEntity(const Entity& entity, Graphics::RenderJob& job,
	Graphics::RenderGroup group)
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
		
		auto hax = job.specialHaxxor;
		job.mappingFunc.push_back([this, bucket, hax](auto a, auto b) {

			auto anbuck = (AnimationBucket*)bucket;
			for (const auto& ent : anbuck->indexToEntity)
			{
				auto find = animSys->entityToBucketAndIndexInBucket.find(ent);

				const auto& stolenBuckAndIndex = animSys->entityToBucketAndIndexInBucket[ent];
				auto boneMatricesToStealPtr = ((AnimationBucket*)animSys->pipelineToRenderBucket[stolenBuckAndIndex.bucket])->matrices[animSys->mapingIndex][stolenBuckAndIndex.index].jointMatrix;
				memcpy(anbuck->matrices[0][entityToBucketAndIndexInBucket[ent].index].jointMatrix, boneMatricesToStealPtr, sizeof(JointMatrices));

			}
			renderer->GetPipelineHandler()->UpdateConstantBuffer("VS_SKINNED_DATA", anbuck->matrices[0].data(), sizeof(JointMatrices) * b);
		});

		job.mappingFunc.push_back([this, bucket, hax](auto a, auto b) {
			auto anbuck = (AnimationBucket*)bucket;
			for (const auto& ent : anbuck->indexToEntity)
			{

				const auto& stolenBuckAndIndex = animSys->entityToBucketAndIndexInBucket[ent];
				auto transformMatrixToSteal = ((AnimationBucket*)animSys->pipelineToRenderBucket[stolenBuckAndIndex.bucket])->transforms[stolenBuckAndIndex.index];
				memcpy(&anbuck->transforms[entityToBucketAndIndexInBucket[ent].index], &transformMatrixToSteal, sizeof(DirectX::XMFLOAT4X4));
			}
			renderer->GetPipelineHandler()->UpdateConstantBuffer(hax, anbuck->transforms.data(), sizeof(DirectX::XMFLOAT4X4) * b);
		});
	}
	else
	{
		if (findEntity != entityToBucketAndIndexInBucket.end())
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

void SE::Core::AnimationShadowSystem::RemoveEntity(const Entity& entity)
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

void SE::Core::AnimationShadowSystem::UpdateTransform(const Entity& entity, const DirectX::XMFLOAT4X4& transform)
{
	StartProfile;

	auto find = entityToBucketAndIndexInBucket.find(entity);

	if (find != entityToBucketAndIndexInBucket.end()) {

		DirectX::XMFLOAT4X4 transposed;
		DirectX::XMStoreFloat4x4(&transposed, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&transform)));
		pipelineToRenderBucket[find->second.bucket]->transforms[find->second.index] = transposed;

	}

	StopProfile;
}

void SE::Core::AnimationShadowSystem::AnimationBucket::AddEntity(const Entity& entity,
	const DirectX::XMFLOAT4X4& transform, RenderableManagerInstancing::BucketAndID& bucketAndID)
{
	RenderBucket::AddEntity(entity, transform, bucketAndID);

	matrices[0].push_back(mats);
	matrices[1].push_back(mats);
}

void SE::Core::AnimationShadowSystem::AnimationBucket::RemoveFromBucket(RenderableManagerInstancing* rm, size_t index,
	DirectX::XMFLOAT4X4* transform)
{
	const auto last = matrices[0].size() - 1;
	matrices[1][index] = matrices[1][last];
	matrices[0][index] = matrices[0][last];

	matrices[0].pop_back();
	matrices[1].pop_back();

	RenderBucket::RemoveFromBucket(rm, index, transform);
}

SE::Core::RenderableManagerInstancing::RenderBucket* SE::Core::AnimationShadowSystem::CreateBucket(
	Graphics::RenderJob& job)
{
	StartProfile;
	auto bucket = new AnimationBucket(job.pipeline);
	bucket->pipeline = job.pipeline;
	//	job.maxInstances = 256; Set from the outside
	
	ProfileReturnConst(bucket);
}
