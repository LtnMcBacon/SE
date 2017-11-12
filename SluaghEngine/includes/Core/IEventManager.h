#ifndef SE_CORE_INTERFACE_EVENT_MANAGER_H_
#define SE_CORE_INTERFACE_EVENT_MANAGER_H_


#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include "Entity.h"
#include <Utilz\Delegate.h>
#include <Graphics\RenderJob.h>
#include "IManager.h"
#include "IDataManager.h"
#include <Window\IWindow.h>
namespace SE
{
	namespace Core
	{
		class IEventManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				Window::IWindow* window;
				IEntityManager* entityManager;
				IDataManager* dataManager;
			};

			virtual ~IEventManager() {};

			virtual void RegisterEventCallback(const Utilz::GUID _event,
				const std::function<void(const Entity, const std::vector<void*>& args)> _eventTriggerCallback,
				const std::function<bool(const Entity, std::vector<void*>& args)> _eventTriggerCheck) = 0;


			virtual void SetLifetime(const Entity entity, float lifetime) = 0;


			virtual void RegisterToSetRenderObjectInfo(const Utilz::Delegate<void(const Entity& entity, SE::Graphics::RenderJob* info)>&& callback) = 0;
			virtual void TriggerSetRenderObjectInfo(const Entity& entity, SE::Graphics::RenderJob* info) = 0;

			virtual void RegisterToSetDirty(const Utilz::Delegate<void(const Entity& entity, size_t index)>&& callback) = 0;
			virtual void TriggerSetDirty(const Entity& entity, size_t index) = 0;

			virtual void RegisterToUpdateRenderableObject(const Utilz::Delegate<void(const Entity&)>&& callback) = 0;
			virtual void TriggerUpdateRenderableObject(const Entity& entity) = 0;

			virtual void RegisterToToggleVisible(const Utilz::Delegate<void(const Entity&, bool)> && callback) = 0;
			virtual void ToggleVisible(const Entity& entity, bool visible) = 0;
		protected:
			IEventManager() {};
			



		};

		/**
		* @brief Create an instance of the EventManager
		**/
		DECLDIR_CORE IEventManager* CreateEventManager(const IEventManager::InitializationInfo& info);
	}
}

#endif