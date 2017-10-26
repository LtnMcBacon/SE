#include "AnimationManager.h"
#include <Profiler.h>


static const SE::Utilz::GUID SkinnedVertexShader("SkinnedVS.hlsl");

SE::Core::AnimationManager::AnimationManager(const InitializationInfo & initInfo) : initInfo(initInfo)
{
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.console);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.transformManager);
	_ASSERT(initInfo.renderableManager);

	initInfo.renderableManager->RegisterToSetRenderObjectInfo({ this, &AnimationManager::SetRenderObjectInfo });
	
	auto result = initInfo.resourceHandler->LoadResource(Utilz::GUID("SkinnedVS.hlsl"), [this](auto guid, auto data, auto size) {
		auto result = initInfo.renderer->GetPipelineHandler()->CreateVertexShader(SkinnedVertexShader, data, size);
		if (result < 0)
			return ResourceHandler::InvokeReturn::Fail;
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	});
	if (result < 0)
		throw std::exception("Could not load SkinnedVertexShader.");


	Allocate(10);
}

SE::Core::AnimationManager::~AnimationManager()
{
	operator delete(animationData.data);
}

void SE::Core::AnimationManager::CreateAnimation(const Entity & entity, const CreateInfo & info)
{
	StartProfile;
	auto& find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
		ProfileReturnVoid;

	// Check if the entity is alive
	if (!initInfo.entityManager->Alive(entity))
		ProfileReturnVoid;


	// Make sure we have enough memory.
	if (animationData.used + 1 > animationData.allocated)
		Allocate(animationData.allocated * 2);

	// Register the entity
	auto index = animationData.used++;
	entityToIndex[entity] = index;
	animationData.entity[index] = entity;

	// Load skeleton
	if (!animationSystem.IsSkeletonLoaded(info.skeleton))
	{

		auto result = initInfo.resourceHandler->LoadResource(info.skeleton, [this](auto guid, auto data, auto size) {
			auto result = LoadSkeleton(guid, data, size);
			if (result < 0)
				return ResourceHandler::InvokeReturn::Fail;
			return ResourceHandler::InvokeReturn::DecreaseRefcount;
		});
		if (result < 0)
		{
			initInfo.console->PrintChannel("Could not load skeleton %u. Error: %d\n", "Resources", info.skeleton, result);
			animationData.used--;
			ProfileReturnVoid;
		}
	}

	animationData.skeleton[index] = info.skeleton;

	// Load animations
	for (size_t i = 0; i < info.animationCount; i++)
	{
		if (!animationSystem.IsAnimationLoaded(info.animations[i]))
		{
			auto result = initInfo.resourceHandler->LoadResource(info.animations[i], [this](auto guid, auto data, auto size) {
				auto result = LoadAnimation(guid, data, size);
				if (result < 0)
					return ResourceHandler::InvokeReturn::Fail;
				return ResourceHandler::InvokeReturn::DecreaseRefcount;
			});
			if (result < 0)
			{
				initInfo.console->PrintChannel("Could not load animation %u. Error: %d", "Resources", info.animations[i], result);
				ProfileReturnVoid;
			}
		}

	}

	StopProfile;
}

void SE::Core::AnimationManager::Frame(Utilz::TimeCluster * timer)
{
	timer->Start(CREATE_ID_HASH("AnimationManager"));
	GarbageCollection();
	timer->Stop(CREATE_ID_HASH("AnimationManager"));
}

void SE::Core::AnimationManager::Start(const Entity & entity, const Utilz::GUID & animation, float speed)
{	
	StartProfile;
	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		if (animationSystem.IsAnimationLoaded(animation))
		{
			//Graphics::AnimationJobInfo info;
			//info.animating = true;
			//info.speed = speed;
			//info.timePos = 0.0f;
			//info.animationHandle = findSkelAnim->second;
			//if (animationData.job[entityIndex->second] >= 0) // If the the entity already had an animation playing.
			//	initInfo.renderer->UpdateAnimation(animationData.job[entityIndex->second], info); // Update the animation job
			//else
			//{
			//	animationData.job[entityIndex->second] = initInfo.renderer->StartAnimation(info); // Create a new animation job
			//	initInfo.renderableManager->UpdateRenderableObject(entity); // And update the renderable manager.
			//}
		}

	}
	StopProfile;
}

