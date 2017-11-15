#include "AnimationManager.h"
#include <Profiler.h>
#include <Graphics\VertexStructs.h>
#include <Imgui\imgui.h>

static const SE::Utilz::GUID SkinnedVertexShader("SkinnedVS.hlsl");
static const SE::Utilz::GUID SkinnedOncePerObject("SkinnedOncePerObject");

SE::Core::AnimationManager::AnimationManager(const IAnimationManager::InitializationInfo & initInfo) : initInfo(initInfo)
{
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.console);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.eventManager);
	_ASSERT(initInfo.transformManager);

	initInfo.eventManager->RegisterToSetRenderObjectInfo({ this, &AnimationManager::CreateRenderObjectInfo });
	initInfo.eventManager->RegisterToToggleVisible({ this, &AnimationManager::ToggleVisible });

	animationSystem = new AnimationSystem(initInfo.renderer);

	renderableManager = new RenderableManager({ initInfo.resourceHandler, initInfo.renderer,
		initInfo.console, initInfo.entityManager,
		initInfo.eventManager, initInfo.transformManager },
		10, animationSystem);

	ResourceHandler::Callbacks sC;
	sC.loadCallback = [this](auto guid, auto data, auto size, auto udata, auto usize)
	{
		*usize = size;
		auto result = this->initInfo.renderer->GetPipelineHandler()->CreateVertexShader(guid, data, size);
		if (result < 0)
			return ResourceHandler::LoadReturn::FAIL;
		return ResourceHandler::LoadReturn::SUCCESS;
	};
	sC.destroyCallback = [this](auto guid, auto data, auto size)
	{
		this->initInfo.renderer->GetPipelineHandler()->DestroyVertexShader(guid);
	};
	sC.invokeCallback = [](auto guid, auto data, auto size)
	{
		return ResourceHandler::InvokeReturn::SUCCESS;
	};
	auto result = initInfo.resourceHandler->LoadResource(SkinnedVertexShader, sC, ResourceHandler::LoadFlags::IMMUTABLE | ResourceHandler::LoadFlags::LOAD_FOR_VRAM);
	if (result < 0)
		throw std::exception("Could not load SkinnedVertexShader.");

	Allocate(10);
}

SE::Core::AnimationManager::~AnimationManager()
{
	delete renderableManager;
	operator delete(animationData.data);
}

void SE::Core::AnimationManager::CreateAnimatedObject(const Entity & entity, const IAnimationManager::CreateInfo & info)
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
	animationData.animInfo[index].nrOfLayers = 0;
	animationData.playing[index] = 0u;
	animationData.attacher[index] = {};

	for(size_t j = 0; j < AnimationPlayInfo::maxLayers; j++){

	animationData.animInfo[index].timePos[j] = 0.0f;
	animationData.animInfo[index].blendFactor[j] = -1.0f;
	animationData.animInfo[index].blendSpeed[j] = 0.0f;
	animationData.animInfo[index].blockBlending[j] = false;

	}

	renderableManager->CreateRenderableObject(entity, { info.mesh });
	
	// Load skeleton
	if (!animationSystem->IsSkeletonLoaded(info.skeleton))
	{

		auto result = initInfo.resourceHandler->LoadResource(info.skeleton, [this](auto guid, auto data, auto size) {
			auto result = LoadSkeleton(guid, data, size);
			if (result < 0)
				return ResourceHandler::InvokeReturn::FAIL;
			return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
		});
		if (result < 0)
		{
			initInfo.console->PrintChannel("Resources", "Could not load skeleton %u. Error: %d\n",  info.skeleton, result);
			animationData.used--;
			ProfileReturnVoid;
		}
	}

	animationData.animInfo[index].skeleton = info.skeleton;


	// Load animations
	for (size_t i = 0; i < info.animationCount; i++)
	{
		if (!animationSystem->IsAnimationLoaded(info.animations[i]))
		{
			auto result = initInfo.resourceHandler->LoadResource(info.animations[i], [this](auto guid, auto data, auto size) {
				auto result = LoadAnimation(guid, data, size);
				if (result < 0)
					return ResourceHandler::InvokeReturn::FAIL;
				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
			});
			if (result < 0)
			{
				initInfo.console->PrintChannel("Resources", "Could not load animation %u. Error: %d",  info.animations[i], result);
				ProfileReturnVoid;
			}
		}

	}

	// Set Playing to false
	animationData.playing[index] = 0u;

	StopProfile;
}

