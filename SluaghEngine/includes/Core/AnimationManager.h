#ifndef SE_CORE_ANIMATION_MANAGER_H_
#define SE_CORE_ANIMATION_MANAGER_H_
#include <Utilz\GUID.h>
#include "EntityManager.h"
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>
#include <unordered_map>
#include <Graphics\FileHeaders.h>

#include <random>

namespace SE
{
	namespace Core
	{
		enum class AnimationTrigger : uint32_t
		{
			Move
		};
		class AnimationManager
		{
			// We make the animation manager a friend to the RenderableManager so we can reach private data from this manager in RenderableManager
			friend class RenderableManager;
		public:
			AnimationManager(Graphics::IRenderer* renderer, ResourceHandler::IResourceHandler* resourceHandler, const EntityManager& entityManager);
			~AnimationManager();

			void CreateSkeleton(const Entity& entity, const Utilz::GUID& skeleton);
			void AddAnimation(const Entity& entity, const Utilz::GUID& animation);

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame();
		private:
			/**
			* @brief	Allocate more memory
			*/
			void Allocate(size_t size);
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index);
			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection();

			int LoadSkeleton(const Utilz::GUID& skeleton, void*data, size_t size);
			int LoadAnimation(const Utilz::GUID& animation, void * data, size_t size);

			Graphics::IRenderer* renderer;
			ResourceHandler::IResourceHandler* resourceHandler;
			const EntityManager& entityManager;
			std::default_random_engine generator;

			struct AnimationData
			{
				static const size_t size = sizeof(Entity);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				size_t* skeletonIndex;
			};
			
			AnimationData animationData;
			std::unordered_map <Entity, size_t, EntityHasher> entityToIndex;

			std::vector<int> skeletonHandle;
			std::map<Utilz::GUID, size_t, Utilz::GUID::Compare> guidToSkeletonIndex;


		};
	}
}
#endif