#include "Room.h"


using namespace SE;
using namespace Gameplay;


void Room::UpdateFlowField()
{

}

void Room::UpdateFlowField(DirectionToAdjacentRoom exit)
{

}

void Room::UpdateAIs(float dt)
{
	for (auto &enemy : enemyEntities)
	{
		enemy->Update(dt);

	}
}

void Room::UpdateAdjacentRooms(float dt)
{

}

void Room::Update(float dt)
{

}

Room::Room(char map[25][25])	
{
	pos start;
	start.x = start.y = 1.5f;
	memcpy(this->map, map, 25 * 25 * sizeof(char));
	roomField = new FlowField(map, 1.0f, start, 0.0f, 0.0f);
}

Room::~Room()
{
	
}

bool Room::AddEnemyToRoom()
{
	/* Should check to make sure that a pre-determined condition ("total power level of room"?)
	* is okay, and first then add the enemy to the room. Otherwise, it should be rejected and stay in the current room.
	*/
	return true;
}