void SE::Core::AnimationManager::Frame(Utilz::TimeCluster * timer)
{
	timer->Start(("AnimationManager"));
	renderableManager->Frame(nullptr);
	static std::future<bool> lambda;
	auto dt = initInfo.window->GetDelta();

	aniUpdateTime += dt;
	if (aniUpdateTime > 0.033f)
	{
		if (lambda.valid())
		{
			lambda.get();
		}

		animationSystem->UpdateMatricesIndex();
		for (size_t i = 0; i < animationData.used; i++)
		{
			if (animationData.playing[i] == 1u)
			{
				auto& ai = animationData.animInfo[i];

				for (size_t j = 0; j < ai.nrOfLayers; j++) {

					if (ai.blendFactor[j] < 0.0) {
					
						if (ai.toBlendTarget == true) {

							ai.blendSpeed[j] = 0.0f;
							ai.blendFactor[j] = 0.0f;
						}

						else {

							if (ai.toBlendSource == true) {

								if (j == ai.blendBackInfo.animIndex) {

									ai.toBlendSource = false;

									for(size_t k = 0; k < ai.nrOfLayers - 1; k++){

										ai.animationSpeed[k] = ai.blendBackInfo.previousSpeed[k];
									}
								}
							}

							OverwriteAnimation(ai, j, ai.nrOfLayers - 1);
							ai.nrOfLayers--;

						}
						
					}

					if (ai.toBlendTarget == true) {

						if (j == ai.blendBackInfo.animIndex) {

							if (ai.timePos[j] >= ai.blendBackInfo.animLength) {

								ai.toBlendTarget = false;
								ai.toBlendSource = true;
								ai.blendSpeed[j] = -15.0f;

								for (size_t index = 0; index < ai.nrOfLayers; index++) {

									if (index != j) {

										ai.blendSpeed[index] = 15.0f;
									}
								}
							}
						}
					}

					ai.timePos[j] += ai.animationSpeed[j] * aniUpdateTime;

					ai.blendFactor[j] += ai.blendSpeed[j] * dt;
					ai.blendFactor[j] = min(ai.blendFactor[j], 1.0f);

				}
				updateJob.push_back({ animationData.entity[i], ai });
				//animationSystem->CalculateMatrices(animationData.entity[i], ai, true);
			}

		}
		auto UpdateLoop = [this]()
		{
			for (size_t i = 0; i < updateJob.size(); i++)
			{
				animationSystem->CalculateMatrices(updateJob[i].ent, updateJob[i].animInfo, true);
			}

			updateJob.clear();
			return true;
		};

		UpdateLoop();
		
		//lambda = initInfo.threadPool->Enqueue(UpdateLoop);
		aniUpdateTime = 0.0f;
	}

	for (size_t i = 0; i < animationData.used; i++)
	{
		auto& att = animationData.attacher[i];

		for (size_t k = 0; k < Attacher::maxSlots; k++) {


			// If an entity is attached to this entity...
			if (att.slots[k].attached == true) {

				// Get the joint transformation matrix
				DirectX::XMFLOAT4X4 matrix;
				animationSystem->GetJointMatrix(animationData.entity[i], att.slots[k].jointIndex, matrix);
				DirectX::XMFLOAT4X4 parentTransform = initInfo.transformManager->GetTransform(animationData.entity[i]);

				// Get the joint inversed inverse bindpose
				DirectX::XMMATRIX inverseBindPose = DirectX::XMMatrixIdentity();
				animationSystem->GetJointInverseBindPose(animationData.animInfo[i].skeleton, att.slots[k].jointIndex, inverseBindPose);
				inverseBindPose = DirectX::XMMatrixInverse(nullptr, inverseBindPose);

				// Decompose the joint transformation matrix
				DirectX::XMVECTOR jointScale, jointQuat, jointTrans;
				DirectX::XMMatrixDecompose(&jointScale, &jointQuat, &jointTrans, inverseBindPose * XMLoadFloat4x4(&matrix) * XMLoadFloat4x4(&parentTransform));

				DirectX::XMFLOAT4X4 transform;
				DirectX::XMFLOAT4X4 localTransform = initInfo.transformManager->GetTransform(att.slots[k].entity);
				DirectX::XMStoreFloat4x4(&transform, XMLoadFloat4x4(&localTransform) * inverseBindPose * XMLoadFloat4x4(&matrix) * XMLoadFloat4x4(&parentTransform));
				initInfo.transformManager->SetTransform(att.slots[k].entity, transform);

			}
		}
	}
	renderableManager->Frame(nullptr);
	GarbageCollection();
	timer->Stop(("AnimationManager"));
}

