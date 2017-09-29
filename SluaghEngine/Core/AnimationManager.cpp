#include <AnimationManager.h>
#include <Profiler.h>
#include <Utilz\Console.h>

SE::Core::AnimationManager::AnimationManager(Graphics::IRenderer * renderer, ResourceHandler::IResourceHandler * resourceHandler, const EntityManager & entityManager)
	:renderer(renderer), resourceHandler(resourceHandler), entityManager(entityManager)
{
	_ASSERT(renderer);
	_ASSERT(resourceHandler);

	Allocate(10);
}

SE::Core::AnimationManager::~AnimationManager()
{
}

void SE::Core::AnimationManager::CreateSkeleton(const Entity & entity, const Utilz::GUID & skeleton)
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


	auto res = resourceHandler->LoadResource(skeleton, ResourceHandler::LoadResourceDelegate::Make<AnimationManager, &AnimationManager::LoadSkeleton>(this));
	if (res)
	{
		Utilz::Console::Print("Could not load skeleton %u. Error: %d\n", skeleton, res);
		ProfileReturnVoid;
	}


}

void SE::Core::AnimationManager::AddAnimation(const Entity & entity, const Utilz::GUID & animation)
{
	StartProfile;
	auto& find = entityToIndex.find(entity);
	if (find == entityToIndex.end())
	{
		Utilz::Console::Print("Can not add animation to an entity without a skeleton\n");
		ProfileReturnVoid;
	}

	auto res = resourceHandler->LoadResource(animation, ResourceHandler::LoadResourceDelegate::Make<AnimationManager, &AnimationManager::LoadAnimation>(this));
	if (res)
	{
		Utilz::Console::Print("Could not load animation %u. Error: %d\n", animation, res);
		ProfileReturnVoid;
	}



}

void SE::Core::AnimationManager::Frame()
{
	GarbageCollection();
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
	//newData.dirty = (size_t*)(newData.entity + newData.allocated);


	// Copy data
	memcpy(newData.entity, animationData.entity, animationData.used * sizeof(Entity));


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
	const Entity& entity = animationData.entity[index];
	const Entity& last_entity = animationData.entity[last];

	// Copy the data
	animationData.entity[index] = last_entity;
	//cameraData.dirty[index] = cameraData.dirty[last];

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

int SE::Core::AnimationManager::LoadSkeleton(const Utilz::GUID & skeleton, void * data, size_t size)
{
	auto skelH = (Skeleton_Header*)data;

	// After the skeleton header, there will only be joints
	JointAttributes* jointAttr = (JointAttributes*)(skelH + 1);

	const auto& index = guidToSkeletonIndex[skeleton];

	auto handle = renderer->CreateSkeleton(jointAttr, skelH->nrOfJoints);
	//skeletonHandle[index]
	//guidToSkeletonIndex[skeleton] = skeletonHandle.size() - 1;

	return 0;
}

int SE::Core::AnimationManager::LoadAnimation(const Utilz::GUID & animation, void * data, size_t size)
{
	auto animH = (Animation_Header*)data;

	// After the animation header, there will only be matrices of type XMFLOAT4X4
	auto matrices = (DirectX::XMFLOAT4X4*)(animH + 1);

	renderer->CreateAnimation(matrices, animH->animationLength, animH->nrOfJoints, 0);

	return 0;
}
