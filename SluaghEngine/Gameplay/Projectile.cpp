#include <Gameplay\Projectile.h>
#include <Profiler.h>
#include "CoreInit.h"
#include <Gameplay\ProjectileData.h>

void SE::Gameplay::Projectile::UpdateMovement(float dt)
{
	StartProfile;

	float xMovement = 0.0f;
	float yMovement = 0.0f;

	rotation = CoreInit::managers.transformManager->GetRotation(this->unitEntity).y + rotData.force * dt;

	if (rotData.style == RotationStyle::NONE || rotData.style == RotationStyle::SELF)
	{
		xMovement = sinf(rotation);
		yMovement = cosf(rotation);
	}

	/*Normalize the movement vector*/
	float moveTot = abs(xMovement) + abs(yMovement);
	if (moveTot != 0.0f)
	{
		xMovement /= moveTot;
		yMovement /= moveTot;
	}

	/*Move the entity in the normalized direction*/
	MoveEntity(xMovement * dt * speed, yMovement * dt * speed);
	CoreInit::managers.transformManager->SetRotation(this->unitEntity, 0.0f, rotation, 0.0f);

	UpdateBounding();

	StopProfile;
}

void SE::Gameplay::Projectile::UpdateActions(float dt)
{
	StartProfile;

	if (collisionData.type != CollisionType::NONE)
	{
		active = false;
	}

	if (lifeTime <= 0.0f || this->health <= 0.0f)
	{
		active = false;
	}

	while (tempStorage.size())
	{
		functionsToRun.push_back(tempStorage.back());
		tempStorage.pop_back();
	}

	for (int i = 0; i < functionsToRun.size(); i++)
	{
		functionsToRun[i](this, dt);

	}

	collisionData = CollisionData();

	lifeTime -= dt;

	StopProfile;

}

void SE::Gameplay::Projectile::RecreateEntity(Utilz::GUID meshGuid)
{
	this->unitEntity = CoreInit::managers.entityManager->Create();
	CoreInit::managers.transformManager->Create(this->unitEntity);
	CoreInit::managers.renderableManager->CreateRenderableObject(this->unitEntity, { meshGuid });

}

void SE::Gameplay::Projectile::UpdateBounding()
{
	StartProfile;
	rect.upperLeftX = -extentX;
	rect.upperLeftY = extentY;

	rect.upperRightX = extentX;
	rect.upperRightY = extentY;

	rect.lowerLeftX = -extentX;
	rect.lowerLeftY = -extentY;

	rect.lowerRightX = extentX;
	rect.lowerRightY = -extentY;

	RotatePoint(rect.upperLeftX, rect.upperLeftY);
	RotatePoint(rect.upperRightX, rect.upperRightY);

	RotatePoint(rect.lowerLeftX, rect.lowerLeftY);
	RotatePoint(rect.lowerRightX, rect.lowerRightY);

	rect.upperLeftX += xPos;
	rect.upperLeftY += yPos;

	rect.upperRightX += xPos;
	rect.upperRightY += yPos;

	rect.lowerLeftX += xPos;
	rect.lowerLeftY += yPos;

	rect.lowerRightX += xPos;
	rect.lowerRightY += yPos;
	StopProfile;
}

void SE::Gameplay::Projectile::RotatePoint(float & xCoord, float & yCoord)
{
	StartProfile;
	float s = sinf(-CoreInit::managers.transformManager->GetRotation(this->unitEntity).y);
	float c = cosf(-CoreInit::managers.transformManager->GetRotation(this->unitEntity).y);

	// rotate point
	float xnew = xCoord * c - yCoord * s;
	float ynew = xCoord * s + yCoord * c;
	xCoord = xnew;
	yCoord = ynew;
	StopProfile;
}

SE::Gameplay::Projectile::Projectile() : GameUnit(-10000.0f, -10000.0f, 100)
{
	extentX = 0.1f;
	extentY = 0.1f;
	rotation = 0.0f;

	speed = 1.0f;
	lifeTime = 10;
	target = ValidTarget::EVERYONE;

	eventDamage = DamageEvent();
	eventHealing = HealingEvent();
	eventCondition = ConditionEvent();

	rect.radius = sqrt(0.02);

	UpdateBounding();
}

