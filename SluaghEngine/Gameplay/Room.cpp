#include "Room.h"
#include "Profiler.h"
#include <d3d11.h>
#include <cassert>


using namespace SE;
using namespace Gameplay;


void Room::UpdateFlowField(float playerX, float playerY)
{
	StartProfile;
	/*
	 * To be written/implemented
	 */
	pos playerPos;
	playerPos.x = playerX;
	playerPos.y = playerY;
	roomField->Update(playerPos);
	StopProfile;
}

void Room::UpdateFlowField(DirectionToAdjacentRoom exit)
{
	StartProfile;
	/*
	 * To be written/implemented
	 */
	StopProfile;
}

void Room::UpdateAIs(float dt)
{
	StartProfile;
	int collisionX = 0.0;
	int collisionY = 0.0;
	for (auto enemy : enemyEntities)
	{
		/*float currentX = enemy->GetXPosition();
		float currentY = enemy->GetYPosition();*/
		
		enemy->Update(dt);

		//float afterX = enemy->GetXPosition();
		//float afterY = enemy->GetYPosition();

		//bool collision = CheckCollisionInRoom(
		//	currentX, currentY,
		//	afterX, afterY,
		//	0.25f, 0.25f,
		//	collisionX, collisionY);



		//if(collision)
		//{
		//	if (collisionX == -1 /*&& collisionY == 0*/)
		//	{
		//		enemy->SetXPosition(currentX);
		//	}
		//	else if(collisionX == 1/* && collisionY == 0*/)
		//	{
		//		enemy->SetXPosition(currentX);
		//	}
		//	if (collisionY == -1 /*&& collisionX == 0*/)
		//	{
		//		enemy->SetYPosition(currentY);
		//	}
		//	else if (collisionY == 1/* && collisionX == 0*/)
		//	{
		//		enemy->SetYPosition(currentY);

		//	}
		
		//}

	}
	StopProfile;
}

void Room::UpdateAdjacentRooms(float dt)
{
	StartProfile;
	for (auto room : adjacentRooms)
		if(room)
			room->UpdateAIs(dt);
	StopProfile;
}

void Room::Update(float dt, float playerX, float playerY)
{
	StartProfile;
	UpdateFlowField(playerX, playerY);
	UpdateAdjacentRooms(dt);
	UpdateAIs(dt);
	StopProfile;
}

bool Room::CheckCollisionInRoom(float xCenterPosition, float yCenterPosition, float xExtent, float yExtent)
{
	
	const int xLeftFloored = int(floor(xCenterPosition - xExtent));
	const int xRightFloored = int(floor(xCenterPosition + xExtent));
	const int yUpFloored = int(floor(yCenterPosition + yExtent));
	const int yDownFloored = int(floor(yCenterPosition - yExtent));


	if (map[xLeftFloored][yDownFloored])
	{
		return true;
	}
	if (map[xLeftFloored][yUpFloored])
	{
		return true;
	}

	if (map[xRightFloored][yUpFloored])
	{
		return true;
	}
	if (map[xRightFloored][yDownFloored])
	{
		return true;
	}



	
	return false;
}

bool Room::CheckCollisionInRoom(float xCenterPositionBefore, float yCenterPositionBefore, float xCenterPositionAfter,
	float yCenterPositionAfter, float xExtent, float yExtent, int& xCollision, int& yCollision)
{
	bool collision = false;
	const int xLeftBeforeFloored = int(xCenterPositionBefore - xExtent);
	const int xRightBeforeFloored = int(xCenterPositionBefore + xExtent);
	const int xCenterBeforeFloored = int(xCenterPositionBefore);
	const int yUpBeforeFloored = int(yCenterPositionBefore + yExtent);
	const int yDownBeforeFloored = int(yCenterPositionBefore - yExtent);
	const int yCenterBeforeFloored = int(yCenterPositionBefore);

	const int xLeftAfterFloored = int(xCenterPositionAfter - xExtent);
	const int xRightAfterFloored = int(xCenterPositionAfter + xExtent);

	const int yUpAfterFloored = int(yCenterPositionAfter + yExtent);
	const int yDownAfterFloored = int(yCenterPositionAfter - yExtent);




	if(map[xLeftAfterFloored][yDownBeforeFloored] || map[xLeftAfterFloored][yUpBeforeFloored])
	{
		xCollision = -1;
		collision = true;
	}
	else if(map[xRightAfterFloored][yDownBeforeFloored] || map[xRightAfterFloored][yUpBeforeFloored])
	{
		xCollision = 1;
		collision = true;
	}

	if (map[xRightBeforeFloored][yUpAfterFloored] || map[xLeftBeforeFloored][yUpAfterFloored])
	{
		yCollision = 1;
		collision = true;
	}
	else if (map[xRightBeforeFloored][yDownAfterFloored] || map[xLeftBeforeFloored][yDownAfterFloored])
	{
		yCollision = -1;
		collision = true;
	}/*

	if(map[xLeftAfterFloored][yDownAfterFloored])
	{
		xCollision = -1;
		yCollision = -1;
		collision = true;
		
	}
	if(map[xLeftAfterFloored][yUpAfterFloored])
	{
		xCollision = -1;
		yCollision = 1;
		collision = true;
		
	}
	if(map[xRightAfterFloored][yUpAfterFloored])
	{
		xCollision = 1;
		yCollision = 1;
		collision = true;
		
	}
	if(map[xRightAfterFloored][yDownAfterFloored])
	{
		xCollision = 1;
		yCollision = -1;
		collision = true;
		
	}*/
	return collision;
}

Room::Room(char map[25][25])	
{
	pos start;
	start.x = start.y = 1.5f;
	memcpy(this->map, map, 25 * 25 * sizeof(char));
	bool foundStart = false;
	for(int x = 0; x < 25 && !foundStart; x++)
	{
		for(int y= 0 ; y < 25 && !foundStart; y++)
		{
			if(!this->map[x][y])
			{
				start.x = x + 0.5f;
				start.y = y + 0.5f;
				foundStart = true;
			}
		}
	}
	roomField = new FlowField(map, 1.0f, start, 0.0f, 0.0f);
	enemyEntities.reserve(5);
}

Room::~Room()
{
	delete roomField;
	for (auto AI : enemyEntities)
		delete AI;
}

bool Room::AddEnemyToRoom(SE::Gameplay::EnemyUnit *enemyToAdd)
{
	enemyEntities.push_back(enemyToAdd);

	/* Should check to make sure that a pre-determined condition ("total power level of room"?)
	* is okay, and first then add the enemy to the room. Otherwise, it should be rejected and stay in the current room.
	*/

	return true;
}


