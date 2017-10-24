#include "Room.h"
#include "Profiler.h"
#include <limits>
#include <d3d11.h>
#include <cassert>
#include "CoreInit.h"
#include <math.h>
#include <algorithm>


using namespace SE;
using namespace Gameplay;
#undef max
#undef min

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
	for (auto enemy : enemyUnits)
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

bool SE::Gameplay::Room::OnSegment(LinePoint p, LinePoint q, LinePoint r)
{
	StartProfile;

	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		ProfileReturnConst(true);

	ProfileReturnConst(false);
}

int SE::Gameplay::Room::Orientation(LinePoint p, LinePoint q, LinePoint r)
{
	// See http://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
	float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

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

	for (int i = 0; i < enemyUnits.size(); i++)
	{
		if (!enemyUnits[i]->IsAlive())
		{
			delete enemyUnits[i];
			enemyUnits[i] = enemyUnits.back();
			enemyUnits.pop_back();
		}
	}

	StopProfile;
}

bool Room::CheckCollisionInRoom(float xCenterPosition, float yCenterPosition, float xExtent, float yExtent)
{
	StartProfile;
	const int xLeftFloored = int(floor(xCenterPosition - xExtent));
	const int xRightFloored = int(floor(xCenterPosition + xExtent));
	const int yUpFloored = int(floor(yCenterPosition + yExtent));
	const int yDownFloored = int(floor(yCenterPosition - yExtent));


	if (tileValues[xLeftFloored][yDownFloored])
	{
		ProfileReturnConst(true);
	}
	if (tileValues[xLeftFloored][yUpFloored])
	{
		ProfileReturnConst(true);
	}

	if (tileValues[xRightFloored][yUpFloored])
	{
		ProfileReturnConst(true);
	}
	if (tileValues[xRightFloored][yDownFloored])
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




	if (tileValues[xLeftAfterFloored][yDownBeforeFloored] || tileValues[xLeftAfterFloored][yUpBeforeFloored])
	{
		xCollision = -1;
		collision = true;
	}
	else if (tileValues[xRightAfterFloored][yDownBeforeFloored] || tileValues[xRightAfterFloored][yUpBeforeFloored])
	{
		xCollision = 1;
		collision = true;
	}

	if (tileValues[xRightBeforeFloored][yUpAfterFloored] || tileValues[xLeftBeforeFloored][yUpAfterFloored])
	{
		yCollision = 1;
		collision = true;
	}
	else if (tileValues[xRightBeforeFloored][yDownAfterFloored] || tileValues[xLeftBeforeFloored][yDownAfterFloored])
	{
		yCollision = -1;
		collision = true;
	}/*

	 if(tileValues[xLeftAfterFloored][yDownAfterFloored])
	 {
	 xCollision = -1;
	 yCollision = -1;
	 collision = true;

	 }
	 if(tileValues[xLeftAfterFloored][yUpAfterFloored])
	 {
	 xCollision = -1;
	 yCollision = 1;
	 collision = true;

	 }
	 if(tileValues[xRightAfterFloored][yUpAfterFloored])
	 {
	 xCollision = 1;
	 yCollision = 1;
	 collision = true;

	 }
	 if(tileValues[xRightAfterFloored][yDownAfterFloored])
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


	for (int i = 0; i < projectiles.size(); i++)
	{
		if (ProjectileAgainstEnemies(projectiles[i]) == false)
		{
			ProjectileAgainstWalls(projectiles[i]);
		}
	}

	StopProfile;
}

bool SE::Gameplay::Room::GetClosestEnemy(float xPos, float yPos, float & xReturn, float & yReturn)
{
	StartProfile;

	if (enemyUnits.size() == 0)
	{
		return false;
	}

	int enemy = -1;
	float closestDistance = 10000.0f;

	for (int i = 0; i < enemyUnits.size(); i++)
	{
		float enemyX = enemyUnits[i]->GetXPosition() - xPos;
		float enemyY = enemyUnits[i]->GetYPosition() - yPos;
		float distance = sqrt(enemyX * enemyX + enemyY * enemyY);

		if (distance < closestDistance)
		{
			enemy = i;
			closestDistance = distance;
		}
	}

	xReturn = enemyUnits[enemy]->GetXPosition();
	yReturn = enemyUnits[enemy]->GetYPosition();

	ProfileReturnConst(true);
}

bool Room::GetClosestEnemy(float xPos, float yPos, EnemyUnit* &closestUnit)
{
	StartProfile;

	if (enemyUnits.size() == 0)
	{
		return false;
	}

	int enemy = -1;
	float closestDistance = 10000.0f;

	for (int i = 0; i < enemyUnits.size(); i++)
	{
		float enemyX = enemyUnits[i]->GetXPosition() - xPos;
		float enemyY = enemyUnits[i]->GetYPosition() - yPos;
		float distance = sqrt(enemyX * enemyX + enemyY * enemyY);

		if (distance < closestDistance)
		{
			enemy = i;
			closestDistance = distance;
		}
	}

	closestUnit = enemyUnits[enemy];

	ProfileReturnConst(true);
}

bool Room::CheckLineOfSightBetweenPoints(float startX, float startY, float endX, float endY) const
{
	StartProfile;
	if(int(startX) == int(endX))
	{
		int distance = abs(int(startY) - int(endY));
		int x = int(startX);
		int y = int(startY);
		if(startY < endY)
		{
			for (int i = 0; i < distance; i++)
				if (map[x][y + i])
					ProfileReturnConst(false);
		}
		else
		{
			for (int i = 0; i < distance; i++)
				if (map[x][y - i])
					ProfileReturnConst(false);
		}
	}
	else if(int(startY) == int(endY))
	{
		int distance = abs(int(startX) - int(endX));
		int x = int(startX);
		int y = int(startY);
		if (startX < endX)
		{
			for (int i = 0; i < distance; i++)
				if (map[x+i][y])
					ProfileReturnConst(false);
		}
		else
		{
			for (int i = 0; i < distance; i++)
				if (map[x-i][y])
					ProfileReturnConst(false);
		}
	}
	else
	{
		/**
		 * Breseham's Line ALgorithm
		 * https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html
		 *  Octants:
		 *	 \2 | 1/
		 *	3 \ | / 0
		 *	 ---+---
		 *	4 / | \ 7
		 *	 /5 | 6\	
		 *	 
		 */

		float m = (endY - startY) / (endX - startX);
		
		/*Decide octant*/
		int octant = 0;

		if(0.f < m && m <= 1.f)
		{
			if (startX < endX)
				octant = 0;
			else
				octant = 4;
		}
		else if(1.f < m)
		{
			if (startY < endY)
				octant = 1;
			else
				octant = 5;
		}
		else if(m < -1.f)
		{
			if (startY < endY)
				octant = 2;
			else
				octant = 6;
		}
		else
		{
			if (endX < startX)
				octant = 3;
			else
				octant = 7;
		}
		float xStart = 0.f;
		float xEnd = 0.f;
		float yStart = 0.f;
		float yEnd = 0.f;
		switch(octant)
		{
		case 0:
			xStart = startX;
			xEnd = endX;
			yStart = startY;
			yEnd = endY;
			break;

		case 1:
			xStart = startX;
			xEnd = endX;
			yStart = startY;
			yEnd = endY;
			break;

		case 2:
			xStart = endX;
			xEnd = startX;
			yStart = startY;
			yEnd = endY;
			break;

		case 3:
			xStart = endX;
			xEnd = startX;
			yStart = startY;
			yEnd = endY;
			break;

		case 4:
			xStart = endX;
			xEnd = startX;
			yStart = endY;
			yEnd = startY;
			break;

		case 5:
			xStart = endX;
			xEnd = startX;
			yStart = endY;
			yEnd = startY;
			break;

		case 6:
			xStart = startX;
			xEnd = endX;
			yStart = endY;
			yEnd = startY;
			break;

		case 7:
			xStart = startX;
			xEnd = endX;
			yStart = endY;
			yEnd = startY;
			break;

		default: 
			break;
		}

		if(m < 0.f) /*Negative Slope*/
		{
			if(octant == 2 || octant == 6) /*Loop over y*/
			{
				
				int x = int(xStart);
				float eps = 0;
				for (int y = int(yStart); y < int(yEnd); y++)
				{
					if (map[x][y])
						ProfileReturnConst(false);
					eps += m;
					if (eps > -0.5f)
					{
						x--;
						eps += 1.f;
					}
				}
			}
			else /*Loop over x*/
			{
				int y = yStart;
				float eps = 0;
				for (int x = int(xStart); x <= int(xEnd); x++)
				{
					if (map[x][y])
						ProfileReturnConst(false);
					eps += m;
					if (eps > -0.5f)
					{
						y--;
						eps += 1.f;
					}
				}
			}
		}
		else
		{
			if (octant == 1 || octant == 5) /*Loop over y*/
			{
				
				int x = int(xStart);
				float eps = 0;
				for(int y = int(yStart); y < int(yEnd); y++)
				{
					if (map[x][y])
						ProfileReturnConst(false);
					eps += m;
					if(eps < 0.5f)
					{
						x++;
						eps -= 1.f;
					}
				}
			}
			else /*Loop over x*/
			{
				
				int y = int(yStart);
				float eps = 0;
				for (int x = int(xStart); x <= int(xEnd); x++)
				{
					if (map[x][y])
						ProfileReturnConst(false);
					eps += m;
					if (eps < 0.5f)
					{
						y++;
						eps -= 1.f;
					}
				}
			}
		}
	}

	ProfileReturnConst(true);
}