SE::Gameplay::Projectile::Projectile(SE::Gameplay::ProjectileCreationData& cData, ProjectileData& pData) :
	GameUnit(pData.startPosX, pData.startPosY, 100)
{
	extentX = cData.width;
	extentY = cData.height;
	rotation = pData.startRotation;
	rotData = cData.rot;

	speed = cData.projectileSpeed;
	lifeTime = cData.projectileLifeTime;
	target = cData.projectileTarget;

	eventDamage = pData.eventDamage;
	eventHealing = pData.eventHealing;
	eventCondition = pData.eventCondition;
	
	rect.radius = sqrt(extentX*extentX + extentY*extentY);

	generation = cData.generation;

	UpdateBounding();
}

SE::Gameplay::Projectile::Projectile(const Projectile & other) : GameUnit(other)
{
	this->active = other.active;
	this->collisionData = other.collisionData;
	this->eventCondition = other.eventCondition;
	this->eventDamage = other.eventDamage;
	this->eventHealing = other.eventHealing;
	this->extentX = other.extentX;
	this->extentY = other.extentY;
	this->functionsToRun = other.functionsToRun;
	this->tempStorage = other.tempStorage;
	this->behaviourData = other.behaviourData;
	this->lifeTime = other.lifeTime;
	this->rect = other.rect;
	this->rotation = other.rotation;
	this->rotData = other.rotData;
	this->speed = other.speed;
	this->target = other.target;
	this->generation = other.generation;
	this->targetsHit = other.targetsHit;
}

SE::Gameplay::Projectile & SE::Gameplay::Projectile::operator=(const Projectile & other)
{
	GameUnit::operator=(other);
	this->active = other.active;
	this->collisionData = other.collisionData;
	this->eventCondition = other.eventCondition;
	this->eventDamage = other.eventDamage;
	this->eventHealing = other.eventHealing;
	this->extentX = other.extentX;
	this->extentY = other.extentY;
	this->functionsToRun = other.functionsToRun;
	this->tempStorage = other.tempStorage;
	this->behaviourData = other.behaviourData;
	this->lifeTime = other.lifeTime;
	this->rect = other.rect;
	this->rotation = other.rotation;
	this->rotData = other.rotData;
	this->speed = other.speed;
	this->target = other.target;
	this->generation = other.generation;
	this->targetsHit = other.targetsHit;

	return *this;
}

SE::Gameplay::Projectile::Projectile(Projectile && other) : GameUnit(other)
{
	this->active = other.active;
	this->collisionData = other.collisionData;
	this->eventCondition = other.eventCondition;
	this->eventDamage = other.eventDamage;
	this->eventHealing = other.eventHealing;
	this->extentX = other.extentX;
	this->extentY = other.extentY;
	this->functionsToRun = other.functionsToRun;
	this->tempStorage = other.tempStorage;
	this->behaviourData = other.behaviourData;
	this->lifeTime = other.lifeTime;
	this->rect = other.rect;
	this->rotation = other.rotation;
	this->rotData = other.rotData;
	this->speed = other.speed;
	this->target = other.target;
	this->generation = other.generation;
	this->targetsHit = other.targetsHit;
}

SE::Gameplay::Projectile::~Projectile()
{
	StartProfile;
	
	ProfileReturnVoid;
}

void SE::Gameplay::Projectile::AddBehaviourFunction(const std::function<bool(Projectile*projectile, float dt)>& func)
{
	tempStorage.push_back(func);
}

int SE::Gameplay::Projectile::AddBehaviourData(BehaviourData data)
{
	behaviourData.push_back(data);
	return behaviourData.size() - 1;
}

SE::Gameplay::BehaviourData& SE::Gameplay::Projectile::GetBehaviourData(int index)
{
	return behaviourData[index];
}

bool SE::Gameplay::Projectile::CheckIfAlreadyHit(GameUnit * unitToCheck)
{
	for (int i = 0; i < targetsHit.size(); i++)
	{
		if (unitToCheck == targetsHit[i])
			return true;
	}

	return false;
}
