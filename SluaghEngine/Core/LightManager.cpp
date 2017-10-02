#include <Core\LightManager.h>
#include <Profiler.h>
#include <list>

namespace SE {
	namespace Core {
		LightManager::LightManager(Graphics::IRenderer * renderer, const EntityManager & entityManager, TransformManager* transformManager)
			:renderer(renderer), entityManager(entityManager), transformManager(transformManager)
		{
			_ASSERT(renderer);
			_ASSERT(transformManager);
		}

		LightManager::~LightManager()
		{
		}
		
		int LightManager::AddLight(Entity entity, Graphics::LightData data)
		{
			StartProfile;
			// Check if the entity is alive
			if (!entityManager.Alive(entity))
				ProfileReturnConst(-1);

			// chexk if entity exist in text 
			auto fileLoaded = entID.find(entity);
			if (fileLoaded == entID.end())
			{
				entID[entity].ID = lights.size();
				lights.push_back(data);
				DirectX::XMFLOAT3 tempPos = transformManager->GetPosition(entity);
				lights[lights.size() - 1].pos = DirectX::XMFLOAT4(tempPos.x, tempPos.y, tempPos.z, data.pos.w);
				ent.push_back(entity);
				ProfileReturnConst(0);
			}
			ProfileReturnConst(-1);
		}

		int LightManager::RemoveLight(Entity entity)
		{
			StartProfile;
			// Check if the entity is alive
			if (!entityManager.Alive(entity))
				ProfileReturnConst(-1);

			// chexk if entity exist in text 
			auto fileLoaded = entID.find(entity);
			if (fileLoaded != entID.end())
			{
				renderer->DisableLightRendering(lights[entID[entity].ID]);
				Destroy(entID[entity].ID);
				ProfileReturnConst(0);
			}
			ProfileReturnConst(-1);
		}

		int LightManager::ToggleLight(Entity entity, bool show)
		{
			StartProfile;
			// Check if the entity is alive
			if (!entityManager.Alive(entity))
				ProfileReturnConst(-1);

			// chexk if entity exist in text 
			auto fileLoaded = entID.find(entity);
			if (fileLoaded != entID.end())
			{
				if (show && !entID[entity].show)
				{
					renderer->EnableLightRendering(lights[entID[entity].ID]);
					entID[entity].show = true;
				}
				else if (!show && entID[entity].show)
				{
					renderer->DisableLightRendering(lights[entID[entity].ID]);
					entID[entity].show = false;
				}
				ProfileReturnConst(0);
			}
			ProfileReturnConst(-1);
		}

		void LightManager::Frame()
		{
			GarbageCollection();
		}

		void LightManager::GarbageCollection()
		{
			StartProfile;
			uint32_t alive_in_row = 0;

			while (lights.size() > 0 && alive_in_row < 4U)
			{
				std::uniform_int_distribution<uint32_t> distribution(0U, lights.size() - 1U);
				uint32_t i = distribution(generator);
				if (entityManager.Alive(ent[i]))
				{
					alive_in_row++;
					continue;
				}
				alive_in_row = 0;
				renderer->DisableLightRendering(lights[entID[ent[i]].ID]);
				Destroy(i);
			}
			StopProfile;
		}

		void LightManager::Destroy(size_t index)
		{
			StartProfile;
			// Temp variables
			size_t last = lights.size() - 1;
			const Entity& entity = ent[index];
			const Entity& last_entity = ent[last];

			// Copy the data
			ent[index] = last_entity;
			lights[index] = lights[last];
			entID[last_entity] = entID[entity];

			// Remove last spot 
			entID.erase(entity);
			lights.pop_back();
			ent.pop_back();

			StopProfile;
		}
	}
}