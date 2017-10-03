#include "Projectile.h"
#include <Profiler.h>
#include "ProjectileData.h"
#include <Core\CollisionManager.h>
#include "Core/Engine.h"
#include <Gameplay\ProjectileData.h>

void SE::Gameplay::Projectile::UpdateMovement(float dt)
{
	StartProfile;

	float xMovement;
	float yMovement;

	rotation += rotData.force;

	if (rotData.style == RotationStyle::NONE || rotData.style == RotationStyle::SELF)
	{
		xMovement = sinf(rotation);
		yMovement = cosf(rotation);
	}
	else if (rotData.style == RotationStyle::PLAYER)
	{
		xMovement = cosf(rotation);
		yMovement = sinf(rotation);
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

	UpdateBounding();

	StopProfile;
}

void SE::Gameplay::Projectile::UpdateActions(float dt)
{
	StartProfile;

	if (collisionVecX != 0.0f || collisionVecY != 0.0f)
	{
		for (int i = 0; i < onCollision.size(); i++)
		{
			functionsToRun.push_back(onCollision[i]);
		}
	}

	if (lifeTime <= 0.0f || this->health <= 0.0f)
	{
		alive = false;

		for (int i = 0; i < onDeath.size(); i++)
		{
			functionsToRun.push_back(onDeath[i]);
		}
	}

	for (int i = 0; i < functionsToRun.size(); i++)
	{
		if (!functionsToRun[i](this))
		{
			std::swap(functionsToRun[i], functionsToRun[functionsToRun.size() - 1]);
			functionsToRun.pop_back();
		}
	}

	StopProfile;

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
	float s = sinf(rotation);
	float c = cosf(rotation);

	// rotate point
	float xnew = xCoord * c - yCoord * s;
	float ynew = xCoord * s + yCoord * c;
	StopProfile;
}

SE::Gameplay::Projectile::Projectile(ProjectileData data) :
	GameUnit(data.startPosX, data.startPosY, 100)
{
	extentX = 0.25f; /*Should not be hardcoded! Obviously*/
	extentY = 0.25f;
	rotation = data.startRotation;

	UpdateBounding();

}

SE::Gameplay::Projectile::~Projectile()
{
	StartProfile;
	/*
	* Code body
	*/
	ProfileReturnVoid;
}

