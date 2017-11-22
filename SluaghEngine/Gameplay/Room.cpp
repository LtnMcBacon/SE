#include "Room.h"
#include "Profiler.h"
#include <limits>
#include <d3d11.h>
#include <cassert>
#include "CoreInit.h"
#include <math.h>
#include <algorithm>
#include <Items.h>



using namespace SE;
using namespace Gameplay;
#undef max
#undef min


//shaders
static const SE::Utilz::GUID Trans("SimpleNormTransPS.hlsl");
static const SE::Utilz::GUID Norm("SimpleNormMapPS.hlsl");
static const SE::Utilz::GUID BushShader("SimpleLightPS.hlsl");



void Room::UpdateFlowField(float playerX, float playerY)
{
	StartProfile;

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
		ProfileReturnConst(0);  // colinear

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
		CoreInit::managers.eventManager->ToggleVisible(enemyUnits[i]->GetEntity(), true);
		if (!enemyUnits[i]->IsAlive())
		{
			// Blood spatter
			auto bs = CoreInit::managers.entityManager->Create();
			auto p = CoreInit::managers.transformManager->GetPosition(enemyUnits[i]->GetEntity());
			p.y = 0;
			CoreInit::managers.transformManager->Create(bs, p, { DirectX::XM_PIDIV2, 0,0 }, {1,1, 0.05f});
		
			Core::DecalCreateInfo ci;
			ci.textureName = "bloodSpatt.png";			
			CoreInit::managers.decalManager->Create(bs, ci);
			CoreInit::managers.eventManager->SetLifetime(bs, 60);


			auto spw = CoreInit::subSystems.window->GetRand() % 100;
			if (spw < 80)
			{	
				auto item = Item::Create();

				Item::Drop(item, p);
				itemsInRoom.push_back(item);
				CoreInit::managers.dataManager->SetValue(item, "Pickup", false);
			//	CoreInit::managers.eventManager->RegisterEntitytoEvent(item, "RoomChange", &beingRendered);
			}
		
			if(auto enemyWep = std::get_if<Core::Entity>(&CoreInit::managers.dataManager->GetValue(enemyUnits[i]->GetEntity(), "Weapon", false)))
			{
				CoreInit::managers.entityManager->DestroyNow(*enemyWep);
			}
			
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
		ProjectileAgainstEnemies(projectiles[i]);
		ProjectileAgainstWalls(projectiles[i]);
	}

	StopProfile;
}

bool SE::Gameplay::Room::GetClosestEnemy(float xPos, float yPos, float & xReturn, float & yReturn)
{
	StartProfile;

	int enemy = -1;
	float closestDistance = 10000.0f;

	for (int i = 0; i < enemyUnits.size(); i++)
	{
		if (enemyUnits[i]->UnitHealthAboveZero())
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
	}
	if (enemy != -1)
	{
		xReturn = enemyUnits[enemy]->GetXPosition();
		yReturn = enemyUnits[enemy]->GetYPosition();
	}
	ProfileReturnConst(true);
}

