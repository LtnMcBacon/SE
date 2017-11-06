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
	StartProfile;
	// See http://www.geeksforgeeks.org/orientation-3-ordered-points/
	// for details of below formula.
	float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

	if (val == 0)
		return 0;  // colinear

	ProfileReturnConst((val > 0) ? 1 : 2); // clock or counterclock wise
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
	}
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
	static const int stepsize = 100;
	float deltaX = endX - startX;
	float deltaY = endY - startY;
	float deltaTot = abs(deltaX) + abs(deltaY);

	if (int(deltaTot) == 0.f)
		ProfileReturnConst(true);

	deltaX /= stepsize;
	deltaY /= stepsize;

	float x = startX;
	float y = startY;



	for(int i = 0 ; i < stepsize; i++)
	{
		if (tileValues[int(x)][int(y)])
			ProfileReturnConst(false);

		x += deltaX;
		y += deltaY;
	}

	ProfileReturnConst(true);
}

float Room::DistanceToClosestWall(float startX, float startY, float &distX, float &distY)
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
			if (tileValues[xStart - xDistance][i])
			{
				float xDist = startX - (xDistance - 1);
				float yDist = i - yStart + startY;
				float dist = sqrt(xDist*xDist + yDist*yDist);
				if (dist < distance)
				{
					distance = dist;
					distX = xDist;
					distY = yDist;
				}
			}
		/*Check right side*/
		for (int i = yStart - yDistance; i <= yStart + yDistance; i++)
			if (tileValues[xStart + xDistance][i])
			{
				float xDist = startX + (xDistance - 1);
				float yDist = i - yStart + startY;
				float dist = sqrt(xDist*xDist + yDist*yDist);
				if (dist < distance)
				{
					distance = dist;
					distX = xDist;
					distY = yDist;
				}
			}
		/*Check Up*/
		for (int i = xStart - xDistance; i <= xStart + xDistance; i++)
			if (tileValues[i][yStart + yDistance])
			{
				float yDist = startY + (xDistance - 1);
				float xDist = i - xStart + startX;
				float dist = sqrt(xDist*xDist + yDist*yDist);
				if (dist < distance)
				{
					distance = dist;
					distX = xDist;
					distY = yDist;
				}

			}
		/*Check Down*/
		for (int i = xStart - xDistance; i <= xStart + xDistance; i++)
			if (tileValues[i][yStart - yDistance])
			{
				float yDist = startY - (xDistance - 1);
				float xDist = i - xStart + startX;
				float dist = sqrt(xDist*xDist + yDist*yDist);
				if (dist < distance)
				{
					distance = dist;
					distX = xDist;
					distY = yDist;
				}

			}
		yDistance++;
		xDistance++;
	} while (distance == std::numeric_limits<float>::max());

	ProfileReturnConst(distance);


}

void Room::DistanceToAllEnemies(float startX, float startY, std::vector<float>& returnVector)
{
	StartProfile;
	for(auto enemy : enemyUnits)
	{
		float distX = startX - enemy->GetXPosition();
		float distY = startY - enemy->GetYPosition();

		returnVector.push_back(sqrt(distX*distX + distY*distY));
	}
	StopProfile;
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
	if (projectile.GetValidTarget() == ValidTarget::PLAYER)
		ProfileReturnConst(false);
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
		cData.hitUnit = enemyUnits[enemyCollidedWith]->GetSharedPtr();
	}

	if (cData.type != CollisionType::NONE)
	{
		projectile.SetCollisionData(cData);
		ProfileReturnConst(true);
	}

	ProfileReturnConst(false);
}

bool Room::PointInsideWall(float x, float y)
{
	return !tileValues[int(x)][int(y)];
}

void SE::Gameplay::Room::CreateEntities()
{
	auto floorGUID = Utilz::GUID("Placeholder_Floor.mesh");
	auto wallGUID = Utilz::GUID("HighWall.mesh");
	auto doorGUID = Utilz::GUID("Door.mesh");
	int numberOfEntitesPlaced = 0;


	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			auto ent = CoreInit::managers.entityManager->Create();
			CoreInit::managers.transformManager->Create(ent);
			CoreInit::managers.transformManager->SetPosition(ent, DirectX::XMFLOAT3(i + 0.5f, 0.0f, j + 0.5f));

			if (tileValues[i][j] == 0) 
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { floorGUID });
				CoreInit::managers.transformManager->SetScale(ent, DirectX::XMFLOAT3(0.04f, 1.0f, 0.04f));
			}
			else if (tileValues[i][j] == 10) 
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { wallGUID });
				CoreInit::managers.transformManager->SetPosition(ent, DirectX::XMFLOAT3(i + 0.5f, 0.5f, j + 0.5f));
			}
			else if (tileValues[i][j] == 1 || tileValues[i][j] == 2)
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { doorGUID });
				CoreInit::managers.transformManager->SetRotation(ent, 0.0f, FloorCheck(i, j), 0.0f);
			}

			CoreInit::managers.renderableManager->ToggleRenderableObject(ent, true);
			roomEntities.push_back(ent);
		}
	}
}

void SE::Gameplay::Room::RenderRoom(bool render)
{
	for (int i = 0; i < roomEntities.size(); i++)
	{
		CoreInit::managers.renderableManager->ToggleRenderableObject(roomEntities[i], render);
	}
}

Room::Room(Utilz::GUID fileName)
{
	StartProfile;
	pos start;
	loadfromFile(fileName);

	start.x = start.y = 1.5f;
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
	CreateEntities();
	
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

	
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
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
	ProfileReturnConst(rotation);
}
