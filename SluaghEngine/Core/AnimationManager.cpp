#include <AnimationManager.h>
#include <Profiler.h>
#include <Utilz\Console.h>

SE::Core::AnimationManager::AnimationManager(Graphics::IRenderer * renderer, ResourceHandler::IResourceHandler * resourceHandler, const EntityManager & entityManager, RenderableManager* renderableManager)
	:renderer(renderer), resourceHandler(resourceHandler), entityManager(entityManager), renderableManager(renderableManager)
{
	_ASSERT(renderer);
	_ASSERT(resourceHandler);
	_ASSERT(renderableManager);

	renderableManager->RegisterToSetRenderObjectInfo({ this, &AnimationManager::SetRenderObjectInfo });
	skinnedShader = 0;
	auto res = resourceHandler->LoadResource(Utilz::GUID("SkinnedVS.hlsl"), { this, &AnimationManager::LoadSkinnedShader });
	if (res)
		throw std::exception("Could not load default skinned vertex shader.");


	Allocate(10);
}

SE::Core::AnimationManager::~AnimationManager()
{
	operator delete(animationData.data);
}

void SE::Core::AnimationManager::CreateAnimation(const Entity & entity, const Core::CreateAnimationInfo& info)
{
	StartProfile;
	auto& find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
		ProfileReturnVoid;

	// Check if the entity is alive
	if (!entityManager.Alive(entity))
		ProfileReturnVoid;


	// Make sure we have enough memory.
	if (animationData.used + 1 > animationData.allocated)
		Allocate(animationData.allocated * 2);

	// Register the entity
	auto index = animationData.used++;
	entityToIndex[entity] = index;
	animationData.entity[index] = entity;
	animationData.job[index] = -1;

	// Load skeleton
	auto& findSkeleton = guidToSkeletonIndex.find(info.skeleton);
	auto& skelIndex = guidToSkeletonIndex[info.skeleton];
	if (findSkeleton == guidToSkeletonIndex.end())
	{

		auto res = resourceHandler->LoadResource(info.skeleton, [this, &skelIndex](auto guid, auto data, auto size) {
			skelIndex = LoadSkeleton(data, size);
			if (skelIndex == -1)
				return -1;
			return 1;
		});
		if (res)
		{
			Utilz::Console::Print("Could not load skeleton %u. Error: %d\n", info.skeleton, res);
			animationData.used--;
			ProfileReturnVoid;
		}
	}

	// Load animations
	for (size_t i = 0; i < info.animationCount; i++)
	{
		auto& findSkelAnim = guidToSkelAnimationIndex.find(info.skeleton);
		auto& skelAnimIndex = guidToSkelAnimationIndex[info.skeleton];
		if (findSkeleton == guidToSkelAnimationIndex.end())
		{
			auto res = resourceHandler->LoadResource(info.skeleton, [this, skelIndex, &skelAnimIndex](auto guid, auto data, auto size) {
				skelAnimIndex = LoadAnimation(data, size);
				if (skelAnimIndex == -1)
					return -1;
				return 1;
			});
			if (res)
			{
				Utilz::Console::Print("Could not load animation %u. Error: %d\n", info.skeleton, res);
				ProfileReturnVoid;
			}
		}

	}

	StopProfile;
}

void SE::Core::AnimationManager::Frame()
{
	GarbageCollection();
}

void SE::Core::AnimationManager::Start(const Entity & entity, const Utilz::GUID & animation, float speed)
{	
	StartProfile;
	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		auto& findSkelAnim = guidToSkelAnimationIndex.find(animation);
		if (findSkelAnim != guidToSkelAnimationIndex.end())
		{
			Graphics::AnimationJobInfo info;
			info.skeletonID = animationData.skeletonIndex[entityIndex->second];
			info.animationID = findSkelAnim->second;
			info.speed = speed;
			
			if (animationData.job[entityIndex->second] >= 0) // If the the entity already had an animation playing.
				renderer->UpdateAnimation(animationData.job[entityIndex->second], info); // Update the animation job
			else
			{
				animationData.job[entityIndex->second] = renderer->StartAnimation(info); // Create a new animation job
				renderableManager->UpdateRenderableObject(entity); // And update the renderable manager.
			}
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
		if (animationData.job[entityIndex->second] >= 0)
			renderer->SetAnimationSpeed(animationData.job[entityIndex->second], speed);

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
		if (animationData.job[entityIndex->second] >= 0)
			renderer->StartAnimation(animationData.job[entityIndex->second]);

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
		if (animationData.job[entityIndex->second] >= 0)
			renderer->PauseAnimation(animationData.job[entityIndex->second]);
	}
	StopProfile;
}

void SE::Core::AnimationManager::SetRenderObjectInfo(const Entity & entity, Graphics::RenderObjectInfo * info)
{
	StartProfile;
	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);

	// If the entity index is equal to the end of the undordered map, it means that no animated entity was found
	if (entityIndex == entityToIndex.end()) {
		info->type = Graphics::RenderObjectInfo::JobType::STATIC;
	}

	// Otherwise, there was an animated entity and we should use the skinned vertex shader
	else {

		info->type = Graphics::RenderObjectInfo::JobType::SKINNED;
		info->animationJob = animationData.job[entityIndex->second];
		info->vertexShader = skinnedShader;
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
	newData.skeletonIndex = (size_t*)(newData.entity + newData.size);
	newData.job = (int*)(newData.skeletonIndex + newData.allocated);

	// Copy data
	memcpy(newData.entity, animationData.entity, animationData.used * sizeof(Entity));
	memcpy(newData.skeletonIndex, animationData.skeletonIndex, animationData.used * sizeof(size_t));
	memcpy(newData.job, animationData.job, animationData.used * sizeof(int));

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
	animationData.skeletonIndex[index] = animationData.skeletonIndex[last];
	animationData.job[index] = animationData.job[last];

	// Replace the index for the last_entity 
	entityToIndex[last_entity] = index;
	entityToIndex.erase(entity);

	animationData.used--;
	StopProfile;
}

void SE::Core::AnimationManager::GarbageCollection()
{
	StartProfile;
	uint32_t alive_in_row = 0;
	while (animationData.used > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<uint32_t> distribution(0U, animationData.used - 1U);
		uint32_t i = distribution(generator);
		if (entityManager.Alive(animationData.entity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}

int SE::Core::AnimationManager::LoadSkeleton(void * data, size_t size)
{
	auto skelH = (Graphics::Skeleton_Header*)data;

	// After the skeleton header, there will only be joints
	auto jointAttr = (Graphics::JointAttributes*)(skelH + 1);

	return renderer->CreateSkeleton(jointAttr, skelH->nrOfJoints);
}

int SE::Core::AnimationManager::LoadAnimation(void * data, size_t size)
{
	auto animH = (Graphics::Animation_Header*)data;

	// After the animation header, there will only be matrices of type XMFLOAT4X4
	auto matrices = (DirectX::XMFLOAT4X4*)(animH + 1);

	renderer->CreateAnimation(matrices, animH->animationLength, animH->nrOfJoints);

	return 0;
}

int SE::Core::AnimationManager::LoadSkinnedShader(const Utilz::GUID & guid, void * data, size_t size)
{
	StartProfile;
	skinnedShader = renderer->CreateVertexShader(data, size);
	if (skinnedShader == -1)
		ProfileReturnConst(-1);
	ProfileReturnConst(0);
}