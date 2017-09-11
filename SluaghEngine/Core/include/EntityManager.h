#ifndef SE_CORE_ENTITYMANAGER_H_
#define SE_CORE_ENTITYMANAGER_H_
/*Entity system built on the principles described by
 * http://bitsquid.blogspot.se/2014/08/building-data-oriented-entity-system.html
 */
#include <cstdint>

namespace SE
{
	namespace Core
	{
		struct Entity
		{
			static const uint32_t ENTITY_INDEX_BITS = 22;
			static const uint32_t ENTITY_INDEX_MASK = (1 << ENTITY_INDEX_BITS) - 1;
			static const uint32_t ENTITY_GENERATION_BITS = 8;
			static const uint32_t ENTITY_GENERATION_MASK = (1 << ENTITY_GENERATION_BITS) - 1;

			uint32_t id;
		};

		class EntityManager
		{
		public:
			EntityManager();
			~EntityManager();
			EntityManager(const EntityManager& other) = delete;
			EntityManager(const EntityManager&& other) = delete;
			EntityManager& operator=(const EntityManager& rhs) = delete;

			Entity Create();
			bool Alive(Entity e) const;
			void Destroy(Entity e);


		private:
			/*The index part of the entity is used to index this array, as such we only
			 * need to save the generation bits in the array.
			 */
			uint8_t* _generationOfEntity;
			size_t _generationCount;

			/*When an entity is destroyed we'd like to be able to reuse the index in order
			 * to conserve memory.
			 */
			uint32_t* _reusableIndices;
			size_t _reusableIndicesCount;

		};
	}
}



#endif //SE_CORE_ENTITYMANAGER_H_

