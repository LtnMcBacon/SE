#include "PlayerUnit.h"
#include <Profiler.h>
#include "Flowfield.h"
#include <Core\CollisionManager.h>
#include "Core/Engine.h"

bool SE::Gameplay::PlayerUnit::CorrectCollision(float dt, float &xMov, float &yMov)
{
	StartProfile;
	float moveTot = abs(xMov) + abs(yMov);
	float xMovementTot = xMov;
	float yMovementTot = yMov;
	if (moveTot != 0.0f)
	{
		xMovementTot /= moveTot;
		yMovementTot /= moveTot;
	}
	xMovementTot *= dt;
	yMovementTot *= dt;


	float sampleX = 0.f;
	float sampleY = 0.f;

	float localExtent = extends + 0.15;

	pos myPos;
	/*Checking collision in left x-axis*/
	myPos.x = xPos + xMovementTot - localExtent;
	myPos.y = yPos - localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		xMov = 0;
		ProfileReturnConst(true);
	}
	myPos.y = yPos + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		xMov = 0;
		ProfileReturnConst(true);
	}

	/*Checking collision in right x-axis*/
	myPos.x = xPos + xMovementTot + localExtent;
	myPos.y = yPos - localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		xMov = 0;
		ProfileReturnConst(true);
	}
	myPos.y = yPos + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		xMov = 0;
		ProfileReturnConst(true);
	}

	/*Checking collision in down y-axis*/
	myPos.x = xPos - localExtent;
	myPos.y = yPos + yMovementTot - localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		yMov = 0;
		ProfileReturnConst(true);
	}
	myPos.x = xPos + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		yMov = 0;
		ProfileReturnConst(true);
	}

	/*Checking collision in up y-axis*/
	myPos.x = xPos - localExtent;
	myPos.y = yPos + yMovementTot + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		yMov = 0;
		ProfileReturnConst(true);
	}
	myPos.x = xPos + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		yMov = 0;
		ProfileReturnConst(true);
	}

	ProfileReturnConst(false);
}

void SE::Gameplay::PlayerUnit::UpdateMovement(float dt, const MovementInput & inputs)
{
	StartProfile;
	float xMovement = 0.f;
	float yMovement = 0.f;

	// Handle input and apply movement
	


	// Check for collision and update the movement based on it
	CorrectCollision(dt, xMovement, yMovement);

	/*Normalize the movement vector*/
	float moveTot = abs(xMovement) + abs(yMovement);
	if (moveTot != 0.0f)
	{
		xMovement /= moveTot;
		yMovement /= moveTot;
	}

	/*Move the entity in the normalized direction*/
	MoveEntity(xMovement * dt, yMovement * dt);
	StopProfile;
}

void SE::Gameplay::PlayerUnit::AddForce(float force[2])
{
	StartProfile;
	forcesToApply[0] += force[0];
	forcesToApply[1] += force[1];
	StopProfile;
}

void SE::Gameplay::PlayerUnit::UpdateMap(const char** mapForRoom)
{
	StartProfile;
	map = mapForRoom;
	StopProfile;
}

SE::Gameplay::PlayerUnit::PlayerUnit(void* skills, void* perks, float xPos, float yPos, const char** mapForRoom) :
	GameUnit(xPos, yPos, 100),
	map(mapForRoom)
{
	extends = 0.25f; /*Should not be hardcoded! Obviously*/
}

SE::Gameplay::PlayerUnit::~PlayerUnit()
{
	StartProfile;
	/*
	* Code body
	*/
	ProfileReturnVoid;
}