void SE::Core::AnimationManager::SetSpeed(const Entity & entity, float speed)
{
	StartProfile;
	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		/*if (animationData.job[entityIndex->second] >= 0)
			initInfo.renderer->SetAnimationSpeed(animationData.job[entityIndex->second], speed);*/

	}
	StopProfile;
}

void SE::Core::AnimationManager::SetKeyFrame(const Entity & entity, float keyFrame)
{
	StartProfile;
	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		/*if (animationData.job[entityIndex->second] >= 0)
			initInfo.renderer->SetKeyFrame(animationData.job[entityIndex->second], keyFrame);*/

	}
	StopProfile;
}

void SE::Core::AnimationManager::Start(const Entity & entity)const
{
	StartProfile;
	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		/*if (animationData.job[entityIndex->second] >= 0)
			initInfo.renderer->StartAnimation(animationData.job[entityIndex->second]);*/

	}
	StopProfile;
}

void SE::Core::AnimationManager::Pause(const Entity & entity)const
{
	StartProfile;
	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		/*if (animationData.job[entityIndex->second] >= 0)
			initInfo.renderer->PauseAnimation(animationData.job[entityIndex->second]);*/
	}
	StopProfile;
}

void SE::Core::AnimationManager::SetRenderObjectInfo(const Entity & entity, Graphics::RenderJob * info)
{
	StartProfile;
	// Get the entity register from the animationManager
	const auto entityIndex = entityToIndex.find(entity);

	// there was an animated entity and we should use the skinned vertex shader
	if (entityIndex != entityToIndex.end()) {
		info->pipeline.VSStage.shader = SkinnedVertexShader;
		info->maxInstances = 8;
	}
	StopProfile;
}

void SE::Core::AnimationManager::Allocate(size_t size)
{
	StartProfile;
	_ASSERT(size > animationData.allocated);

	// Allocate new memory
	AnimationData newData;
	newData.allocated = size;
	newData.data = operator new(size * AnimationData::size);
	newData.used = animationData.used;

	// Setup the new pointers
	newData.entity = (Entity*)newData.data;
	newData.skeleton = (Utilz::GUID*)(newData.entity + newData.size);

	// Copy data
	memcpy(newData.entity, animationData.entity, animationData.used * sizeof(Entity));
	memcpy(newData.skeleton, animationData.skeleton, animationData.used * sizeof(Utilz::GUID));

	// Delete old data;
	operator delete(animationData.data);
	animationData = newData;

	StopProfile;
}

void SE::Core::AnimationManager::Destroy(size_t index)
{
	StartProfile;

	// Temp variables
	size_t last = animationData.used - 1;
	const Entity entity = animationData.entity[index];
	const Entity last_entity = animationData.entity[last];

	// Copy the data
	animationData.entity[index] = last_entity;
	animationData.skeleton[index] = animationData.skeleton[last];

	// Replace the index for the last_entity 
	entityToIndex[last_entity] = index;
	entityToIndex.erase(entity);

	animationData.used--;
	StopProfile;
}

void SE::Core::AnimationManager::Destroy(const Entity & entity)
{
}

void SE::Core::AnimationManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (animationData.used > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<uint32_t> distribution(0U, animationData.used - 1U);
		uint32_t i = distribution(generator);
		if (initInfo.entityManager->Alive(animationData.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}

int SE::Core::AnimationManager::LoadSkeleton(const Utilz::GUID& guid, void * data, size_t size)
{
	auto skelH = (Skeleton_Header*)data;

	// After the skeleton header, there will only be joints
	auto jointAttr = (JointAttributes*)(skelH + 1);

	return animationSystem.AddSkeleton(guid, jointAttr, skelH->nrOfJoints);
}

int SE::Core::AnimationManager::LoadAnimation(const Utilz::GUID& guid, void * data, size_t size)
{
	auto animH = (Animation_Header*)data;

	// After the animation header, there will only be matrices of type XMFLOAT4X4
	auto matrices = (DirectX::XMFLOAT4X4*)(animH + 1);

	return animationSystem.AddAnimation(guid,matrices, animH->animationLength, animH->nrOfJoints);

	//return initInfo.renderer->CreateAnimation(matrices, animH->animationLength, animH->nrOfJoints);
}