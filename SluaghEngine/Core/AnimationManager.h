#ifndef SE_CORE_ANIMATION_MANAGER_H_
#define SE_CORE_ANIMATION_MANAGER_H_

#include <IAnimationManager.h>
#include <unordered_map>
#include <Graphics\FileHeaders.h>
#include <list>
#include <random>
#include <Utilz\CircularFiFo.h>
#include "RenderableManager.h"
#include "AnimationSystem.h"
#include <stack>

namespace SE
{
	namespace Core
	{
		enum class AnimationTrigger : uint32_t
		{
			Move
		};


		class AnimationManager : public IAnimationManager
		{
		public:
			AnimationManager(const IAnimationManager::InitializationInfo& initInfo);
			~AnimationManager();

			void CreateAnimatedObject(const Entity& entity, const IAnimationManager::CreateInfo& info)override;

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame(Utilz::TimeCluster* timer)override;

			void AttachToEntity(const Entity& source, const Entity& entityToAttach, const Utilz::GUID& jointGUID, int slotIndex)override;

			void Start(const Entity& entity, const Utilz::GUID* animations, size_t nrOfAnims, float duration, AnimationFlags flag)override;
			void Start(const Entity& entity, const AnimationPlayInfo& playInfo)override;
			void Start(const Entity& entity, bool looping)const override;
			void SetSpeed(const Entity& entity, float speed)override;
			void SetKeyFrame(const Entity& entity, float keyFrame)override;
			void SetBlendSpeed(const Entity& entity, int index, float speed)override;
			void Pause(const Entity& entity)const override;
			bool IsAnimationPlaying(const Entity& entity, const Utilz::GUID animationToCheck) const override;
			void UpdateBlending(const Entity& entity, int index)override;
			
			void ToggleVisible(const Entity& entity, bool visible)override;

		private:
			/**
			* @brief	Allocate more memory
			*/
			void Allocate(size_t size);
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index)override;
			/**
			* @brief	Remove an enitity
			*/
			void Destroy(const Entity& entity)override;
			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection()override;

			int LoadSkeleton(const Utilz::GUID& guid, void*data, size_t size);
			int LoadAnimation(const Utilz::GUID& guid, void * data, size_t size);
			
			void CreateRenderObjectInfo(const Entity& entity, Graphics::RenderJob * info);

			void OverwriteAnimation(AnimationInfo& info, size_t to, size_t from);

			IAnimationManager::InitializationInfo initInfo;
			std::default_random_engine generator;

			RenderableManager* renderableManager;

			struct AttacherSlot {

				bool attached = false;
				Entity entity;
				int jointIndex = -1;
			};

			struct Attacher {

				static const size_t maxSlots = 2;
				AttacherSlot slots[maxSlots];
			};

			struct AnimationData
			{
				static const size_t size = sizeof(Entity) + sizeof(AnimationInfo) + sizeof(uint8_t) + sizeof(Attacher);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				AnimationInfo* animInfo;	
				uint8_t* playing;
				Attacher* attacher;

			};
			AnimationData animationData;
			std::unordered_map <Entity, size_t, EntityHasher> entityToIndex;

			struct updateInfo
			{
				Entity ent;
				AnimationInfo& animInfo;
			};

			std::vector<updateInfo> updateJob;
			AnimationSystem* animationSystem;

			float aniUpdateTime = 0.0;

		};
	}
}
#endif