float Room::DistanceToClosestWall(float startX, float startY)
{
	StartProfile;
	/*We should never have a room of this size*/
	float distance = std::numeric_limits<float>::max();
	int xStart = int(startX);
	int yStart = int(startY);
	int xDistance = 1;
	int yDistance = 1;
	do
	{
		/*Check left side*/
		for (int i = yStart - yDistance; i <= yStart + yDistance; i++)
			if (map[xStart - xDistance][i])
			{
				float xDist = startX - (xDistance - 1);
				float yDist = i - yStart + startY;
				float dist = sqrt(xDist*xDist + yDist*yDist);
				if (dist < distance)
					distance = dist;
			}
		/*Check right side*/
		for (int i = yStart - yDistance; i <= yStart + yDistance; i++)
			if (map[xStart + xDistance][i])
			{
				float xDist = startX + (xDistance - 1);
				float yDist = i - yStart + startY;
				float dist = sqrt(xDist*xDist + yDist*yDist);
				if (dist < distance)
					distance = dist;
			}
		/*Check Up*/
		for (int i = xStart - xDistance; i <= xStart + xDistance; i++)
			if (map[i][yStart + yDistance])
			{
				float yDist = startY + (xDistance - 1);
				float xDist = i - xStart + startX;
				float dist = sqrt(xDist*xDist + yDist*yDist);
				if (dist < distance)
					distance = dist;

			}
		/*Check Down*/
		for (int i = xStart - xDistance; i <= xStart + xDistance; i++)
			if (map[i][yStart - yDistance])
			{
				float yDist = startY - (xDistance - 1);
				float xDist = i - xStart + startX;
				float dist = sqrt(xDist*xDist + yDist*yDist);
				if (dist < distance)
					distance = dist;

			}
		yDistance++;
		xDistance++;
	} while (distance == std::numeric_limits<float>::max());

	ProfileReturnConst(distance);


}

