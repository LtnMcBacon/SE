#include "Entity.h"

SE::Core::Entity::Entity(uint32_t id)
{
	this->id = id;
}

SE::Core::Entity::Entity()
{
	this->id = 0;
}

uint8_t SE::Core::Entity::Gen() const
{
	return ((this->id >> ENTITY_INDEX_BITS) & ENTITY_GENERATION_MASK);
}

uint32_t SE::Core::Entity::Index() const
{
	return (this->id & ENTITY_INDEX_MASK);
}

bool SE::Core::Entity::operator==(const Entity& rhs) const
{
	return this->id == rhs.id;
}

size_t SE::Core::EntityHasher::operator()(const Entity& e) const
{
	// No need for actual hashing
	return e.id;
}