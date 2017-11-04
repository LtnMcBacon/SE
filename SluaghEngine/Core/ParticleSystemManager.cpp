#include "ParticleSystemManager.h"
#include <Profiler.h>


SE::Core::ParticleSystemManager::ParticleSystemManager(const InitializationInfo& initInfo) : initInfo(initInfo)
{
	_ASSERT(initInfo.renderer);
	_ASSERT(initInfo.resourceHandler);
	_ASSERT(initInfo.entityManager);
	_ASSERT(initInfo.transformManager);
	_ASSERT(initInfo.renderableManager);
	_ASSERT(initInfo.console);
}


SE::Core::ParticleSystemManager::~ParticleSystemManager()
{
}

void SE::Core::ParticleSystemManager::CreateSystem(const Entity & entity, const CreateInfo & info)
{
	StartProfile;
	auto find = entityToIndex.find(entity);
	if (find == entityToIndex.end()) // The entity had no system
	{
		/*Register the entity*/
		auto newEntry = particleSystemData.size();
		indexToEntity[newEntry] = entity;

		// Register instant destroy
		initInfo.entityManager->RegisterDestroyCallback(entity, { this, &ParticleSystemManager::Destroy });

		particleSystemData[newEntry].visible = 0u;
		particleSystemData[newEntry].loaded = 0u;

		// Load the particle system file
		{
			auto res = initInfo.resourceHandler->LoadResource(info.systemFile, [entity, newEntry, this](auto guid, auto data, auto size) {
			
				//if (async)
				//{
				//	toUpdateStruct tu = { entity };
				//	memcpy(&tu.info, data, sizeof(tu.info));
				//	toUpdate.push(tu);
				//}
				//else
				//{
				//	memcpy(&particleSystemData[newEntry].particleFileInfo, data, sizeof(particleSystemData[newEntry].particleFileInfo));
				//}

				return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
			});

			if (res)
				initInfo.console->PrintChannel("Resources", "Could not load particle system file. GUID: %u, Error: %d",  info.systemFile, res);
		}

		particleSystemData.push_back({});
	}

	StopProfile;
}

void SE::Core::ParticleSystemManager::ToggleVisible(const Entity & entity, bool visible)
{
	StartProfile;
	auto find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
	{
		//If the visibility state is switched to what it already is we dont do anything.
		if ((bool)particleSystemData[find->second].visible == visible)
		{
			ProfileReturnVoid;
		}
		if (particleSystemData[find->second].loaded == 1u) // Only show the system if it has been loaded. Otherwise wait.
		{
			// Tell renderer.
			if (visible)
			{

			}
			else
			{

			}
		}

		particleSystemData[find->second].visible = visible ? 1u : 0u;
		
		
	}
	StopProfile;
}

void SE::Core::ParticleSystemManager::Frame(Utilz::TimeCluster * timer)
{
	StartProfile;
	timer->Start(("ParticleSystemManager"));
	GarbageCollection();

	while (!toUpdate.wasEmpty())
	{
		auto& top = toUpdate.top();

		auto find = entityToIndex.find(top.entity);
		if (find != entityToIndex.end())
		{
			particleSystemData[find->second].particleFileInfo = top.info;
			particleSystemData[find->second].loaded = 1u;
			if (particleSystemData[find->second].visible == 1u)
			{
				// Tell renderer
			}
		}

		toUpdate.pop();

	}
	timer->Stop(("ParticleSystemManager"));
	StopProfile;
}

void SE::Core::ParticleSystemManager::Destroy(size_t index)
{
	StartProfile;
	size_t last = particleSystemData.size() - 1;
	const Entity entity = indexToEntity[index];
	const Entity last_entity = indexToEntity[last];

	if(particleSystemData[index].visible == 1u)
		ToggleVisible(entity, false);

	// Copy the data
	indexToEntity[index] = last_entity;
	particleSystemData[index].visible = particleSystemData[last].visible;


	// Replace the index for the last_entity 
	entityToIndex[last_entity] = index;
	entityToIndex.erase(entity);

	particleSystemData.pop_back();
	StopProfile;
}

void SE::Core::ParticleSystemManager::Destroy(const Entity & entity)
{
	auto find = entityToIndex.find(entity);
	if (find != entityToIndex.end())
		Destroy(find->second);
}

void SE::Core::ParticleSystemManager::GarbageCollection()
{
	StartProfile;
	uint8_t alive_in_row = 0;
	while (particleSystemData.size() > 0 && alive_in_row < 4U)
	{
		std::uniform_int_distribution<size_t> distribution(0U, particleSystemData.size() - 1U);
		size_t i = distribution(generator);
		if (initInfo.entityManager->Alive(indexToEntity[i]))
		{
			alive_in_row++;
			continue;
		}
		alive_in_row = 0;
		Destroy(i);
	}
	StopProfile;
}