bool Room::GetClosestEnemy(float xPos, float yPos, EnemyUnit* &closestUnit)
{
	StartProfile;



	if (enemyUnits.size() == 0)
	{
		ProfileReturnConst(false);
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

float SE::Gameplay::Room::DistanceToClosestDoor(float startX, float startY, DirectionToAdjacentRoom &direction) const
{
	float distance = 1000.0f;
	for (int i = 0; i < 4; i++)
	{
		if(DoorArr[i].active)
		{
			float doorDist = sqrtf((startX - DoorArr[i].posX)*(startX - DoorArr[i].posY) +
				(startY - DoorArr[i].posX)*(startY - DoorArr[i].posY));
			if(doorDist < distance)
			{
				distance = doorDist;
				direction = DirectionToAdjacentRoom(i);
			}
		}
	}
	
	return distance;
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

int SE::Gameplay::Room::PointCollisionWithEnemy(float x, float y, Projectile& projectile)
{
	StartProfile;

	for (int i = 0; i < enemyUnits.size(); i++)
	{
		
		if(!projectile.CheckIfAlreadyHit(enemyUnits[i]) && abs(enemyUnits[i]->GetXPosition() - x) < enemyUnits[i]->GetExtent() && abs(enemyUnits[i]->GetYPosition() - y) < enemyUnits[i]->GetExtent())
		{
			projectile.AddToHit(enemyUnits[i]);
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

	if ((enemyCollidedWith = PointCollisionWithEnemy(r.upperLeftX, r.upperLeftY, projectile)) != -1) //check if front left corner of projectile is in a blocked square
	{
		collidedLeft = true;
		cData.type = CollisionType::ENEMY;
	}
	else if ((enemyCollidedWith = PointCollisionWithEnemy(r.upperRightX, r.upperRightY, projectile)) != -1) //check if front right corner of projectile is in a blocked square
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

bool SE::Gameplay::Room::CreateWall(SE::Core::Entity ent, int x, int y)
{
	int right = tileValues[x + 1][y];
	int left = tileValues[x - 1][y];
	int up = tileValues[x][y + 1];
	int down = tileValues[x][y - 1];
	if (x == 24 || y == 24 || x == 0 || y == 0)
	{
		
		if ((x == 24 && y != 0))
		{
			int side = 1;
			if (left == (char)255)
				side = side * 2;
			if (up == (char)255)
				side = side * 3;
			if (down == (char)255)
				side = side * 7;
			if (side == 42)
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Top] });
			}
			else if (tileValues[x][y - 1] == (char)22 || tileValues[x][y - 1] == (char)48)
			{
				if (side != 2 || side != 6)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 2)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 6)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
				}
			}
			else if (tileValues[x][y + 1] == (char)22 || tileValues[x][y + 1] == (char)48)
			{			
				if (side != 2 || side != 14)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
				}
				else if (side == 2)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 14)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
				}
			}
			else
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
				CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
			}
		}
		else if ((y == 24 && x != 0))
		{
			int side = 1;
			if (right == (char)255)
				side = side * 5;
			if (left == (char)255)
				side = side * 2;
			if (down == (char)255)
				side = side * 7;
			if (side == 70)
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Top] });
			}
			else if (tileValues[x - 1][y] == (char)22 || tileValues[x - 1][y] == (char)48)
			{
				if (side != 7 || side != 35)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 7)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 35)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
				}	
			}
			else if (tileValues[x + 1][y] == (char)22 || tileValues[x + 1][y] == (char)48)
			{			
				if (side != 7 || side != 14)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
				}
				else if (side == 7)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 14)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
			}
			else
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
				CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
			}
		}
		else if ((x == 0 && y != 24))
		{
			int side = 1;
			if (right == (char)255)
				side = side * 5;
			if (up == (char)255)
				side = side * 3;
			if (down == (char)255)
				side = side * 7;
			if (side == (char)105)
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Top] });
			}
			else if (tileValues[x][y - 1] == (char)22 || tileValues[x][y - 1] == (char)48)
			{	
				if (side != 5 || side != 15)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
				}
				else if (side == 5)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 15)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
				}
			}
			else if (tileValues[x][y + 1] == (char)22 || tileValues[x][y + 1] == (char)48)
			{
				if (side != 5 || side != 35)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
				}
				else if (side == 5)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 35)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
				}
			}
			else
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
				CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
			}
		}
		else if ((y == 0 && x != 24))
		{
			int side = 1;
			if (right == (char)255)
				side = side * 5;
			if (left == (char)255)
				side = side * 2;
			if (up == (char)255)
				side = side * 3;
			if (side == 30)
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Top] });
			}
			else if (tileValues[x - 1][y] == (char)22 || tileValues[x - 1][y] == (char)48)
			{
				if (side != 3 || side != 15)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
				}
				else if (side == 3)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 15)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
				}
			}
			else if (tileValues[x + 1][y] == (char)22 || tileValues[x + 1][y] == (char)48)
			{
				if (side != 3 || side != 6)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
				}
				else if (side == 3)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 6)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
			}
			else
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
				CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
			}
		}
		else if ((x == 0 && y == 0) || (x == 24 && y == 0) || (x == 0 && y == 24) || (x == 24 && y == 24))
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Top] });
		}
	}
	else if (right + left + up + down == 40)
	{
		CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Top] });
	}
	else
	{
		int side = 1;
		if (right == (char)255)
			side = side * 5;
		if (left == (char)255)
			side = side * 2;
		if (up == (char)255)
			side = side * 3;
		if (down == (char)255)
			side = side * 7;
		//    3
		//  2/#/5
		//    7

		switch (side) {
		case 1:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::HighWall] });
			break;
		}
		case 2:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
			break;
		}
		case 3:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
			break;
		}
		case 5:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
			break;
		}
		case 7:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
			break;
		}
		case 10:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
			break;
		}
		case 21:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
			break;
		}
		case 6:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
			break;
		}
		case 15:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
			break;
		}
		case 35:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
			break;
		}
		case 14:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
			break;
		}
		case 30:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
			break;
		}
		case 105:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
			break;
		}
		case 70:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
			break;
		}
		case 42:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
			CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
			break;
		}
		default:
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::HighWall] });
			break;
		}
		}
	}
	if (!(x == 24 || y == 24) && (right == 0 || up == 0 || tileValues[x + 1][y + 1] == 0 || tileValues[x - 1][y + 1] == 0 || tileValues[x + 1][y - 1] == 0))
		return true;
	return false;
}

void SE::Gameplay::Room::RandomizeWallAndFloorTexture(SE::Utilz::GUID & wallGuid, SE::Utilz::GUID &floorGuid)
{
	auto rand = CoreInit::subSystems.window->GetRand();
	auto randNr = (rand % 3);

	if (IsOutside == true)
	{
		wallGuid = Materials[Materials::OutsideWall]; 
	}
	else
	{
		switch (randNr)
		{
		case 0:
			wallGuid = Materials[Materials::WallStone];
			break;
		case 1:
			wallGuid = Materials[Materials::Wood];
			break;
		case 2:
			wallGuid = Materials[Materials::Dirt];
			break;
		}
	}

	
	rand = CoreInit::subSystems.window->GetRand();
	randNr = (rand % 3);

	switch (randNr)
	{
	case 0:
		floorGuid = Materials[Materials::FloorStone];
		break;
	case 1:
		floorGuid = Materials[Materials::Dirt];
		break;
	case 2:
		floorGuid = Materials[Materials::Wood];
		break;
	}
	
}

