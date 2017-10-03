#pragma once
#ifndef SE_CORE_LIGHT_MANAGER_H_
#define SE_CORE_LIGHT_MANAGER_H_
#include "EntityManager.h"
#include <unordered_map>
#include <stack>
#include <vector>
#include <Graphics\IRenderer.h>
#include <random>
#include <Graphics\LightInfo.h>
#include "TransformManager.h"


namespace SE
{
	namespace Core
	{
		/**
		*
		* @brief This class is used to bind renderable text and texture to an entity.
		*
		* @details Use this class when you want an entity to be rendered as a text or texture in the GUI.
		*
		**/
		class LightManager
		{
		public:
			LightManager(Graphics::IRenderer* renderer, const EntityManager& entityManager, TransformManager* transformManager);
			~LightManager();		

			int AddLight(Entity entity, Graphics::LightData data);

			int RemoveLight(Entity entity);

			int ToggleLight(Entity entity, bool show);

			void Frame();
		private:
			void GarbageCollection();
			void Destroy(size_t index);
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

			Graphics::IRenderer* renderer;
			const EntityManager& entityManager;
			TransformManager* transformManager;
		};
	}
}
#endif