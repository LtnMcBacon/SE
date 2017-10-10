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
	/*int collisionX = 0.0;
	int collisionY = 0.0;*/
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
		if (room)
			room->UpdateAIs(dt);
	StopProfile;
}

bool SE::Gameplay::Room::OnSegment(float pX, float pY, float qX, float qY, float rX, float rY)
{
	StartProfile;

	if (qX <= max(pX, rX) && qX >= min(pX, rX) && qY <= max(pY, rY) && qY >= min(pY, rY))
		ProfileReturnConst(true);

	ProfileReturnConst(false);
}

int SE::Gameplay::Room::Orientation(float pX, float pY, float qX, float qY, float rX, float rY)
{
	// See http://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
	int val = (qY - pY) * (rX - qX) - (qX - pX) * (rY - qY);

	if (val == 0)
		return 0;  // colinear

	return (val > 0) ? 1 : 2; // clock or counterclock wise
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
	StartProfile;
	const int xLeftFloored = int(floor(xCenterPosition - xExtent));
	const int xRightFloored = int(floor(xCenterPosition + xExtent));
	const int yUpFloored = int(floor(yCenterPosition + yExtent));
	const int yDownFloored = int(floor(yCenterPosition - yExtent));


	if (map[xLeftFloored][yDownFloored])
	{
		ProfileReturnConst(true);
	}
	if (map[xLeftFloored][yUpFloored])
	{
		ProfileReturnConst(true);
	}

	if (map[xRightFloored][yUpFloored])
	{
		ProfileReturnConst(true);
	}
	if (map[xRightFloored][yDownFloored])
	{
		ProfileReturnConst(true);
	}


	ProfileReturnConst(false);
}

bool Room::CheckCollisionInRoom(float xCenterPositionBefore, float yCenterPositionBefore, float xCenterPositionAfter,
	float yCenterPositionAfter, float xExtent, float yExtent, int& xCollision, int& yCollision)
{
	StartProfile;
	bool collision = false;
	const int xLeftBeforeFloored = int(xCenterPositionBefore - xExtent);
	const int xRightBeforeFloored = int(xCenterPositionBefore + xExtent);
	//const int xCenterBeforeFloored = int(xCenterPositionBefore);
	const int yUpBeforeFloored = int(yCenterPositionBefore + yExtent);
	const int yDownBeforeFloored = int(yCenterPositionBefore - yExtent);
	//const int yCenterBeforeFloored = int(yCenterPositionBefore);

	const int xLeftAfterFloored = int(xCenterPositionAfter - xExtent);
	const int xRightAfterFloored = int(xCenterPositionAfter + xExtent);

	const int yUpAfterFloored = int(yCenterPositionAfter + yExtent);
	const int yDownAfterFloored = int(yCenterPositionAfter - yExtent);




	if (map[xLeftAfterFloored][yDownBeforeFloored] || map[xLeftAfterFloored][yUpBeforeFloored])
	{
		xCollision = -1;
		collision = true;
	}
	else if (map[xRightAfterFloored][yDownBeforeFloored] || map[xRightAfterFloored][yUpBeforeFloored])
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
	ProfileReturn(collision);
}

