#ifndef _SE_GAMEPLAY_PROJECTILE_DATA_H_
#define _SE_GAMEPLAY_PROJECTILE_DATA_H_
#include <Core\Entity.h>
#include <vector>
#include "EventStructs.h"

#include <ResourceHandler\IResourceHandler.h>
#include <Gameplay\Projectile.h>
#include "Utilz/GUID.h"

#include <memory>

namespace SE
{
	namespace Gameplay
	{
		
		struct ProjectileData
		{
			float startPosX, startPosY, startPosZ = 0.5f; // starting position
			//float magnitudeX, magnitudeY; // directional power vector for determining what direction the projectile will move in
			//float extentsX, extentsY; // width and height of the projectile in the xy plane, assuming the projectile is pointing straight up
			//float speed; // speed the projectile moves at
			//float maxLifeTime; // how many seconds the projectile should exist at max
			
			//Rotation rotation;
			float startRotation; // the starting rotation, most of the time it will be the same as the players rotation

			ValidTarget target; // what type of unit the projectile can hit

			DamageEvent eventDamage; // event to be transfered on hit
			HealingEvent eventHealing; // event to be transfered on hit
			ConditionEvent eventCondition; // event to be transfered on hit
			
			std::weak_ptr<GameUnit*> ownerUnit;

			Utilz::GUID fileNameGuid;
		};
		
	}
}




#endif