void SE::Gameplay::Room::CreateEntities()
{
	int numberOfEntitesPlaced = 0;
	Core::IMaterialManager::CreateInfo cubeInfo;
	
	int DoorCounter = 0;
	int counter = 0;

	SE::Utilz::GUID wallTexture;
	SE::Utilz::GUID floorTexture;

	RandomizeWallAndFloorTexture(wallTexture, floorTexture);


	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			if (tileValues[i][j] != id_DeadArea)
			{
				auto ent = CoreInit::managers.entityManager->Create();
				CoreInit::managers.transformManager->Create(ent);
				CoreInit::managers.transformManager->SetPosition(ent, DirectX::XMFLOAT3(i + 0.5f, 0.0f, j + 0.5f));

				CreationArguments args{ ent, i, j, DoorCounter, cubeInfo, wallTexture, floorTexture };
				auto exists = propItemToFunction.find(tileValues[i][j]);
				if (exists != propItemToFunction.end())
				{
					exists->second(args);
				}

				DoorCounter = args.doorCounter;
			}
		}
	}
}


void SE::Gameplay::Room::RenderRoom(bool render)
{

	for (auto& i : itemsInRoom)
	{
		if (auto pickedup = std::get<bool>( CoreInit::managers.dataManager->GetValue(i, "Pickup", false)); pickedup == false)
		{
			CoreInit::managers.entityManager->Destroy(i);
		}	
	}
	itemsInRoom.clear();
	for (int i = 0; i < roomEntities.size(); i++)
	{
		
		CoreInit::managers.eventManager->ToggleVisible(roomEntities[i], render);
		CoreInit::managers.eventManager->ToggleShadow(roomEntities[i], render);


	}
	for(auto enemy : enemyUnits)
	{
		CoreInit::managers.eventManager->ToggleVisible(enemy->GetEntity(), render);
		CoreInit::managers.eventManager->ToggleShadow(enemy->GetEntity(), render);
		if(auto weapon = std::get_if<Core::Entity>(&CoreInit::managers.dataManager->GetValue(enemy->GetEntity(), "Weapon", false)))
		{
			CoreInit::managers.eventManager->ToggleVisible(*weapon, render);
		}

	}
	beingRendered = render;
}

SE::Gameplay::Room::DirectionToAdjacentRoom SE::Gameplay::Room::CheckForTransition(float playerX, float playerY)
{
	for (int i = 0; i < 4; i++)
	{
		if (DoorArr[i].active)
		{
			if (sqrt((DoorArr[i].posX - playerX) * (DoorArr[i].posX - playerX) + (DoorArr[i].posY - playerY) * (DoorArr[i].posY - playerY)) <= 1.5f)
			{
					return DirectionToAdjacentRoom(i);
			}
		}
	}

	return SE::Gameplay::Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NONE;
}

void SE::Gameplay::Room::CreateEnemies()
{
	//for (int i = 0; i < 1; i++)
	//{
	//	pos enemyPos;
	//	do
	//	{
	//		enemyPos.x = subSystem.window->GetRand() % 25;
	//		enemyPos.y = subSystem.window->GetRand() % 25;
	//	} while (tileValues[int(enemyPos.x)][int(enemyPos.y)]);

	//	Gameplay::EnemyUnit* enemy = eFactory.CreateEnemy(enemyGUID, &blackBoard);
	//	enemy->PositionEntity(enemyPos.x + .5f, enemyPos.y + .5f);

	//	testRoom->AddEnemyToRoom(enemy);
	//}
}

