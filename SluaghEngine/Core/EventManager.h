#ifndef SE_CORE_EVENT_MANAGER_H_
#define SE_CORE_EVENT_MANAGER_H_
#include <IEventManager.h>
#include <Utilz\Event.h>
#include <unordered_map>
#include <random>

namespace SE
{
	namespace Core
	{
		class EventManager : public IEventManager
		{
		public:
			EventManager(const IEventManager::InitializationInfo& info);
			~EventManager();


			void RegisterEventCallback(const Utilz::GUID _event,
				const std::function<void(const Entity, const std::vector<void*>& args)> _eventTriggerCallback,
				const std::function<bool(const Entity, std::vector<void*>& args)> _eventTriggerCheck)override;

			void SetLifetime(const Entity entity, float lifetime)override;

			void RegisterToSetRenderObjectInfo(const Utilz::Delegate<void(const Entity& entity, SE::Graphics::RenderJob* info)>&& callback) override
			{
				SetRenderObjectInfo += callback;
			}
			void TriggerSetRenderObjectInfo(const Entity& entity, SE::Graphics::RenderJob* info) override
			{
				SetRenderObjectInfo(entity, info);
			}

			void RegisterToSetDirty(const Utilz::Delegate<void(const Entity& entity, size_t index)>&& callback) override
			{
				SetDirty += callback;
			}
			void TriggerSetDirty(const Entity& entity, size_t index) override
			{
				SetDirty(entity, index);
			}

			void RegisterToUpdateRenderableObject(const Utilz::Delegate<void(const Entity&)>&& callback) override
			{
				UpdateRenderableObject += callback;
			}
			void TriggerUpdateRenderableObject(const Entity& entity) override
			{
				UpdateRenderableObject(entity);
			}
			void RegisterToToggleVisible(const Utilz::Delegate<void(const Entity&, bool)> && callback)
			{
				ToggleVisibleEvent += callback;
			}
			void ToggleVisible(const Entity& entity, bool visible)
			{
				ToggleVisibleEvent(entity, visible);
			}

			void Frame(Utilz::TimeCluster* timer) override;

		private:
			Utilz::Event<void(const Entity& entity, SE::Graphics::RenderJob* info)> SetRenderObjectInfo;
			Utilz::Event<void(const Entity& entity, size_t index)> SetDirty;
			Utilz::Event<void(const Entity& entity)> UpdateRenderableObject;
			Utilz::Event<void(const Entity&, bool)> ToggleVisibleEvent;

			/**
			* @brief	Allocate more memory
			*/
			void Allocate(size_t size);
			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index) override;
			/**
			* @brief	Remove an enitity
			*/
			void Destroy(const Entity& entity) override;
			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection() override;
			struct Events
			{
				static const uint8_t MAX = 8;
				uint8_t nrOfEvents;
				Utilz::GUID event_[MAX];

			};
			struct EventData
			{
				static const size_t size = sizeof(Entity) + sizeof(Events);
				size_t allocated = 0;
				size_t used = 0;
				void* data = nullptr;
				Entity* entity;
				Events* events;
			};

			EventData eventData;
			std::unordered_map<Entity, size_t, EntityHasher> entityToIndex;

			IEventManager::InitializationInfo initInfo;
			std::default_random_engine generator;

		};
	}
}

#endif