void SE::Core::AnimationManager::AttachToEntity(const Entity& source, const Entity& entityToAttach, const Utilz::GUID& jointGUID, int slotIndex) {

	// Assert the given slot index is larger than max slots
	_ASSERT(slotIndex < Attacher::maxSlots);

	// Find the source entity
	auto &sourceEntityIndex = entityToIndex.find(source);
	if (sourceEntityIndex != entityToIndex.end())
	{
		// Get animation info and attacher slots for the source entity
		auto& ai = animationData.animInfo[sourceEntityIndex->second];
		auto& at = animationData.attacher[sourceEntityIndex->second];

		// Check if the entity to attach is alive
		if(initInfo.entityManager->Alive(entityToAttach)){

			// If the entity to attach exists, check if the joint can be found in the source entity skeleton
			int found = animationSystem->FindJointIndex(ai.skeleton, jointGUID);
			if(found != -1){

				at.slots[slotIndex].attached = true;
				at.slots[slotIndex].entity = entityToAttach;
				at.slots[slotIndex].jointIndex = found;

				initInfo.transformManager->Create(at.slots[slotIndex].entity);

			}

		}
	}
}
void SE::Core::AnimationManager::DettachFromEntity(const Entity& source, int slotIndex) {

	// Assert the given slot index is larger than max slots
	_ASSERT(slotIndex < Attacher::maxSlots);

	// Find the source entity
	auto &sourceEntityIndex = entityToIndex.find(source);
	if (sourceEntityIndex != entityToIndex.end())
	{
		// Get animation info and attacher slots for the source entity
		auto& at = animationData.attacher[sourceEntityIndex->second];
		at.slots[slotIndex].attached = false;

	}
}
bool SE::Core::AnimationManager::Start(const Entity & entity, const Utilz::GUID * animations, size_t nrOfAnims, float duration, AnimationFlags flag)
{
	StartProfile;

	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		auto& ai = animationData.animInfo[entityIndex->second];

		Utilz::GUID GUIDTemporaryStorage[maxLayers];
		int animationsNotRunning = 0;
		
		for (int i = 0; i < nrOfAnims; i++)
		{
			bool alreadyRunning = false;
			for (int j = 0; j < ai.nrOfLayers; j++)
			{
				if (ai.animation[j] == animations[i])
				{
					alreadyRunning = true;
					break;
				}
			}
			if (!alreadyRunning)
				GUIDTemporaryStorage[animationsNotRunning++] = animations[i];
		}
		
		nrOfAnims = animationsNotRunning;
		if (!nrOfAnims)
			ProfileReturnConst(false);


		for (size_t i = 0; i < nrOfAnims; i++)
		{
			if (!animationSystem->IsAnimationLoaded(GUIDTemporaryStorage[i]))
			{
				auto result = initInfo.resourceHandler->LoadResource(GUIDTemporaryStorage[i], [this](auto guid, auto data, auto size) {
					auto result = LoadAnimation(guid, data, size);
					if (result < 0)
						return ResourceHandler::InvokeReturn::FAIL;
					return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
				});
				if (result < 0)
				{
					initInfo.console->PrintChannel("Resources", "Could not load animation %u. Error: %d", GUIDTemporaryStorage[i], result);
					GUIDTemporaryStorage[i] = GUIDTemporaryStorage[nrOfAnims - 1];
					i--;
					nrOfAnims--;
				}
			}
		}

		// If the animation flag is set to force blending...
		if (!(flag & AnimationFlags::FORCEBLENDING)) {

			// Loop through all layers in the animation info for this entity
			for (size_t i = 0; i < ai.nrOfLayers; i++) {

				// If this animation layer doesn't allow blending, simply return. 
				// All layers must support blending for this. 
				if (ai.blockBlending[i]) {

					ProfileReturnConst(false);
				}
			}
		}

		if (flag & AnimationFlags::IMMEDIATE) {

			for (size_t i = 0; i < nrOfAnims; i++) {

				ai.animation[i] = GUIDTemporaryStorage[i];
				unsigned int animLength = animationSystem->GetAnimationLength(ai.animation[i]);
				ai.animationSpeed[i] = animLength / duration;
				ai.looping[i] = flag & AnimationFlags::LOOP ? true : false;
				ai.blockBlending[i] = flag & AnimationFlags::BLOCKBLENDING ? true : false;
				ai.blendSpeed[i] = 0.0f;
				ai.timePos[i] = 0.0f;
				ai.blendFactor[i] = 1.0f;
			}

			ai.nrOfLayers = nrOfAnims;
			animationData.playing[entityIndex->second] = true;
		}

		else {



			if (flag & AnimationFlags::BLENDTOANDBACK) {

				ai.toBlendTarget = true;
				ai.blendBackInfo.animIndex = ai.nrOfLayers;
				ai.blendBackInfo.animLength = animationSystem->GetAnimationLength(GUIDTemporaryStorage[0]);

				flag |= AnimationFlags::BLENDTO;
			}

			if (flag & AnimationFlags::BLENDTO) {

				// Set info for animations to blend from
				for (size_t i = 0; i < ai.nrOfLayers; i++) {

					//ai.animationSpeed[i] = 0.0f;
					if (!(flag & AnimationFlags::BLENDTOANDBACK))
						ai.looping[i] = false;
						ai.blendSpeed[i] = -15.0f;
						ai.blendFactor[i] = 1.0f;
						ai.blendBackInfo.previousSpeed[i] = ai.animationSpeed[i];
						ai.animationSpeed[i] = 0.0f;
				}

				// Set info for the new animations to blend to
				for (size_t j = 0; j < nrOfAnims; j++) {

					if (ai.nrOfLayers + j >= maxLayers) {

						if (flag & AnimationFlags::FORCED) {

							OverwriteAnimation(ai, ai.nrOfLayers - 1, ai.nrOfLayers);
							ai.nrOfLayers--;

						}

						else {

							break;
						}
					}

					ai.animation[ai.nrOfLayers + j] = GUIDTemporaryStorage[j];
					unsigned int animLength = animationSystem->GetAnimationLength(ai.animation[j]);
					ai.animationSpeed[ai.nrOfLayers + j] = animLength / duration;
					ai.blendFactor[ai.nrOfLayers + j] = 0.0f;
					ai.blendSpeed[ai.nrOfLayers + j] = 15.0f;
					ai.looping[ai.nrOfLayers + j] = flag & AnimationFlags::LOOP ? true : false;
					ai.blockBlending[ai.nrOfLayers + j] = flag & AnimationFlags::BLOCKBLENDING ? true : false;
					ai.timePos[ai.nrOfLayers + j] = 0.0f;

				}

			}

			ai.nrOfLayers += nrOfAnims;
			animationData.playing[entityIndex->second] = true;
		}

	}

	ProfileReturnConst(true);

}

