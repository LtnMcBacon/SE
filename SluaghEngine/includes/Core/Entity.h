#ifndef SE_CORE_ENTITY_H_
#define SE_CORE_ENTITY_H_
#include <cstdint>
#include <functional>
namespace SE
{
	namespace Core
	{
		/**
		*
		* @brief An entity.
		*
		* @details An entity is a unique ID. An entity has no information on what components are tied to it, only the component systems know which entities they have.
		* The lower 22 bits of the entity is an index in an array containing the generation of all entities, the 8 bits above that are the generation of the entity.
		* Any systems dealing with entities does not know how this works, it is handled internally by the EntityManager.
		*
		* @warning All entities should be created by EntityManager::Create(). Never use entities not created by this method.
		*
		* @sa EntityManager
		*
		**/
		struct Entity
		{
			static const uint32_t ENTITY_INDEX_BITS = 22;
			static const uint32_t ENTITY_INDEX_MASK = (1 << ENTITY_INDEX_BITS) - 1;
			static const uint32_t ENTITY_GENERATION_BITS = 8;
			static const uint32_t ENTITY_GENERATION_MASK = ((1 << ENTITY_GENERATION_BITS) - 1);
			Entity(uint32_t id);
			Entity();
			uint8_t Gen() const;
			uint32_t Index() const;
			bool operator==(const Entity& rhs) const;

			uint32_t id;
		};

		struct EntityHasher
		{
			size_t operator()(const Entity& e) const;
		};
	}
}


#endif