void Room::DistanceToAllEnemies(float startX, float startY, std::vector<float>& returnVector)
{
	for(auto enemy : enemyUnits)
	{
		float distX = startX - enemy->GetXPosition();
		float distY = startY - enemy->GetYPosition();

		returnVector.push_back(sqrt(distX*distX + distY*distY));
	}
}

bool SE::Gameplay::Room::LineCollision(LinePoint p1, LinePoint q1, LinePoint p2, LinePoint q2)
{
	StartProfile;
	// Find the four orientations needed for general and
	// special cases
	int o1 = Orientation(p1, q1, p2);
	int o2 = Orientation(p1, q1, q2);
	int o3 = Orientation(p2, q2, p1);
	int o4 = Orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		ProfileReturnConst(true);

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && OnSegment(p1, p2, q1))
		ProfileReturnConst(true);

	// p1, q1 and p2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && OnSegment(p1, q2, q1))
		ProfileReturnConst(true);

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && OnSegment(p2, p1, q2))
		ProfileReturnConst(true);

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && OnSegment(p2, q1, q2))
		ProfileReturnConst(true);

	ProfileReturnConst(false); // Doesn't fall in any of the above cases
}

void SE::Gameplay::Room::ProjectileAgainstWalls(Projectile & projectile)
{
	StartProfile;

	bool collidedRight = false;
	bool collidedLeft = false;
	float xPower = 0.0f;
	float yPower = 0.0f;
	BoundingRect r = projectile.GetBoundingRect();
	CollisionData cData;

	if (CheckCollisionInRoom(r.upperLeftX, r.upperLeftY, 0.0f, 0.0f)) //check if front left corner of projectile is in a blocked square
	{
		collidedLeft = true;
		cData.type = CollisionType::OBJECT;

	}

	if (CheckCollisionInRoom(r.upperRightX, r.upperRightY, 0.0f, 0.0f)) //check if front right corner of projectile is in a blocked square
	{
		collidedRight = true;
		cData.type = CollisionType::OBJECT;
	}

	if (collidedLeft)
	{
		if (LineCollision(LinePoint(r.lowerLeftX, r.lowerLeftY), LinePoint(r.upperLeftX, r.upperLeftY), LinePoint(int(r.upperLeftX), ceil(r.upperLeftY)), LinePoint(ceil(r.upperLeftX), ceil(r.upperLeftY)))) // top line
		{
			yPower += 1.0f;
		}
		else if (LineCollision(LinePoint(r.lowerLeftX, r.lowerLeftY), LinePoint(r.upperLeftX, r.upperLeftY), LinePoint(int(r.upperLeftX), int(r.upperLeftY)), LinePoint(ceil(r.upperLeftX), int(r.upperLeftY)))) // bottom line
		{
			yPower -= 1.0f;
		}

		if (LineCollision(LinePoint(r.lowerLeftX, r.lowerLeftY), LinePoint(r.upperLeftX, r.upperLeftY), LinePoint(ceil(r.upperLeftX), int(r.upperLeftY)), LinePoint(ceil(r.upperLeftX), ceil(r.upperLeftY)))) // right line
		{
			xPower += 1.0f;
		}
		else if (LineCollision(LinePoint(r.lowerLeftX, r.lowerLeftY), LinePoint(r.upperLeftX, r.upperLeftY), LinePoint(int(r.upperLeftX), int(r.upperLeftY)), LinePoint(int(r.upperLeftX), ceil(r.upperLeftY)))) // left line
		{
			xPower -= 1.0f;
		}
	}

	if (collidedRight)
	{
		if (LineCollision(LinePoint(r.lowerRightX, r.lowerRightY), LinePoint(r.upperRightX, r.upperRightY), LinePoint(int(r.upperRightX), ceil(r.upperRightY)), LinePoint(ceil(r.upperRightX), ceil(r.upperRightY)))) // top line
		{
			yPower += 1.0f;
		}
		else if (LineCollision(LinePoint(r.lowerRightX, r.lowerRightY), LinePoint(r.upperRightX, r.upperRightY), LinePoint(int(r.upperRightX), int(r.upperRightY)), LinePoint(ceil(r.upperRightX), int(r.upperRightY)))) // bottom line
		{
			yPower -= 1.0f;
		}

		if (LineCollision(LinePoint(r.lowerRightX, r.lowerRightY), LinePoint(r.upperRightX, r.upperRightY), LinePoint(ceil(r.upperRightX), int(r.upperRightY)), LinePoint(ceil(r.upperRightX), ceil(r.upperRightY)))) // right line
		{
			xPower += 1.0f;
		}
		else if (LineCollision(LinePoint(r.lowerRightX, r.lowerRightY), LinePoint(r.upperRightX, r.upperRightY), LinePoint(int(r.upperRightX), int(r.upperRightY)), LinePoint(int(r.upperRightX), ceil(r.upperRightY)))) // left line
		{
			xPower -= 1.0f;
		}
	}

	if (cData.type != CollisionType::NONE)
	{
		/*Normalize the collision vector*/
		float moveTot = abs(xPower) + abs(yPower);
		if (moveTot != 0.0f)
		{
			xPower /= moveTot;
			yPower /= moveTot;
		}

		cData.xVec = xPower;
		cData.yVec = yPower;

		projectile.SetCollisionData(cData);
	}

	StopProfile;
}

