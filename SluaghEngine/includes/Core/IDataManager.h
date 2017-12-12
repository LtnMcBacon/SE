#ifndef SE_CORE_INTERFACE_DATA_MANAGER_H_
#define SE_CORE_INTERFACE_DATA_MANAGER_H_


#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include "IManager.h"
#include <Utilz\GUID.h>
#include <variant>
#include <optional>
#include <fstream>

using namespace std::literals;

namespace SE
{
	namespace Core
	{

		/** The DataManager is used to associate basic datatypes to an entity.
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
		class IDataManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				IEntityManager* entityManager;
			};

			typedef std::variant<bool, int32_t, uint32_t, float, std::string, Core::Entity>  values;

			/**
			* @brief	Set the value of a variable. If new variable it will be created.
			*
			* @param[in] entity The entity.
			* @param[in] key The variable name.
			* @param[in] value The value.
			* @warning String must be passed with the suffix s, "A String"s. Otherwise it will be casted to a bool.
			*
			*/
			virtual void SetValue(const Entity entity, const Utilz::GUID key, const values value) = 0;

			/**
			* @brief	Get the value of a variable, if it does not exist return a default.
			*
			* @param[in] entity The entity.
			* @param[in] key The variable name.
			* @param[in] default_value The default value if not found.
			* @warning String must be passed with the suffix s, "A String"s. Otherwise it will be casted to a bool.
			*
			*/
			virtual values GetValue(const Entity entity, const Utilz::GUID key, const values default_value) = 0;

			/**
			* @brief	Copied the values from one entity to another
			* @detail If entity to is not registered it will be added. If it is registered the values will be overwritten.
			*
			* @param[in] to The entity to copy the values to
			* @param[in] from The entity to copy from.
			*
			*/
			virtual void Copy(const Entity to, const Entity from) = 0;

			/**
			* @brief	Returns a reference to the values registered to an entity.
			* @param[in] to The entity to copy the values to
			* @param[in] from The entity to copy from.
			* @warning The reference is only valid until an entity is added or removed from the manager.
			* Example code:
			* @code
			* if(auto values = dataManager->GetAllValues(ent); values.has_value())
			* {
			*	values->get().find("Health");
			*	...
			* }
			* @endcode
			*/
			virtual std::optional<std::reference_wrapper< std::unordered_map<Utilz::GUID, values, Utilz::GUID::Hasher>>> GetAllValues(const Entity entity) = 0;

			/**
			* @brief	Write the values to a file
			* @param[in] entity The entity to write.
			* @param[in] file The filestream
			*
			*/
			virtual bool WriteToFile(const Entity entity, std::ofstream& file) = 0;

			/**
			* @brief	Read values from a file
			* @param[in] entity The entity to read to.
			* @param[in] file The filestream
			*
			*/
			virtual void CreateFromFile(const Entity entity, std::ifstream& file) = 0;
		protected:
			IDataManager() {};
		};

		/**
		* @brief Create an instance of the DataManager
		**/
		DECLDIR_CORE IDataManager* CreateDataManager(const IDataManager::InitializationInfo& info);


	
	}
}

#endif //SE_CORE_INTERFACE_DATA_MANAGER_H_