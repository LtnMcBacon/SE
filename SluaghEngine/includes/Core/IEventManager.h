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
		/** The Eventmanager is used to register events to entities and to trigger events. The managers also use this for some communication
		* Example code:
		* @code
		* Core::IEventManager::EventCallbacks pickUpEvent;
		* pickUpEvent.triggerCallback = [this](const Core::Entity ent, const std::vector<void*>& args) {
		* bool isWep = std::get<bool>(CoreInit::managers.dataManager->GetValue(ent, "Weapon", false));
		* if (isWep)
		* {
		* CoreInit::subSystems.devConsole->Print("Picked up weapon %s.", std::get<std::string>(CoreInit::managers.dataManager->GetValue(ent, "Name", "Nan"s)).c_str());
		* }
		* CoreInit::managers.entityManager->DestroyNow(ent);
		* };
		*
		* auto pe = player->GetEntity();
		* pickUpEvent.triggerCheck = [pe](const Core::Entity ent, std::vector<void*>& args) {
		* if (CoreInit::managers.collisionManager->CheckCollision(ent, pe))
		* return CoreInit::subSystems.window->ButtonDown(GameInput::INTERACT);
		* return false;
		* };
		* CoreInit::managers.eventManager->RegisterEventCallback("WeaponPickUp", pickUpEvent);
		* @endcode
		*/
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

			struct EventCallbacks
			{
				std::function<void(const Entity, void* userData)> triggerCallback; 
				std::function<bool(const Entity, void* userData)> triggerCheck;
			};

			/**
			* @brief	Register a new event
			*
			* @param[in] _event The event identifier.
			* @param[in] callbacks The callbacks used to check if the event is trigger and to trigger the event.
			*
			*/
			virtual void RegisterEventCallback(const Utilz::GUID _event,
				const EventCallbacks& callbacks) = 0;

			/**
			* @brief	Register an event to an entity
			*
			* @param[in] entity The entity to bind the event to.
			* @param[in] _event The event identifier.
			* @param[in] args to be sent The event identifier.
			*
			*/
			virtual void RegisterEntitytoEvent(const Entity entity, const Utilz::GUID _event, void* userData = nullptr) = 0;

			/**
			* @brief	Set the lifetime of an entity. When the time reaches 0 the entity will be destroyed
			*
			* @param[in] entity The entity to bind the event to.
			* @param[in] lifetime The lifetime of the entity
			*
			*/
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