int SE::Gameplay::Room::PointCollisionWithEnemy(float x, float y)
{
	StartProfile;

	for (int i = 0; i < enemyUnits.size(); i++)
	{
		//if (abs(p.GetXPosition() - enemyUnits[i]->GetXPosition()) < (p.GetExtentX() + enemyUnits[i]->GetExtent()) && abs(p.GetYPosition() - enemyUnits[i]->GetYPosition()) < (p.GetExtentY() + enemyUnits[i]->GetExtent()))
		if(abs(enemyUnits[i]->GetXPosition() - x) < enemyUnits[i]->GetExtent() && abs(enemyUnits[i]->GetYPosition() - y) < enemyUnits[i]->GetExtent())
		{
			ProfileReturnConst(i);
		}
	}

	ProfileReturnConst(-1);
}

bool SE::Gameplay::Room::ProjectileAgainstEnemies(Projectile & projectile)
{
	StartProfile;

	bool collidedRight = false;
	bool collidedLeft = false;
	float xPower = 0.0f;
	float yPower = 0.0f;
	BoundingRect r = projectile.GetBoundingRect();
	CollisionData cData;
	int enemyCollidedWith = -1;

	if ((enemyCollidedWith = PointCollisionWithEnemy(r.upperLeftX, r.upperLeftY)) != -1) //check if front left corner of projectile is in a blocked square
	{
		collidedLeft = true;
		cData.type = CollisionType::ENEMY;
	}
	else if ((enemyCollidedWith = PointCollisionWithEnemy(r.upperRightX, r.upperRightY)) != -1) //check if front right corner of projectile is in a blocked square
	{
		collidedRight = true;
		cData.type = CollisionType::ENEMY;
	}

	if (enemyCollidedWith != -1)
	{
		enemyUnits[enemyCollidedWith]->AddDamageEvent(projectile.GetProjectileDamageEvent());
		enemyUnits[enemyCollidedWith]->AddHealingEvent(projectile.GetProjectileHealingEvent());
		enemyUnits[enemyCollidedWith]->AddConditionEvent(projectile.GetProjectileConditionEvent());
	}

	//if we at some point want to have a reflection vector from enemies as well then comment in the two sections below and change so that the two last points in the below calls corresponds to
	//the lines formed by taking the centerpoint of the enemy we collided with and adding/subtracting the extent of the enemy

	//if (collidedLeft)
	//{
	//	if (LineCollision(LinePoint(r.lowerLeftX, r.lowerLeftY), LinePoint(r.upperLeftX, r.upperLeftY), LinePoint(int(r.upperLeftX), ceil(r.upperLeftY)), LinePoint(ceil(r.upperLeftX), ceil(r.upperLeftY)))) // top line
	//	{
	//		yPower += 1.0f;
	//	}
	//	else if (LineCollision(LinePoint(r.lowerLeftX, r.lowerLeftY), LinePoint(r.upperLeftX, r.upperLeftY), LinePoint(int(r.upperLeftX), int(r.upperLeftY)), LinePoint(ceil(r.upperLeftX), int(r.upperLeftY)))) // bottom line
	//	{
	//		yPower -= 1.0f;
	//	}

	//	if (LineCollision(LinePoint(r.lowerLeftX, r.lowerLeftY), LinePoint(r.upperLeftX, r.upperLeftY), LinePoint(ceil(r.upperLeftX), int(r.upperLeftY)), LinePoint(ceil(r.upperLeftX), ceil(r.upperLeftY)))) // right line
	//	{
	//		xPower += 1.0f;
	//	}
	//	else if (LineCollision(LinePoint(r.lowerLeftX, r.lowerLeftY), LinePoint(r.upperLeftX, r.upperLeftY), LinePoint(int(r.upperLeftX), int(r.upperLeftY)), LinePoint(int(r.upperLeftX), ceil(r.upperLeftY)))) // left line
	//	{
	//		xPower -= 1.0f;
	//	}
	//}

	//if (collidedRight)
	//{
	//	if (LineCollision(LinePoint(r.lowerRightX, r.lowerRightY), LinePoint(r.upperRightX, r.upperRightY), LinePoint(int(r.upperRightX), ceil(r.upperRightY)), LinePoint(ceil(r.upperRightX), ceil(r.upperRightY)))) // top line
	//	{
	//		yPower += 1.0f;
	//	}
	//	else if (LineCollision(LinePoint(r.lowerRightX, r.lowerRightY), LinePoint(r.upperRightX, r.upperRightY), LinePoint(int(r.upperRightX), int(r.upperRightY)), LinePoint(ceil(r.upperRightX), int(r.upperRightY)))) // bottom line
	//	{
	//		yPower -= 1.0f;
	//	}

	//	if (LineCollision(LinePoint(r.lowerRightX, r.lowerRightY), LinePoint(r.upperRightX, r.upperRightY), LinePoint(ceil(r.upperRightX), int(r.upperRightY)), LinePoint(ceil(r.upperRightX), ceil(r.upperRightY)))) // right line
	//	{
	//		xPower += 1.0f;
	//	}
	//	else if (LineCollision(LinePoint(r.lowerRightX, r.lowerRightY), LinePoint(r.upperRightX, r.upperRightY), LinePoint(int(r.upperRightX), int(r.upperRightY)), LinePoint(int(r.upperRightX), ceil(r.upperRightY)))) // left line
	//	{
	//		xPower -= 1.0f;
	//	}
	//}

	if (cData.type != CollisionType::NONE)
	{
		///*Normalize the collision vector*/
		//float moveTot = abs(xPower) + abs(yPower);
		//if (moveTot != 0.0f)
		//{
		//	xPower /= moveTot;
		//	yPower /= moveTot;
		//}

		//cData.xVec = xPower;
		//cData.yVec = yPower;

		projectile.SetCollisionData(cData);

		ProfileReturnConst(true);
	}

	ProfileReturnConst(false);
}

