#ifndef SE_CORE_ENTITYMANAGER_H_
#define SE_CORE_ENTITYMANAGER_H_
/*Entity system built on the principles described by
* http://bitsquid.blogspot.se/2014/08/building-data-oriented-entity-system.html
*/
#include <cstdint>
#include <IEntityManager.h>
namespace SE
{
	namespace Core
	{


		/**
		*
		* @brief The entity manager is responsible for creating and destroying all entities in the system.
		*
		*@details All entities must be created and destroyed via the EntityManager.
		*
		* @sa Entity
		*
		**/

		class EntityManager : public IEntityManager
		{
		public:
			EntityManager();
			~EntityManager();
			EntityManager(const EntityManager& other) = delete;
			EntityManager(const EntityManager&& other) = delete;
			EntityManager& operator=(const EntityManager& rhs) = delete;
			EntityManager& operator=(const EntityManager&& other) = delete;

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
			void DestroyNow(const Entity& e);

			/**
			* @brief    Register a destroy callback for an entity.
			* @param[in] e The entity to bind to.
			* @param[in] callback The callback to bind.
			*/
			void RegisterDestroyCallback(const Entity& e, const Utilz::Delegate<void(const Entity&)>& callback);
		private:
			static const size_t _minimumFreeIndices = 1024;
			static const size_t _generationCapacityIncrement = 512;
			static const size_t _reusableIndicesCapacityIncrement = 128;
		
			/*The index part of the entity is used to index this array, as such we only
			* need to save the generation bits in the array.
			*/
			uint8_t* _generation;
			uint32_t _generationCount;
			uint32_t _generationCapacity;
			Utilz::Event<void(const Entity&)>* _destroyCallbacks;
			/*When an entity is destroyed we'd like to be able to reuse the index in order
			* to conserve memory.
			*/
			uint32_t* _reusableIndices;
			uint32_t _reusableIndicesCount;
			uint32_t _reusableIndicesCapacity;

			void _ExpandGeneration();
			void _ExpandReusable();

		};

	}
}



#endif //SE_CORE_ENTITYMANAGER_H_