Room::Room(Utilz::GUID fileName)
{

	myRoomFile = fileName;
	StartProfile;
	// Every prop here:
	Meshes[Meshes::HighWall] = { "HighWall.mesh" };
	Meshes[Meshes::Passage] = { "HighWall_Passage.mesh" };
	Meshes[Meshes::OneSide] = { "HighWall_OneSide.mesh" };
	Meshes[Meshes::Corner] = { "HighWall_Corner.mesh" };
	Meshes[Meshes::Top] = { "HighWall_Top.mesh" };
	Meshes[Meshes::ThreeSides] = { "HighWall_ThreeSides.mesh" };
	Meshes[Meshes::Door] = { "Door.mesh" };
	Meshes[Meshes::Floor] = { "floorTest.mesh" };
	Meshes[Meshes::Torch] = { "Torch_fbx.mesh" };
	Meshes[Meshes::Pillar_short] = { "Pillar_short.mesh" };
	Meshes[Meshes::Table_long] = { "Table_long_fbx.mesh" };
	Meshes[Meshes::Grass] = { "GreenPlane.mesh" };
	Meshes[Meshes::FloorTorch] = { "FloorTorch.mesh" };
	Meshes[Meshes::Table_small] = { "Table_small.mesh" };
	Meshes[Meshes::Table_round] = { "Table_round.mesh" };
	Meshes[Meshes::Chair] = { "Chair.mesh" };
	Meshes[Meshes::Bush] = { "Bush.mesh" };
	Meshes[Meshes::TableGroup1] = { "Table_group1.mesh" };
	Meshes[Meshes::Candlestick_tri] = { "Candlestick_tri.mesh" };
	Meshes[Meshes::PotGroup1] = { "Pot_group1.mesh" };
	Meshes[Meshes::Potatosack_closed] = { "Potato_Sack_Closed.mesh" };
	Meshes[Meshes::Potatosack_open] = { "Potato_Sack_Open.mesh" };
	Meshes[Meshes::Fireplace] = { "Fireplace.mesh" };
	Meshes[Meshes::Painting] = { "Painting.mesh" };


	// Materials
	Materials[Materials::Stone] = { "Cube.mat" };
	Materials[Materials::FloorStone] = { "floorTest.mat" };
	Materials[Materials::DoorMat] = { "Cube.mat" };
	Materials[Materials::WallStone] = { "HighWall.mat" };
	Materials[Materials::Bush] = { "Bush.mat" };
	Materials[Materials::Dirt] = { "brownPlane.mat" };
	Materials[Materials::Grass] = { "GreenPlane.mat" };

	Materials[Materials::Wood] = { "Wood_plane.mat" };
	Materials[Materials::OutsideWall] = { "StoneWallPlane.mat" };



	Prop Chair;
	Chair.guid = Meshes[Meshes::Chair];
	Chair.matGuid = Materials[Materials::Wood];

	Prop Torch;
	Torch.guid = Meshes[Meshes::Torch];
	Torch.matGuid = Materials[Materials::Stone];

	Prop Table_long;
	Table_long.guid = Meshes[Meshes::Table_long];
	Table_long.matGuid = Materials[Materials::Wood];

	Prop Table_small;
	Table_small.guid = Meshes[Meshes::Table_small];
	Table_small.matGuid = Materials[Materials::Wood];

	Prop Table_round;
	Table_round.guid = Meshes[Meshes::Table_round];
	Table_round.matGuid = Materials[Materials::Wood];

	Prop FloorTorch;
	FloorTorch.guid = Meshes[Meshes::FloorTorch];
	FloorTorch.matGuid = Materials[Materials::Stone];

	Prop TableGroup1;
	TableGroup1.guid = Meshes[Meshes::TableGroup1];
	TableGroup1.matGuid = Materials[Materials::Wood];

	Prop CandleStick_tri;
	CandleStick_tri.guid = Meshes[Meshes::Candlestick_tri];
	CandleStick_tri.matGuid = Materials[Materials::Wood];

	Prop PotGroup1;
	PotGroup1.guid = Meshes[Meshes::PotGroup1];
	PotGroup1.matGuid = Materials[Materials::Stone];

	Prop PotatoSackOpen;
	PotatoSackOpen.guid = Meshes[Meshes::Potatosack_open];
	PotatoSackOpen.matGuid = Materials[Materials::Dirt];

	Prop PotatoSackClosed;
	PotatoSackClosed.guid = Meshes[Meshes::Potatosack_closed];
	PotatoSackClosed.matGuid = Materials[Materials::Dirt];

	Prop Bush;
	Bush.guid = Meshes[Meshes::Bush];
	Bush.matGuid = Materials[Materials::Bush];

	Prop Fireplace;
	Fireplace.guid = Meshes[Meshes::Fireplace];
	Fireplace.matGuid = Materials[Materials::Stone];


	// 4x4 tile props - add more here
	propVectors[PropTypes::BIGPROPS] = { FloorTorch, TableGroup1 };
	propVectors[PropTypes::TABLES]   = { Table_small, Table_round };
	propVectors[PropTypes::MEDIUM]   = { Table_long, CandleStick_tri, Fireplace };
	propVectors[PropTypes::BUSHES]   = { Bush };

	// 1x1 tile props // Add more props here
	propVectors[PropTypes::GENERIC] =
	{ Table_small,
		Table_round,
		PotGroup1,
		PotatoSackClosed,
		PotatoSackOpen
	};

	propItemToFunction[id_Bush] = [this](CreationArguments &args) {
		this->CreateBush(args);
	};
	propItemToFunction[id_Floor] = [this](CreationArguments &args) {
		this->CreateFloor(args);
	};
	propItemToFunction[id_Torch] = [this](CreationArguments &args) {
		this->CreateTorch(args);
	};
	propItemToFunction[id_Props] = [this](CreationArguments &args) {
		this->CreateProp(args);
	};
	propItemToFunction[id_Pillar] = [this](CreationArguments &args) {
		this->CreatePillar(args);
	};
	propItemToFunction[id_Wall] = [this](CreationArguments &args) {
		this->CreateWall2(args);
	};
	propItemToFunction[id_Door1] = [this](CreationArguments &args) {
		this->CreateDoor(args);
	};
	propItemToFunction[id_Door2] = [this](CreationArguments &args) {
		this->CreateDoor(args);
	};


	pos start;
	loadfromFile(fileName);
	//	CreateEntities();

		// reset temporary tilevalues where a single prop is overlapping another tile 
		//ResetTempTileValues();

	for (int x = 0; x < 25; x++)
		for (int y = 0; y < 25; y++)
		{
			if (!tileValues[x][y])
			{
				start.x = x;
				start.y = y;
			}
		}


	for (int x = 0; x < 25; x++)
		for (int y = 0; y < 25; y++)
			if (tileValues[x][y] == id_Door1 || tileValues[x][y] == id_Door2)
			{
				DirectionToAdjacentRoom dir = DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NONE;
				if (x - 1 >= 0 && tileValues[x - 1][y] == 0)
					dir = (Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST);
				else if (y - 1 >= 0 && tileValues[x][y - 1] == 0)
					dir = (Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH);
				else if (y + 1 < 25 && tileValues[x][y + 1] == 0)
					dir = (Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH);
				else if (x + 1 < 25 && tileValues[x + 1][y] == 0)
					dir = (Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST);
				DoorArr[size_t(dir)] = { float(x) + 0.5f, float(y) + 0.5f, size_t(x),size_t(y) };
				tileValues[x][y] = id_Wall;
			}


	roomField = new FlowField(tileValues, 1.0f, start, 0.0f, 0.0f);


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

void SE::Gameplay::Room::Load()
{
	CreateEntities();
	ResetTempTileValues();
}

void SE::Gameplay::Room::Unload()
{
	for (auto e : roomEntities)
	{

		CoreInit::managers.entityManager->Destroy(e);


	}
	/*for (auto enemy : enemyUnits)
	{
		CoreInit::managers.entityManager->Destroy(enemy->GetEntity());
		if (auto weapon = std::get_if<Core::Entity>(&CoreInit::managers.dataManager->GetValue(enemy->GetEntity(), "Weapon", false)))
		{
			CoreInit::managers.entityManager->Destroy(*weapon);
		}

	}*/
}

void SE::Gameplay::Room::InitializeAdjacentFlowFields()
{
	for(int i = 0; i < 4; i++)
	{
		if(DoorArr[i].active)
		{
			adjacentRooms[i]->UpdateFlowField(
				adjacentRooms[i]->DoorArr[size_t(ReverseDirection(DirectionToAdjacentRoom(i)))].posX,
				adjacentRooms[i]->DoorArr[size_t(ReverseDirection(DirectionToAdjacentRoom(i)))].posY
			);
		}
	}
}

bool Room::AddEnemyToRoom(SE::Gameplay::EnemyUnit *enemyToAdd)
{
	StartProfile;

	enemyToAdd->SetCurrentRoom(this);
	enemyUnits.push_back(enemyToAdd);
	CoreInit::managers.eventManager->ToggleVisible(enemyToAdd->GetEntity(), beingRendered);
	CoreInit::managers.eventManager->ToggleShadow(enemyToAdd->GetEntity(), beingRendered);
	/* Should check to make sure that a pre-determined condition ("total power level of room"?)
	* is okay, and first then add the enemy to the room. Otherwise, it should be rejected and stay in the current room.
	*/
	if (auto weapon = std::get_if<Core::Entity>(&CoreInit::managers.dataManager->GetValue(enemyToAdd->GetEntity(), "Weapon", false)))
	{
		CoreInit::managers.eventManager->ToggleVisible(*weapon, beingRendered);
	}

	ProfileReturnConst(true);
}

bool Room::AddEnemyToRoom(SE::Gameplay::EnemyUnit *enemyToAdd, DirectionToAdjacentRoom exitDirection)
{
	StartProfile;

	DirectionToAdjacentRoom entranceDirection = ReverseDirection(exitDirection);
	adjacentRooms[int(entranceDirection)]->RemoveEnemyFromRoom(enemyToAdd);
	
	enemyToAdd->SetCurrentRoom(this);

	auto door = DoorArr[int(entranceDirection)];

	float newX = door.posX, newY = door.posY;

	switch(entranceDirection)
	{
	case DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH: newY -= 1.5f; break;
	case DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST: newX += 1.5f; break;
	case DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH: newY += 1.5f; break;
	case DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST: newX -= 1.5f; break;
	case DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NONE: break;
	default: ;
	}

	enemyToAdd->PositionEntity(newX, newY);
	if (auto weapon = std::get_if<Core::Entity>(&CoreInit::managers.dataManager->GetValue(enemyToAdd->GetEntity(), "Weapon", false)))
	{
		CoreInit::managers.eventManager->ToggleVisible(*weapon, beingRendered);
	}
	enemyUnits.push_back(enemyToAdd);
	CoreInit::managers.eventManager->ToggleVisible(enemyToAdd->GetEntity(), beingRendered);
	CoreInit::managers.eventManager->ToggleShadow(enemyToAdd->GetEntity(), beingRendered);

	ProfileReturnConst(true);
}

void SE::Gameplay::Room::RemoveEnemyFromRoom(SE::Gameplay::EnemyUnit * enemyToRemove)
{
	int counter = 0;
	for(auto enemy : enemyUnits)
	{
		if(enemy == enemyToRemove)
		{
			std::swap(enemyUnits[counter], enemyUnits[enemyUnits.size() - 1]);
			enemyUnits.pop_back();
			break;
		}
		counter++;
	}
}

bool SE::Gameplay::Room::GetPositionOfActiveDoor(DirectionToAdjacentRoom door, float & posX, float & posY)
{
	if (DoorArr[int(door)].active)
	{
		posX = DoorArr[int(door)].posX;
		posY = DoorArr[int(door)].posY;
		return true;
	}
	else
		return false;
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
				tileValues[x][y] = (char)(in[counter]);
				if (in[counter] == id_Bush)
				{
					IsOutside = true;
				}
				counter++; 
			}
		}

	
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	
	StopProfile; 
}

