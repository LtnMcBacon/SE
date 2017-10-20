#ifndef SE_INTERFACE_ETITY_MANAGER_H_
#define SE_INTERFACE_ETITY_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif

#include "Entity.h"
#include <Utilz\Event.h>

namespace SE
{
	namespace Core
	{
		class IEntityManager
		{
		public:
			virtual ~IEntityManager() {};


			/**
			* @brief    Creates a unique entity. All entities in the system should be created by this method.
			*
			*
			* @warning Will fail if more than 2^(ENTITY_INDEX_BITS) entities exist.
			* @SA Entity
			*/
			Entity Create();

			/**
			* @brief    Checks if an entity is alive, that is an entity for which the Destroy method hasn't been called.
			*
			* @details    Detailed description here.
			* Continuation of description.
			*
			* This is how you make a new line in the description.
			*
			*
			* @param[in] e The entity to check.
			*
			* @retval true The entity is alive.
			* @retval false The entity is not alive
			*/
			bool Alive(const Entity& e) const;
			/**
			* @brief    Destroys an entity. Does not notify any component systems that the entity is alive. It is up to the systems to check if the entity still exists.
			*
			*
			*
			* @param[in] e The entity to destroy.
			*
			*/
			void Destroy(const Entity& e);

			/**
			* @brief    Destroys an entity. This is an immediate destroy.
			*
			* @param[in] e The entity to destroy.
			*
			*/
			virtual void DestroyNow(const Entity& e) = 0;

			/**
			* @brief    Register a destroy callback for an entity.
			* @param[in] e The entity to bind to.
			* @param[in] callback The callback to bind.
			*/
			void RegisterDestroyCallback(const Entity& e, const Utilz::Delegate<void(const Entity&)>& callback);
		protected:
			IEntityManager() {};
	
			IEntityManager(const IEntityManager& other) = delete;
			IEntityManager(const IEntityManager&& other) = delete;
			IEntityManager& operator=(const IEntityManager& other) = delete;
			IEntityManager& operator=(const IEntityManager&& other) = delete;

		};


		/**
		* @brief Create an instance of the EntityManager
		**/
		DECLDIR_CORE IEntityManager* CreateEntityManager();
	}
}

#endif //SE_INTERFACE_ETITY_MANAGER_H_