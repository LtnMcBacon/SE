#include "Room.h"
#include "Profiler.h"


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
	for (auto enemy : enemyEntities)
	{
		enemy->Update(dt);
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