void SE::Gameplay::Room::CheckProjectileCollision(std::vector<Projectile>& projectiles)
{
	StartProfile;
	BoundingRect r;
	CollisionData cData;

	for (int i = 0; i < projectiles.size(); i++)
	{
		r = projectiles[i].GetBoundingRect();
		bool collidedLeft = false;
		bool collidedRight = false;
		float xPower = 0.0f;
		float yPower = 0.0f;
		cData = CollisionData();

		if (projectiles[i].GetActive() != true)
		{
			continue;
		}

		if (CheckCollisionInRoom(projectiles[i].GetBoundingRect().upperLeftX, projectiles[i].GetBoundingRect().upperLeftY, 0.0f, 0.0f)) //check if front left corner of projectile is in a blocked square
		{
			collidedLeft = true;
			cData.type = CollisionType::OBJECT;

		}

		if (CheckCollisionInRoom(projectiles[i].GetBoundingRect().upperRightX, projectiles[i].GetBoundingRect().upperRightY, 0.0f, 0.0f)) //check if front right corner of projectile is in a blocked square
		{
			collidedRight = true;
			cData.type = CollisionType::OBJECT;
		}

		if (collidedLeft)
		{
			if (LineCollision(r.lowerLeftX, r.lowerLeftY, r.upperLeftX, r.upperLeftY, int(r.upperLeftX), ceil(r.upperLeftY), ceil(r.upperLeftX), ceil(r.upperLeftY))) // top line
			{
				yPower += 1.0f;
			}
			else if (LineCollision(r.lowerLeftX, r.lowerLeftY, r.upperLeftX, r.upperLeftY, int(r.upperLeftX), int(r.upperLeftY), ceil(r.upperLeftX), int(r.upperLeftY))) // bottom line
			{
				yPower -= 1.0f;
			}

			if (LineCollision(r.lowerLeftX, r.lowerLeftY, r.upperLeftX, r.upperLeftY, ceil(r.upperLeftX), int(r.upperLeftY), ceil(r.upperLeftX), ceil(r.upperLeftY))) // right line
			{
				xPower += 1.0f;
			}
			else if (LineCollision(r.lowerLeftX, r.lowerLeftY, r.upperLeftX, r.upperLeftY, int(r.upperLeftX), int(r.upperLeftY), int(r.upperLeftX), ceil(r.upperLeftY))) // left line
			{
				xPower -= 1.0f;
			}
		}

		if (collidedRight)
		{
			if (LineCollision(r.lowerRightX, r.lowerRightY, r.upperRightX, r.upperRightY, int(r.upperRightX), ceil(r.upperRightY), ceil(r.upperRightX), ceil(r.upperRightY))) // top line
			{
				yPower += 1.0f;
			}
			else if (LineCollision(r.lowerRightX, r.lowerRightY, r.upperRightX, r.upperRightY, int(r.upperRightX), int(r.upperRightY), ceil(r.upperRightX), int(r.upperRightY))) // bottom line
			{
				yPower -= 1.0f;
			}

			if (LineCollision(r.lowerRightX, r.lowerRightY, r.upperRightX, r.upperRightY, ceil(r.upperRightX), int(r.upperRightY), ceil(r.upperRightX), ceil(r.upperRightY))) // right line
			{
				xPower += 1.0f;
			}
			else if (LineCollision(r.lowerRightX, r.lowerRightY, r.upperRightX, r.upperRightY, int(r.upperRightX), int(r.upperRightY), int(r.upperRightX), ceil(r.upperRightY))) // left line
			{
				xPower -= 1.0f;
			}
		}

		/*Normalize the movement vector*/
		float moveTot = abs(xPower) + abs(yPower);
		if (moveTot != 0.0f)
		{
			xPower /= moveTot;
			yPower /= moveTot;
		}
		cData.xVec = xPower;
		cData.yVec = yPower;

		projectiles[i].SetCollisionData(cData);
	}

	StopProfile;
}

bool SE::Gameplay::Room::LineCollision(float p1X, float p1Y, float p2X, float p2Y, float q1X, float q1Y, float q2X, float q2Y)
{
	StartProfile;
	// Find the four orientations needed for general and
	// special cases
	int o1 = Orientation(p1X, p1Y, q1X, q1Y, p2X, p2Y);
	int o2 = Orientation(p1X, p1Y, q1X, q1Y, q2X, q2Y);
	int o3 = Orientation(p2X, p2Y, q2X, q2Y, p1X, p2Y);
	int o4 = Orientation(p2X, p2Y, q2X, q2Y, q1X, q1Y);

	// General case
	if (o1 != o2 && o3 != o4)
		ProfileReturnConst(true);

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && OnSegment(p1X, p1Y, p2X, p2Y, q1X, q1Y))
		ProfileReturnConst(true);

	// p1, q1 and p2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && OnSegment(p1X, p1Y, q2X, q2Y, q1X, q1Y))
		ProfileReturnConst(true);

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && OnSegment(p2X, p2Y, p1X, p1Y, q2X, q2Y))
		ProfileReturnConst(true);

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && OnSegment(p2X, p2Y, q1X, q1Y, q2X, q2Y))
		ProfileReturnConst(true);

	ProfileReturnConst(false); // Doesn't fall in any of the above cases
}

Room::Room(char map[25][25])
{
	StartProfile;
	pos start;
	start.x = start.y = 1.5f;
	memcpy(this->map, map, 25 * 25 * sizeof(char));
	bool foundStart = false;
	for (int x = 0; x < 25 && !foundStart; x++)
	{
		for (int y = 0; y < 25 && !foundStart; y++)
		{
			if (!this->map[x][y])
			{
				start.x = x + 0.5f;
				start.y = y + 0.5f;
				foundStart = true;
			}
		}
	}
	roomField = new FlowField(map, 1.0f, start, 0.0f, 0.0f);
	enemyEntities.reserve(5);
	StopProfile;
}

Room::~Room()
{
	delete roomField;
	for (auto enemy : enemyEntities)
	{
		enemy->DestroyEntity();
		delete enemy;
	}
}

bool Room::AddEnemyToRoom(SE::Gameplay::EnemyUnit *enemyToAdd)
{
	StartProfile;
	enemyEntities.push_back(enemyToAdd);

	/* Should check to make sure that a pre-determined condition ("total power level of room"?)
	* is okay, and first then add the enemy to the room. Otherwise, it should be rejected and stay in the current room.
	*/

	ProfileReturnConst(true);
}


