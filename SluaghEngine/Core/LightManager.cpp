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

			transformManager->SetDirty.Add<LightManager, &LightManager::UpdateDirtyPos>(this);
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
			auto& fileLoaded = entID.find(entity);
			if (fileLoaded == entID.end())
			{
				entID[entity].ID = lights.size();
				lights.push_back(data);			
				lights[lights.size() - 1].pos = DirectX::XMFLOAT4(0.0, 0.0, 0.0, data.pos.w);
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
				size_t tempJobID = renderer->DisableLightRendering(fileLoaded->second.jobID);
				entID[jobToEnt[tempJobID]].jobID = fileLoaded->second.jobID;
				Destroy(fileLoaded->second.ID);
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
				if (show && !fileLoaded->second.show)
				{
					DirectX::XMFLOAT3 tempPos = transformManager->GetPosition(entity);
					lights[lights.size() - 1].pos = DirectX::XMFLOAT4(tempPos.x, tempPos.y, tempPos.z, lights[lights.size() - 1].pos.w);
					fileLoaded->second.jobID = renderer->EnableLightRendering(lights[fileLoaded->second.ID]);
					jobToEnt[fileLoaded->second.jobID] = entity;
					fileLoaded->second.show = true;
					
				}
				else if (!show && fileLoaded->second.show)
				{
					size_t tempJobID = renderer->DisableLightRendering(fileLoaded->second.jobID);
					fileLoaded->second.show = false;
					entID[jobToEnt[tempJobID]].jobID = fileLoaded->second.jobID;
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
				size_t tempJobID = renderer->DisableLightRendering(entID[ent[i]].jobID);
				entID[jobToEnt[tempJobID]].jobID = entID[ent[i]].jobID;
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

		void LightManager::UpdateDirtyPos(const Entity& entity, size_t index)
		{
			StartProfile;
			auto find = entID.find(entity);
			if (find != entID.end() && entID[entity].show == true)
			{
				renderer->UpdateLightPos(transformManager->GetPosition(entity), find->second.jobID);
			}
			ProfileReturnVoid;
		}
	}
}