void SE::Core::AnimationManager::Start(const Entity & entity, const AnimationPlayInfo& playInfo)
{
	StartProfile;

	_ASSERT(playInfo.nrOfLayers <= AnimationPlayInfo::maxLayers);

	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		auto& ai = animationData.animInfo[entityIndex->second];
		ai.nrOfLayers = playInfo.nrOfLayers;

		for (size_t i = 0; i < playInfo.nrOfLayers; i++) {

			if (animationSystem->IsAnimationLoaded(playInfo.animations[i]))
			{
				ai.animation[i] = playInfo.animations[i];
				ai.animationSpeed[i] = playInfo.animationSpeed[i];
				ai.looping[i] = playInfo.looping[i];
				ai.blendSpeed[i] = playInfo.blendSpeed[i];
				ai.blockBlending[i] = playInfo.blockBlending[i];
				ai.timePos[i] = playInfo.timePos[i];

				animationData.playing[entityIndex->second] = 1u;
			}

			else
				initInfo.console->PrintChannel("Resources", "Tried to start an unloaded animation. GUID: %u.", playInfo.animations[i]);
		}


		StopProfile;
	}
}

void SE::Core::AnimationManager::SetSpeed(const Entity & entity, float speed)
{
	StartProfile;
	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		auto& ai = animationData.animInfo[entityIndex->second];

		for (size_t i = 0; i < ai.nrOfLayers; i++) {

			animationData.animInfo[entityIndex->second].animationSpeed[i] = speed;
		}
	}
	StopProfile;
}

