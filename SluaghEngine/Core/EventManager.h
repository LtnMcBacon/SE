#ifndef SE_CORE_EVENT_MANAGER_H_
#define SE_CORE_EVENT_MANAGER_H_
#include <IEventManager.h>
#include <Utilz\Event.h>
namespace SE
{
	namespace Core
	{
		class EventManager : public IEventManager
		{
		public:
			EventManager();
			~EventManager();

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
		private:
			Utilz::Event<void(const Entity& entity, SE::Graphics::RenderJob* info)> SetRenderObjectInfo;
			Utilz::Event<void(const Entity& entity, SE::Graphics::RenderJob* info)> SetShadowRenderObjectInfo;
			Utilz::Event<void(const Entity& entity, size_t index)> SetDirty;
			Utilz::Event<void(const Entity& entity)> UpdateRenderableObject;
			Utilz::Event<void(const Entity&, bool)> ToggleVisibleEvent;
			Utilz::Event<void(const Entity&, bool)> ToggleShadowEvent;
		};
	}
}

#endif