#include"LightManager.h"
#include <Profiler.h>


namespace SE {
	namespace Core {

		LightManager::LightManager(const InitializationInfo & initInfo) :initInfo(initInfo)
		{
			_ASSERT(initInfo.renderer);
			_ASSERT(initInfo.entityManager);
			_ASSERT(initInfo.transformManager);

			initInfo.transformManager->RegisterSetDirty({ this, &LightManager::UpdateDirtyPos });

		/*	auto result = initInfo.renderer->GetPipelineHandler()->CreateConstantBuffer("LightDataBuffer", sizeof(float) * 4 + sizeof(float) * 20 * 2 * 4);
			if (result < 0)
				throw std::exception("Could not create LightDataBuffer");*/
		}

		LightManager::~LightManager()
		{
		}

		void LightManager::Create(const Entity & entity, const CreateInfo & data)
		{
			StartProfile;
			// Check if the entity is alive
			if (!initInfo.entityManager->Alive(entity))
				ProfileReturnVoid;

			// chexk if entity exist in text 
			auto& fileLoaded = entID.find(entity);
			if (fileLoaded == entID.end())
			{
				entID[entity].ID = lights.size();
				Graphics::LightData d;
				d.colour = { data.color.x, data.color.y, data.color.z, 1.0f };
				d.pos = { data.pos.x, data.pos.y, data.pos.z, data.radius };
				lights.push_back(d);

				initInfo.transformManager->Create(entity, data.pos);

				ent.push_back(entity);

			}
			ProfileReturnVoid;
		}

		void LightManager::ToggleLight(const Entity & entity, bool show)
		{
			StartProfile;
			// Check if the entity is alive
			if (!initInfo.entityManager->Alive(entity))
				ProfileReturnVoid;

			// chexk if entity exist in text 
			auto fileLoaded = entID.find(entity);
			if (fileLoaded != entID.end())
			{
				if (show && !fileLoaded->second.show)
				{
					DirectX::XMFLOAT3 tempPos = initInfo.transformManager->GetPosition(entity);
					lights[lights.size() - 1].pos = DirectX::XMFLOAT4(tempPos.x, tempPos.y, tempPos.z, lights[lights.size() - 1].pos.w);
					fileLoaded->second.jobID = initInfo.renderer->EnableLightRendering(lights[fileLoaded->second.ID]);
					jobToEnt[fileLoaded->second.jobID] = entity;
					fileLoaded->second.show = true;

				}
				else if (!show && fileLoaded->second.show)
				{
					size_t tempJobID = initInfo.renderer->DisableLightRendering(fileLoaded->second.jobID);
					fileLoaded->second.show = false;
					entID[jobToEnt[tempJobID]].jobID = fileLoaded->second.jobID;
				}

			}
			ProfileReturnVoid;
		}

		void LightManager::Frame(Utilz::TimeCluster * timer)
		{
			StartProfile;
			timer->Start(CREATE_ID_HASH("LightManger"));
			GarbageCollection();

			struct LightDataBuffer
			{
				DirectX::XMFLOAT4 size;
				Graphics::LightData data[20];
			};
			LightDataBuffer data;
			data.size = DirectX::XMFLOAT4(1.0f, 0.0f,0.0f,0.0);
			data.data[0].pos = DirectX::XMFLOAT4(0.25f, 1.0f, -1.0f, 100.0f);
			data.data[0].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("LightDataBuffer", &data, sizeof(LightDataBuffer));
			initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("CameraPos", &data.data[0].pos, sizeof(DirectX::XMFLOAT4));


			timer->Stop(CREATE_ID_HASH("LightManger"));
			StopProfile;
		}
		


		void LightManager::GarbageCollection()
		{
			StartProfile;
			uint32_t alive_in_row = 0;

			while (lights.size() > 0 && alive_in_row < 4U)
			{
				std::uniform_int_distribution<uint32_t> distribution(0U, lights.size() - 1U);
				uint32_t i = distribution(generator);
				if (initInfo.entityManager->Alive(ent[i]))
				{
					alive_in_row++;
					continue;
				}
				alive_in_row = 0;
				size_t tempJobID = initInfo.renderer->DisableLightRendering(entID[ent[i]].jobID);
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
			const Entity entity = ent[index];
			const Entity last_entity = ent[last];

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

		void LightManager::Destroy(const Entity & entity)
		{
		}

		void LightManager::UpdateDirtyPos(const Entity& entity, size_t index)
		{
			StartProfile;
			auto find = entID.find(entity);
			if (find != entID.end() && entID[entity].show == true)
			{
				initInfo.renderer->UpdateLightPos(initInfo.transformManager->GetPosition(entity), find->second.jobID);
			}
			ProfileReturnVoid;
		}
	}
}