float Room::RotatePainting(int x, int y) {
	StartProfile;
	float rotation = 0;


	if ((tileValues[x + 1][y] == id_Floor))
		rotation = 180;
	else if ( (tileValues[x][y + 1] == id_Floor ))
		rotation = 90;
	else if ((tileValues[x][y - 1] == id_Floor ))
		rotation = -90;
	else if ( (tileValues[x - 1][y] == id_Floor ))
		rotation = 0;


	rotation += 270;

	rotation *= 3.1416 / 180;
	ProfileReturnConst(rotation);

}
float Room::WallCheck(int x, int y)
{
	StartProfile;
	float rotation = 0;


	if (x - 1 >= 0 && ( tileValues[x - 1][y] == id_Wall || tileValues[x - 1][y] == id_Pillar  ))
		rotation = 180;
	else if (y - 1 >= 0 && ( tileValues[x][y - 1] == id_Wall || tileValues[x][y - 1] == id_Pillar))
		rotation = 90;																
	else if (y + 1 < 25 && ( tileValues[x][y + 1] == id_Wall || tileValues[x][y + 1] == id_Pillar))
		rotation = -90;																
	else if (x + 1 < 25 && ( tileValues[x + 1][y] == id_Wall || tileValues[x + 1][y] == id_Pillar))
		rotation = 0;


	rotation += 270;

	rotation *= 3.1416 / 180;
	ProfileReturnConst(rotation);
}

