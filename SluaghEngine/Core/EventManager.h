#ifndef SE_CORE_EVENT_MANAGER_H_
#define SE_CORE_EVENT_MANAGER_H_
#include <IEventManager.h>
#include <Utilz\Event.h>
#include <unordered_map>
#include <random>
#include <vector>


namespace SE
{
	namespace Core
	{
		class EventManager : public IEventManager
		{
		public:
			EventManager(const IEventManager::InitializationInfo& info);
			~EventManager();


			/**
			* @brief	Register a new entity event
			* @detail Entity events run the trigger check once per entity registered to it.
			*
			* @param[in] _event The event identifier.
			* @param[in] callbacks The callbacks used to check if the event is trigger and to trigger the event.
			*
			*/
			void RegisterEntityEvent(const Utilz::GUID _event,
				const EntityEventCallbacks& callbacks)override;

			/**
			* @brief	Register a new trigger event
			* @detail Trigger events are triggered from the outside, by the user.
			*
			* @param[in] _event The event identifier.
			* @param[in] callback The callback used when the event has been triggered.
			*
			*/
			void RegisterTriggerEvent(const Utilz::GUID _event,
				const std::function<void(const Entity)>& callback) override;

			/**
			* @brief	Trigger a trigger event
			* @detail Trigger events usually are buffered until the event managers frame
			*
			* @param[in] _event The event identifier.
			* @param[in] now If the event should fire immediatly
			*
			*/
			void TriggerEvent(const Utilz::GUID _event, bool now) override;
		
			void RegisterEntitytoEvent(const Entity entity, const Utilz::GUID _event) override;
			
			/**
			* @brief	UnRegister an event to an entity
			*
			* @param[in] entity The entity to unbind the event from.
			* @param[in] _event The event identifier.
			*
			*/
			void UnregisterEntitytoEvent(const Entity entity, const Utilz::GUID _event) override;

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
			void RegisterToToggleVisible(const Utilz::Delegate<void(const Entity&, bool)> && callback) override
			{
				ToggleVisibleEvent += callback;
			}
			void ToggleVisible(const Entity& entity, bool visible) override
			{
				ToggleVisibleEvent(entity, visible);
			}
			void RegisterToToggleShadow(const Utilz::Delegate<void(const Entity&, bool)>&& callback) override
			{
				ToggleShadowEvent += callback;
			}
			void ToggleShadow(const Entity& entity, bool on) override
			{
				ToggleShadowEvent(entity, on);
			}

			void RegisterToSetShadowRenderObjectInfo(const Utilz::Delegate<void(const Entity& entity, SE::Graphics::RenderJob* info)>&& callback) override
			{
				SetShadowRenderObjectInfo += callback;
			}

			void TriggerSetShadowRenderObjectInfo(const Entity& entity, SE::Graphics::RenderJob* info) override
			{
				SetShadowRenderObjectInfo(entity, info);
			}

			void Frame(Utilz::TimeCluster* timer) override;
			/**
			* @brief	Remove an enitity
			*/
			void Destroy(const Entity& entity) override;
		private:
			Utilz::Event<void(const Entity& entity, SE::Graphics::RenderJob* info)> SetRenderObjectInfo;
			Utilz::Event<void(const Entity& entity, SE::Graphics::RenderJob* info)> SetShadowRenderObjectInfo;
			Utilz::Event<void(const Entity& entity, size_t index)> SetDirty;
			Utilz::Event<void(const Entity& entity)> UpdateRenderableObject;
			Utilz::Event<void(const Entity&, bool)> ToggleVisibleEvent;
			Utilz::Event<void(const Entity&, bool)> ToggleShadowEvent;

			/**
			* @brief	Remove an enitity entry
			*/
			void Destroy(size_t index) override;

			/**
			* @brief	Look for dead entities.
			*/
			void GarbageCollection() override;
			
			struct EntityEntry
			{
				std::vector<Entity> entity;
				std::vector<std::vector<Utilz::GUID>> eventsRegisteredTo;
			};
			EntityEntry entires;
			std::unordered_map<Entity, size_t, EntityHasher> entityToIndex;

			IEventManager::InitializationInfo initInfo;
			std::default_random_engine generator;

	
			struct EntityEvent
			{
				Utilz::GUID event_;
				EntityEventCallbacks callbacks;
				std::vector<Entity> entitesRegistered;
			};
			std::vector<EntityEvent> entityEvents;
			std::unordered_map<Utilz::GUID, size_t, Utilz::GUID::Hasher> entityEventToIndex;

			struct TriggerEventStruct
			{
				Utilz::GUID event_;
				std::function<void(const Entity)> callback;
				std::vector<Entity> entitesRegistered;
			};
			std::vector<TriggerEventStruct> triggerEvents;
			std::vector<size_t> eventsToTrigger;
			std::unordered_map<Utilz::GUID, size_t, Utilz::GUID::Hasher> triggerEventToIndex;
		

			void TriggerEvent(const TriggerEventStruct& tvs);
		};
	}
}

#endif