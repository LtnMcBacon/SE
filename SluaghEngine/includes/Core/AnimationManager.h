#ifndef SE_CORE_ANIMATION_MANAGER_H_
#define SE_CORE_ANIMATION_MANAGER_H_
#include <Utilz\GUID.h>
#include "EntityManager.h"
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>
#include <unordered_map>
#include "RenderableManager.h"
#include <Graphics\FileHeaders.h>
#include <list>
#include <random>
#include <Utilz\CircularFiFo.h>

namespace SE
{
	namespace Core
	{
		enum class AnimationTrigger : uint32_t
		{
			Move
		};

		struct CreateAnimationInfo
		{
			Utilz::GUID skeleton;
			size_t animationCount;
			Utilz::GUID* animations;
		};

		class AnimationManager
		{
		public:
			AnimationManager(Graphics::IRenderer* renderer, ResourceHandler::IResourceHandler* resourceHandler, const EntityManager& entityManager, RenderableManager* renderableManager);
			~AnimationManager();

			void CreateAnimation(const Entity& entity, const CreateAnimationInfo& info);

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame();

			void Start(const Entity& entity, const Utilz::GUID& animation, float speed);
			void SetSpeed(const Entity& entity, float speed);
			void Start(const Entity& entity)const;
			void Pause(const Entity& entity)const;
			
		private:
			void SetRenderObjectInfo(const Entity& entity, Graphics::RenderObjectInfo* info);


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

			int LoadSkeleton(void*data, size_t size);
			int LoadAnimation(void * data, size_t size);
			int LoadSkinnedShader(const Utilz::GUID& guid, void* data, size_t size);
			int skinnedShader;


			Graphics::IRenderer* renderer;
			ResourceHandler::IResourceHandler* resourceHandler;
			const EntityManager& entityManager;
			RenderableManager* renderableManager;
			std::default_random_engine generator;

			struct AnimationData
			{
				static const size_t size = sizeof(Entity) + sizeof(size_t) + sizeof(int);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				size_t* skeletonIndex;
				int* job;
			};
			
			AnimationData animationData;
			std::unordered_map <Entity, size_t, EntityHasher> entityToIndex;


			std::map<Utilz::GUID, int, Utilz::GUID::Compare> guidToSkeletonIndex;
			std::map<Utilz::GUID, int, Utilz::GUID::Compare> guidToSkelAnimationIndex;
		};
	}
}
#endif