SE::Gameplay::Room::Prop Room::GenerateRandomProp(int x, int y, CreationArguments &args, float &rot)
{
	StartProfile;

	bool propCheck = false;
	Prop ret;
	size_t nrOfProps;
	int rand;
	int propId;

	// if we find a prop on the right side of the prop and not beneeth its a 1xx
	if (tileValues[x + 1][y] == id_Props && tileValues[x][y + 1] != id_Props)
	{
		// medium prop 1x2
		CreationArguments args2 = args;
		args2.i = x + 1;
		args2.j = y;
		CreateFloor(args2);


		while (propCheck == false)
		{
			nrOfProps = propVectors[PropTypes::MEDIUM].size();
			rand = CoreInit::subSystems.window->GetRand();
			propId = (rand % nrOfProps);

			if (propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace]) {
				if (CheckPropAgainstWall(x, y, propId, "y", rot))
				{
					propCheck = true;
				}
			}
			else {
				propCheck = true;
			}
		}

		ret.guid = propVectors[PropTypes::MEDIUM][propId].guid;
		ret.matGuid = propVectors[PropTypes::MEDIUM][propId].matGuid;

		ProfileReturn(ret);
	}
	// Else we check if its a 2x1 by checking if a prop is beneeth and not on the right side
	else if (tileValues[x][y + 1] == id_Props && tileValues[x + 1][y] != id_Props)
	{
		//medium prop 2x1
		CreationArguments args2 = args;
		args2.i = x;
		args2.j = y + 1;
		CreateFloor(args2);
		
		while (propCheck == false)
		{
			nrOfProps = propVectors[PropTypes::MEDIUM].size();
			rand = CoreInit::subSystems.window->GetRand();
			propId = (rand % nrOfProps);
			rot = 1.5708;

			if (propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace]) {
				if (CheckPropAgainstWall(x, y, propId, "x", rot))
				{
					propCheck = true;
				}
			}
			else {
				propCheck = true;
			}
		}

		

		ret.guid = propVectors[PropTypes::MEDIUM][propId].guid;
		ret.matGuid = propVectors[PropTypes::MEDIUM][propId].matGuid;

		ProfileReturn(ret);
	}
	else if (tileValues[x][y + 1] == id_Props && tileValues[x + 1][y + 1] == id_Props) {
		// big prop 2x2
		CreationArguments args2 = args;

		args2.i = x;
		args2.j = y + 1;
		CreateFloor(args2);

		args2.i = x + 1;
		args2.j = y;
		CreateFloor(args2);

		args2.i = x + 1;
		args2.j = y + 1;
		CreateFloor(args2);
		
		auto nrOfProps = propVectors[PropTypes::BIGPROPS].size();
		auto rand = CoreInit::subSystems.window->GetRand();
		auto propId = (rand % nrOfProps);

		ret.guid = propVectors[PropTypes::BIGPROPS][propId].guid;
		ret.matGuid = propVectors[PropTypes::BIGPROPS][propId].matGuid;

		ProfileReturn(ret);
	}

	// 1x1 tile props
	nrOfProps = propVectors[PropTypes::GENERIC].size();
	rand = CoreInit::subSystems.window->GetRand();
	propId = (rand % nrOfProps);

	ret.guid = propVectors[PropTypes::GENERIC][propId].guid;
	ret.matGuid = propVectors[PropTypes::GENERIC][propId].matGuid;

	ProfileReturn(ret);
}

void SE::Gameplay::Room::CreateBush(CreationArguments &args)
{
	auto nrOfProps = propVectors[PropTypes::BUSHES].size();
	auto rand = CoreInit::subSystems.window->GetRand();
	auto propId = (rand % nrOfProps);
	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { propVectors[PropTypes::BUSHES][propId].guid, true });

	// rand scale
	float randScale = 0.7 + static_cast <float> (rand) / (static_cast <float> (RAND_MAX / (1.5 - 0.7)));
	CoreInit::managers.transformManager->SetScale(args.ent, randScale);

	Core::IMaterialManager::CreateInfo mi;
	mi.materialFile = Materials[Materials::Bush];
	mi.shader = BushShader;
	CoreInit::managers.materialManager->Create(args.ent, mi);
	//CoreInit::managers.renderableManager->ToggleRenderableObject(args.ent, true);
	roomEntities.push_back(args.ent);

	CreateFloor(args);
}

