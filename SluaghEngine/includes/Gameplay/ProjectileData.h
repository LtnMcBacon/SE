#ifndef _SE_GAMEPLAY_PROJECTILE_DATA_H_
#define _SE_GAMEPLAY_PROJECTILE_DATA_H_
#include <Core\Entity.h>
#include <vector>
#include "EventStructs.h"
namespace SE
{
	namespace Gameplay
	{
		enum class ValidTarget
		{
			Everyone,
			Enemies,
			Player
		};

		enum class RotationStyle
		{
			NoRotation,
			AroundSelf,
			AroundPlayer,
			AroundPoint
		};

		struct Rotation
		{
			RotationStyle style = RotationStyle::NoRotation;
			float force;
			float pointToRotateAroundX, pointToRotateAroundY;
		};
		
		struct ProjectileData
		{
			float startPosX, startPosY; // starting position
			float magnitudeX, magnitudeY; // directional power vector for determining what direction the projectile will move in
			float extentsX, extentsY; // width and height of the projectile in the xy plane, assuming the projectile is pointing straight up
			float speed; // speed the projectile moves at
			float maxLifeTime; // how many seconds the projectile should exist at max
			
			Rotation rotation;
			float startRotation; // the starting rotation, most of the time it will be the same as the players rotation

			ValidTarget target; // what tyype of unit the enemy can hit

			DamageEvent eventDamage; // event to be transfered on hit
			HealingEvent eventHealing; // event to be transfered on hit
			ConditionEvent eventCondition; // event to be transfered on hit

			//ParticleEffects
		};
		
	}
}




#endif