void SE::Core::AnimationManager::SetBlendSpeed(const Entity& entity, int index, float speed) {

	StartProfile;

	auto dt = initInfo.window->GetDelta();

	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		auto& ai = animationData.animInfo[entityIndex->second];

		if (index == -1) {

			for (size_t i = 0; i < ai.nrOfLayers; i++) {

				ai.blendSpeed[i] = speed;
			}
		}

		else {

			if (index < maxLayers) {

				ai.blendSpeed[index] = speed;

			}
		}
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
		auto& ai = animationData.animInfo[entityIndex->second];

		for (size_t i = 0; i < ai.nrOfLayers; i++) {

			ai.timePos[i] = keyFrame;
			animationData.playing[entityIndex->second] = 0u;
			animationSystem->CalculateMatrices(animationData.entity[entityIndex->second], ai, false);

		}
	}
	StopProfile;
}

void SE::Core::AnimationManager::Start(const Entity & entity, bool looping)const
{
	StartProfile;
	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		if (renderableManager->IsVisible(entity))
		{
			auto& ai = animationData.animInfo[entityIndex->second];

			for(size_t i = 0; i < ai.nrOfLayers; i++){

				if (animationSystem->IsAnimationLoaded(ai.animation[i]))
				{

					ai.looping[i] = looping;
					animationData.playing[entityIndex->second] = 1u;
					
				}

			}
		}
	}
	StopProfile;
}

void SE::Core::AnimationManager::StopAllAnimations(const Entity & entity) const
{
	StartProfile;
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		animationData.playing[entityIndex->second] = 0u;
		animationData.animInfo[entityIndex->second].nrOfLayers = 0;
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
		animationData.playing[entityIndex->second] = 0u;
	}
	StopProfile;
}

void SE::Core::AnimationManager::UpdateBlending(const Entity& entity, int index) {

	StartProfile;

	auto dt = initInfo.window->GetDelta();

	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		auto& ai = animationData.animInfo[entityIndex->second];

		if (index == -1) {

			for (size_t i = 0; i < ai.nrOfLayers; i++) {

				ai.blendFactor[i] += ai.blendSpeed[i] * dt;
				ai.blendFactor[index] = max(0.0f, min(ai.blendFactor[index], 1.0f));

			}
		}

		else {

			if (index < maxLayers) {
					
				ai.blendFactor[index] += ai.blendSpeed[index] * dt;
				ai.blendFactor[index] = max(0.0f, min(ai.blendFactor[index], 1.0f));

			}
		}

	}
	StopProfile;
}

bool SE::Core::AnimationManager::IsAnimationPlaying(const Entity& entity, const Utilz::GUID animationToCheck) const
{
	StartProfile;
	
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		auto& ai = animationData.animInfo[entityIndex->second];
		for (size_t i = 0; i < ai.nrOfLayers; i++) {

			if (animationToCheck == ai.animation[i])
				ProfileReturnConst(true);

		}
	}


	ProfileReturnConst(false);
}

bool SE::Core::AnimationManager::IsAnimationPlaying(const Entity& entity, const Utilz::GUID* animationToCheck,
	size_t nrOfAnims) const
{
	StartProfile;

	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		auto& ai = animationData.animInfo[entityIndex->second];
		for (size_t j = 0; j < nrOfAnims; j++)
		{
			for (size_t i = 0; i < ai.nrOfLayers; i++) {

				if (animationToCheck[j] == ai.animation[i])
					ProfileReturnConst(true);

			}
		}
	}


	ProfileReturnConst(false);
}