void SE::Gameplay::Room::CreateFloor(CreationArguments &args)
{
	Core::IMaterialManager::CreateInfo matInfo;
	// Create floor
	auto entFloor = CoreInit::managers.entityManager->Create();
	if (tileValues[args.i][args.j] == id_Bush) // bush 
	{
		matInfo.materialFile = Materials[Materials::Dirt];
	}
	else {
		if (IsOutside)
		{
			matInfo.materialFile = Materials[Materials::Grass];
		}
		else {
			matInfo.materialFile = args.floorMat;
		}
	}
	matInfo.shader = Norm;
	CoreInit::managers.transformManager->Create(entFloor);
	CoreInit::managers.transformManager->SetPosition(entFloor, DirectX::XMFLOAT3(args.i + 0.5f, 0.0f, args.j + 0.5f));
	CoreInit::managers.renderableManager->CreateRenderableObject(entFloor, { Meshes[Meshes::Floor] });
	CoreInit::managers.materialManager->Create(entFloor, matInfo);
	//CoreInit::managers.renderableManager->ToggleRenderableObject(entFloor, true);
	roomEntities.push_back(entFloor);


	if (tileValues[args.i][args.j] == id_Torch )
	{
		tileValues[args.i][args.j] = id_Floor;
	}
}

void SE::Gameplay::Room::CreateTorch(CreationArguments &args)
{
	auto entFire = CoreInit::managers.entityManager->Create();
	CoreInit::managers.transformManager->Create(entFire);
	CoreInit::managers.transformManager->SetPosition(entFire, DirectX::XMFLOAT3(args.i + 0.54, 2.18f, args.j - 0.38));
	SE::Core::IParticleSystemManager::CreateInfo info;
	info.systemFile = Utilz::GUID("torchParticle.pts");

	CoreInit::managers.particleSystemManager->CreateSystem(entFire, info);
	//CoreInit::managers.particleSystemManager->ToggleVisible(entFire, true);
	roomEntities.push_back(entFire);


	// Create torch
	Core::IMaterialManager::CreateInfo matInfo;
	matInfo.materialFile = Materials[Materials::FloorStone];
	matInfo.shader = Norm;
	CoreInit::managers.materialManager->Create(args.ent, matInfo);
	CoreInit::managers.transformManager->SetRotation(args.ent, 0, WallCheck(args.i, args.j), 0);
	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { Meshes[Meshes::Torch] });
	//CoreInit::managers.renderableManager->ToggleRenderableObject(args.ent, true);

	roomEntities.push_back(args.ent);

	CreateFloor(args);
}

void SE::Gameplay::Room::CreatePillar(CreationArguments &args)
{
	Core::IMaterialManager::CreateInfo matInfo;
	matInfo.materialFile = Materials[Materials::Stone];
	matInfo.shader = Norm;
	CoreInit::managers.materialManager->Create(args.ent, matInfo);
	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { Meshes[Meshes::Pillar_short] });

	roomEntities.push_back(args.ent);

	CreateFloor(args);
}

void SE::Gameplay::Room::CreateProp(CreationArguments &args)
{
	int i = args.i;
	int j = args.j;

	float rot;
	Prop prop = GenerateRandomProp(i, j, args, rot);

	Core::IMaterialManager::CreateInfo matInfo;
	matInfo.materialFile = prop.matGuid;
	matInfo.shader = Norm;
	CoreInit::managers.materialManager->Create(args.ent, matInfo);
	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { prop.guid });
//	CoreInit::managers.renderableManager->ToggleRenderableObject(args.ent, true);

	roomEntities.push_back(args.ent);

	CreateFloor(args);

	if (tileValues[i + 1][j] == id_Props && tileValues[i][j + 1] != id_Props)
	{
		// medium prop 2x1
		CoreInit::managers.transformManager->Create(args.ent);
		CoreInit::managers.transformManager->SetPosition(args.ent, DirectX::XMFLOAT3(i + 1.0f, 0.0f, j + 0.5f));
		CoreInit::managers.transformManager->SetRotation(args.ent, 0.0, rot, 0.0);
		tileValues[i + 1][j] = 100;
	}
	else if (tileValues[i][j + 1] == id_Props && tileValues[i + 1][j] != id_Props)
	{
		//medium prop 1x2
		CoreInit::managers.transformManager->Create(args.ent);
		CoreInit::managers.transformManager->SetPosition(args.ent, DirectX::XMFLOAT3(i + 0.5f, 0.0f, j + 1.0f));
		CoreInit::managers.transformManager->SetRotation(args.ent, 0.0, rot, 0.0);
		tileValues[i][j + 1] = 100; // temporary 
	}
	else if (tileValues[i][j + 1] == id_Props && tileValues[i + 1][j + 1] == id_Props) {
		// big prop 2x2
		CoreInit::managers.transformManager->Create(args.ent);
		CoreInit::managers.transformManager->SetPosition(args.ent, DirectX::XMFLOAT3(i + 1.0f, 0.0f, j + 1.0f));
		tileValues[i][j + 1] = 100;
		tileValues[i + 1 ][j] = 100;
		tileValues[i + 1 ][j + 1] = 100;
	}
}