bool Room::PointInsideWall(float x, float y)
{
	return !map[int(x)][int(y)];
}
Room::Room(Utilz::GUID fileName)
{
	StartProfile;
	pos start;
	loadfromFile(fileName);

	start.x = start.y = 1.5f;
	//memcpy(this->tileValues, tileValues, 25 * 25 * sizeof(char));
	bool foundStart = false;
	//for (int x = 0; x < 25 && !foundStart; x++)
	//{
	//	for (int y = 0; y < 25 && !foundStart; y++)
	//	{
	//		if (!this->tileValues[x][y])
	//		{
	//			start.x = x + 0.5f;
	//			start.y = y + 0.5f;
	//			foundStart = true;
	//		}
	//	}
	//}
	roomField = new FlowField(tileValues, 1.0f, start, 0.0f, 0.0f);
	enemyUnits.reserve(5);

	
	StopProfile;
}

Room::~Room()
{
	delete roomField;
	for (auto enemy : enemyUnits)
	{
		enemy->DestroyEntity();
		delete enemy;
	}
}

bool Room::AddEnemyToRoom(SE::Gameplay::EnemyUnit *enemyToAdd)
{
	StartProfile;
	enemyUnits.push_back(enemyToAdd);

	/* Should check to make sure that a pre-determined condition ("total power level of room"?)
	* is okay, and first then add the enemy to the room. Otherwise, it should be rejected and stay in the current room.
	*/

	ProfileReturnConst(true);
}

void Room::loadfromFile(Utilz::GUID fileName)
{
	StartProfile;
	
	CoreInit::subSystems.resourceHandler->LoadResource(fileName, [this](auto GUID, void* data, size_t size){
		/*temporary stores values from file*/
		unsigned char* in = (unsigned char*)data;

		int counter = 0; 
		for (int y = 24; y >= 0; y--)
		{
			for (int x = 0; x < 25; x++)
			{
				tileValues[x][y] = (float)(in[counter] / 25); 
				counter++; 
			}
		}

	
		return ResourceHandler::InvokeReturn::Success;
	});
	
	StopProfile; 
}

float Room::FloorCheck(int x, int y)
{
	StartProfile;
	float rotation = 0; 


	if (x - 1 >= 0 && tileValues[x - 1][y] == 0)
		rotation = 270;
	else if (y - 1 >= 0 && tileValues[x][y - 1] == 0)
		rotation = 180;
	else if (y + 1 < 25 && tileValues[x][y + 1] == 0)
		rotation = 0;
	else if (x + 1 < 25 && tileValues[x + 1][y] == 0)
		rotation = 90;

	rotation += 270;

	rotation *= 3.1416 / 180; 
	return rotation; 
	StopProfile; 
}