bool SE::Core::AnimationManager::CurrentAnimationAllowsBlending(const Entity& entity) const
{
	StartProfile;

	// Get the entity register from the animationManager
	auto &entityIndex = entityToIndex.find(entity);
	if (entityIndex != entityToIndex.end())
	{
		auto& ai = animationData.animInfo[entityIndex->second];

		// If this animation layer doesn't allow blending, simply return. 
		// All layers must support blending for this. 
		for(int i = 0; i < ai.nrOfLayers; i++)
			if (ai.blockBlending[i]) {
				ProfileReturnConst(false);
			}
	}

	ProfileReturnConst(true);
}

void SE::Core::AnimationManager::ToggleVisible(const Entity & entity, bool visible)
{
	StartProfile;

	renderableManager->ToggleRenderableObject(entity, visible);
	
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
	newData.animInfo = reinterpret_cast<AnimationInfo*>(newData.entity + size);
	newData.playing = (uint8_t*)(newData.animInfo + size);
	newData.attacher = (Attacher*)(newData.playing + size);
	
	// Copy data
	memcpy(newData.entity, animationData.entity, animationData.used * sizeof(Entity));
	memcpy(newData.animInfo, animationData.animInfo, animationData.used * sizeof(AnimationInfo));
	memcpy(newData.playing, animationData.playing, animationData.used * sizeof(uint8_t));
	memcpy(newData.attacher, animationData.attacher, animationData.used * sizeof(Attacher));


	// Delete old data;
	operator delete(animationData.data);
	animationData = newData;

	StopProfile;
}

void SE::Core::AnimationManager::Destroy(size_t index)
{
	StartProfile;

	renderableManager->Destroy(index);

	// Temp variables
	size_t last = animationData.used - 1;
	const Entity entity = animationData.entity[index];
	const Entity last_entity = animationData.entity[last];

	// Copy the data
	animationData.entity[index] = last_entity;
	animationData.animInfo[index] = animationData.animInfo[last];
	animationData.playing[index] = animationData.playing[last];
	animationData.attacher[index] = animationData.attacher[last];

	// Replace the index for the last_entity 
	entityToIndex[last_entity] = index;
	entityToIndex.erase(entity);

	animationData.used--;
	StopProfile;
}

void SE::Core::AnimationManager::Destroy(const Entity & entity)
{
	const auto exists = entityToIndex.find(entity);
	if (exists != entityToIndex.end())
		Destroy(exists->second);
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

	return animationSystem->AddSkeleton(guid, jointAttr, skelH->nrOfJoints);
}

int SE::Core::AnimationManager::LoadAnimation(const Utilz::GUID& guid, void * data, size_t size)
{
	auto animH = (Animation_Header*)data;

	// After the animation header comes the animated joint indices
	auto joints = (uint32_t*)(animH + 1);

	// After the joint indices comes the keyframe matrices
	auto matrices = (DirectX::XMFLOAT4X4*)(joints + animH->nrOfJoints);

	return animationSystem->AddAnimation(guid, matrices, joints, animH->animationLength, animH->nrOfJoints);

	//return initInfo.renderer->CreateAnimation(matrices, animH->animationLength, animH->nrOfJoints);
}

void SE::Core::AnimationManager::CreateRenderObjectInfo(const Entity& entity, Graphics::RenderJob * info)
{

	StartProfile;
	const auto fe = entityToIndex.find(entity);
	if (fe != entityToIndex.end())
	{
		info->pipeline.IAStage.inputLayout = SkinnedVertexShader;
		info->pipeline.VSStage.shader = SkinnedVertexShader;
		info->maxInstances = 8;
		info->specialHaxxor = SkinnedOncePerObject;
	}
	StopProfile;
}

void SE::Core::AnimationManager::OverwriteAnimation(AnimationInfo & info, size_t to, size_t from)
{
	info.animation[to] = info.animation[from];
	info.animationSpeed[to] = info.animationSpeed[from];
	info.blendFactor[to] = info.blendFactor[from];
	info.blendSpeed[to] = info.blendSpeed[from];
	info.blockBlending[to] = info.blockBlending[from];
	info.looping[to] = info.looping[from];
	info.timePos[to] = info.timePos[from];

}