void SE::Gameplay::Room::CreateWall2(CreationArguments &args)
{
	Core::IMaterialManager::CreateInfo matInfo;
	matInfo.materialFile = args.wallMat;

	if (CreateWall(args.ent, args.i, args.j) == true)
	{
		matInfo.shader = Trans;
		CoreInit::managers.renderableManager->ToggleTransparency(args.ent, true);
	}
	else
	{
		matInfo.shader = Norm;
	}
	CoreInit::managers.transformManager->SetPosition(args.ent, DirectX::XMFLOAT3(args.i + 0.5f, 1.5f, args.j + 0.5f));

	CoreInit::managers.materialManager->Create(args.ent, matInfo);

	roomEntities.push_back(args.ent);


	auto rand = CoreInit::subSystems.window->GetRand();
	int randValue = (rand % 500); // 2% chance a wall will have painting

	if ( 0 < randValue && randValue <= 10 )
	{
		auto test = CoreInit::managers.entityManager->Create();
		Core::IMaterialManager::CreateInfo matInfoPainting;
		matInfoPainting.shader = Norm;
		matInfoPainting.materialFile = Materials[Materials::Wood];
		CoreInit::managers.transformManager->Create(test);
		CoreInit::managers.transformManager->SetPosition(test, DirectX::XMFLOAT3(args.i + 0.5f, 1.0f, args.j + 0.5f));
		CoreInit::managers.transformManager->SetRotation(test, 0.0f, RotatePainting(args.i, args.j), 0.0f);
		CoreInit::managers.renderableManager->CreateRenderableObject(test, { Meshes[Meshes::Painting] });
		CoreInit::managers.materialManager->Create(test, matInfoPainting);
		CoreInit::managers.renderableManager->ToggleRenderableObject(test, true);

		roomEntities.push_back(test);
	}

}

void SE::Gameplay::Room::CreateDoor(CreationArguments & args)
{
	Core::IMaterialManager::CreateInfo matInfo;
	int i = args.i;
	int j = args.j;

		CreateFloor(args);

		matInfo.materialFile = Materials[Materials::DoorMat];
		if ((tileValues[i][j + 1] == id_Floor || tileValues[i + 1][j] == id_Floor || tileValues[i + 1][j + 1] == id_Floor || tileValues[i - 1][j + 1] == id_Floor || tileValues[i + 1][j - 1] == id_Floor))
		{
			matInfo.shader = Trans;
			CoreInit::managers.renderableManager->ToggleTransparency(args.ent, true);
		}
		else
		{
			matInfo.shader = Norm;
		}
		CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { Meshes[Meshes::Door] });
		CoreInit::managers.transformManager->SetRotation(args.ent, 0.0f, FloorCheck(i, j), 0.0f);

		CoreInit::managers.materialManager->Create(args.ent, matInfo);
		roomEntities.push_back(args.ent);

	/*	int arrPos = -1;
		if (i - 1 >= 0 && tileValues[i - 1][j] == 0)
			arrPos = int(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST);
		else if (j - 1 >= 0 && tileValues[i][j - 1] == 0)
			arrPos = int(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_NORTH);
		else if (j + 1 < 25 && tileValues[i][j + 1] == 0)
			arrPos = int(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH);
		else if (i + 1 < 25 && tileValues[i + 1][j] == 0)
			arrPos = int(Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST);

		DoorArr[arrPos].doorEntityPos = roomEntities.size() - 1;
		DoorArr[arrPos].xPos = i + 0.5f;
		DoorArr[arrPos].yPos = j + 0.5f;
		DoorArr[arrPos].active = true;
		DoorArr[arrPos].side = Room::DirectionToAdjacentRoom(arrPos);
*/

}

float Room::FloorCheck(int x, int y)
{
	StartProfile;
	float rotation = 0;


	if (x - 1 >= 0 && tileValues[x - 1][y] == id_Floor)
		rotation = 270;
	else if (y - 1 >= 0 && tileValues[x][y - 1] == id_Floor)
		rotation = 180;
	else if (y + 1 < 25 && tileValues[x][y + 1] == id_Floor)
		rotation = 0;
	else if (x + 1 < 25 && tileValues[x + 1][y] == id_Floor)
		rotation = 90;

	rotation += 270;

	rotation *= 3.1416 / 180;
	ProfileReturnConst(rotation);
}
void SE::Gameplay::Room::ResetTempTileValues()
{
	for (int y = 24; y >= 0; y--)
	{
		for (int x = 0; x < 25; x++)
		{
			if (tileValues[x][y] == (char)100)
			{
				tileValues[x][y] = id_Props;
			}
			
		}
	}
}

bool SE::Gameplay::Room::CheckPropAgainstWall(int x, int y, int propId, std::string dir, float &rot)
{
	bool besideWall = false;
	// if fireplace, check for walls
	if (dir == "y")
	{

		// Check both sides of tile if surrounded by walls
		if (tileValues[x][y - 1] == id_Wall && tileValues[x + 1][y - 1] == id_Wall)
		{
			besideWall = true;
			rot = 0;
		}
		else if (tileValues[x][y + 1] == id_Wall && tileValues[x + 1][y + 1] == id_Wall)
		{
			besideWall = true;
			rot = 3.14159;
		}
	}
	else if (dir == "x")
	{
		// Check both sides of tile if surrounded by walls
		if (tileValues[x - 1][y] == id_Wall && tileValues[x - 1][y + 1] == id_Wall)
		{
			besideWall = true;
			rot = 1.5708;
		}
		else if (tileValues[x + 1][y] == id_Wall && tileValues[x + 1][y + 1] == id_Wall)
		{
			besideWall = true;
			rot = -1.5708;
		}
	}
	return besideWall;
}
