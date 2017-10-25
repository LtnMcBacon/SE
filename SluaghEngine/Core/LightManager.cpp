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

			uint32_t numLights = 0;
			auto result = initInfo.renderer->GetPipelineHandler()->CreateConstantBuffer("LightDataBuffer", sizeof(LightDataBuffer));
			if (result < 0)
				throw std::exception("Could not create LightDataBuffer");
		}

		LightManager::~LightManager()
		{
		}

		void LightManager::Create(const Entity & entity, const CreateInfo & info)
		{
			StartProfile;
			// Check if the entity is alive
			if (!initInfo.entityManager->Alive(entity))
				ProfileReturnVoid;

			// chexk if entity exist
			auto& fileLoaded = entityToLightData.find(entity);
			if (fileLoaded == entityToLightData.end())
			{
				LightData data;
				data.visible = false;
				data.colour = { info.color.x, info.color.y, info.color.z, 1.0f };
				data.pos = { info.pos.x, info.pos.y, info.pos.z, info.radius };

				initInfo.transformManager->Create(entity, info.pos);

				indexToEntity.push_back(entity);

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
			auto fileLoaded = entityToLightData.find(entity);
			if (fileLoaded != entityToLightData.end())
			{
				fileLoaded->second.visible = show;
				if (show && !fileLoaded->second.visible)
				{
					anyTogglesThisFrame = true;
					//DirectX::XMFLOAT3 tempPos = initInfo.transformManager->GetPosition(entity);
					//lights[lights.size() - 1].pos = DirectX::XMFLOAT4(tempPos.x, tempPos.y, tempPos.z, lights[lights.size() - 1].pos.w);
					//fileLoaded->second.jobID = initInfo.renderer->EnableLightRendering(lights[fileLoaded->second.ID]);
					//jobToEnt[fileLoaded->second.jobID] = entity;
					
					/*initInfo.renderer->GetPipelineHandler()->MapConstantBuffer("LightDataBuffer", [this](auto data) {
						auto cb = *(LightDataBuffer*)data;
						uint32_t count = 0;
						for (auto& l : entityToLightData)
						{
							if (l.second.visible)
							{
								cb.data[count].colour = l.second.colour;
								cb.data[count].pos = l.second.pos;
								count++;
							}
							if (count == 20)
								break;
						}
						cb.size[0] = count;

					});*/

					

				}
				else if (!show && fileLoaded->second.visible)
				{
					anyTogglesThisFrame = true;
					/*size_t tempJobID = initInfo.renderer->DisableLightRendering(fileLoaded->second.jobID);
					fileLoaded->second.show = false;
					entID[jobToEnt[tempJobID]].jobID = fileLoaded->second.jobID;*/
				}

			}
			ProfileReturnVoid;
		}

		void LightManager::Frame(Utilz::TimeCluster * timer)
		{
			StartProfile;
			timer->Start(CREATE_ID_HASH("LightManger"));
			GarbageCollection();

		/*	struct LightDataBuffer
			{
				DirectX::XMFLOAT4 size;
				Graphics::LightData data[20];
			};
			LightDataBuffer data;
			data.size = DirectX::XMFLOAT4(1.0f, 0.0f,0.0f,0.0);
			data.data[0].pos = DirectX::XMFLOAT4(0.25f, 1.0f, -1.0f, 100.0f);
			data.data[0].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("LightDataBuffer", &data, sizeof(LightDataBuffer));
			initInfo.renderer->GetPipelineHandler()->UpdateConstantBuffer("CameraPos", &data.data[0].pos, sizeof(DirectX::XMFLOAT4));*/


			if (anyTogglesThisFrame)
			{
				initInfo.renderer->GetPipelineHandler()->MapConstantBuffer("LightDataBuffer", [this](auto data) {
					auto cb = *(LightDataBuffer*)data;
					uint32_t count = 0;
					for (auto& l : entityToLightData)
					{
						if (l.second.visible)
						{
							cb.data[count].colour = l.second.colour;
							cb.data[count].pos = l.second.pos;
							count++;
						}
						if (count == 20)
							break;
					}
					cb.size[0] = count;

				});
				anyTogglesThisFrame = false;
			}


			timer->Stop(CREATE_ID_HASH("LightManger"));
			StopProfile;
		}
		


		void LightManager::GarbageCollection()
		{
			StartProfile;
			uint32_t alive_in_row = 0;

			while (entityToLightData.size() > 0 && alive_in_row < 4U)
			{
				std::uniform_int_distribution<uint32_t> distribution(0U, entityToLightData.size() - 1U);
				uint32_t i = distribution(generator);
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

		void LightManager::Destroy(size_t index)
		{
			StartProfile;
			// Temp variables
			size_t last = entityToLightData.size() - 1;
			const Entity entity = indexToEntity[index];
			const Entity last_entity = indexToEntity[last];

			// Copy the data
			indexToEntity[index] = last_entity;
			entityToLightData[last_entity] = entityToLightData[entity];

			// Remove last spot 
			entityToLightData.erase(entity);
			indexToEntity.pop_back();

			StopProfile;
		}

		void LightManager::Destroy(const Entity & entity)
		{
		}

		void LightManager::UpdateDirtyPos(const Entity& entity, size_t index)
		{
			StartProfile;
			auto find = entityToLightData.find(entity);
			if (find != entityToLightData.end())
			{
				dirtyEntites.push_back({ index, entity });
			//	initInfo.renderer->UpdateLightPos(initInfo.transformManager->GetPosition(entity), find->second.jobID);
			}
			ProfileReturnVoid;
		}
	}
}