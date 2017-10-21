#pragma once
#ifndef SE_CORE_LIGHT_MANAGER_H_
#define SE_CORE_LIGHT_MANAGER_H_

#include <ILightManager.h>
#include <unordered_map>
#include <stack>
#include <vector>
#include <random>
#include <Graphics\LightInfo.h>



namespace SE
{
	namespace Core
	{
		class LightManager : public ILightManager
		{
		public:
			LightManager(const InitializationInfo& initInfo);
			~LightManager();		

			void Create(const Entity& entity, const CreateInfo& data)override;

			void ToggleLight(const Entity& entity, bool show)override;

			void Frame(Utilz::TimeCluster* timer)override;
		private:
			void GarbageCollection()override;
			void Destroy(size_t index)override;
			void Destroy(const Entity& entity)override;
			void UpdateDirtyPos(const Entity& entity, size_t index);
			struct showID
			{
				size_t ID;
				size_t jobID;
				bool show = false;
			};

			// Light variables
			std::unordered_map<Entity, showID, EntityHasher> entID;
			std::vector<Graphics::LightData> lights;
			std::vector<Entity> ent;
			std::map<size_t, Entity> jobToEnt;

			std::default_random_engine generator;

			InitializationInfo initInfo;
		};
	}
}
#endif