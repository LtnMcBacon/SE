#include "Room.h"
#include "Profiler.h"
#include <limits>
#include <d3d11.h>
#include <cassert>
#include "CoreInit.h"
#include <math.h>
#include <algorithm>
#include <Items.h>
#include <EnemyUnit.h>


using namespace SE;
using namespace Gameplay;
#undef max
#undef min


//shaders
static const SE::Utilz::GUID Norm("SimpleNormMapPS.hlsl");
static const SE::Utilz::GUID BushShader("SimpleLightPS.hlsl");



void SE::Gameplay::Room::UpdateHpBars(float playerX, float playerY)
{
	StartProfile;
	const int barWidth = 47;
	const int barHeight = 12;

	const int frameWidth = 51;
	const int frameHeight = 14;

	if (enemyUnits.size() > hpBars.size())
	{
		int toAdd = enemyUnits.size() - hpBars.size();

		for (int i = 0; i < toAdd; i++)
		{
			Core::Entity tempBar = CoreInit::managers.entityManager->Create();
			Core::IGUIManager::CreateInfo barInfo;
			barInfo.texture = "Enemy hpBar.jpg";
			barInfo.textureInfo.height = barHeight;
			barInfo.textureInfo.width = barWidth;
			barInfo.textureInfo.anchor = { 0.0f, 0.0f };
			barInfo.textureInfo.layerDepth = 0.01 * hpBars.size() + 0.01;

			CoreInit::managers.guiManager->Create(tempBar, barInfo);
			CoreInit::managers.guiManager->ToggleRenderableTexture(tempBar, true);

			Core::Entity tempFrame = CoreInit::managers.entityManager->Create();
			Core::IGUIManager::CreateInfo frameInfo;
			frameInfo.texture = "Enemy hpBar Frame.jpg";
			frameInfo.textureInfo.height = frameHeight;
			frameInfo.textureInfo.width = frameWidth;
			frameInfo.textureInfo.anchor = { 0.0f, 0.0f };
			frameInfo.textureInfo.layerDepth = 0.011 * hpBars.size() + 0.011;

			CoreInit::managers.guiManager->Create(tempFrame, frameInfo);
			CoreInit::managers.guiManager->ToggleRenderableTexture(tempFrame, true);

			HpBar tempHpBar;
			tempHpBar.bar = tempBar;
			tempHpBar.frame = tempFrame;
			hpBars.push_back(tempHpBar);
		}
	}
	
	for (int i = 0; i < enemyUnits.size(); i++)
	{
		if (0.f < enemyUnits[i]->GetHealth())
		{
			CoreInit::managers.guiManager->ToggleRenderableTexture(hpBars[i].bar, true);
			CoreInit::managers.guiManager->ToggleRenderableTexture(hpBars[i].frame, true);

			float xPos, yPos;
			auto a = CoreInit::managers.cameraManager->GetActive();

			DirectX::XMFLOAT3 worldPos = CoreInit::managers.transformManager->GetPosition(enemyUnits[i]->GetEntity());
			DirectX::XMVECTOR worldVec = DirectX::XMVectorSet(worldPos.x, worldPos.y, worldPos.z, 1.0f);
			DirectX::XMMATRIX viewProj = DirectX::XMLoadFloat4x4(&CoreInit::managers.cameraManager->GetViewProjection(CoreInit::managers.cameraManager->GetActive()));
			DirectX::XMVECTOR screenPos = DirectX::XMVector4Transform(worldVec, viewProj);

			float w = DirectX::XMVectorGetW(screenPos);

			xPos = (int)round(((DirectX::XMVectorGetX(screenPos) / w + 1) / 2.0f) * CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Window", "Width", 1280));
			yPos = (int)round(((1 - DirectX::XMVectorGetY(screenPos) / w) / 2.0f) * CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Window", "Height", 720));

			CoreInit::managers.guiManager->SetTexturePos(hpBars[i].bar, xPos - 50, yPos - 50);
			CoreInit::managers.guiManager->SetTexturePos(hpBars[i].frame, xPos - 50, yPos - 50);

			CoreInit::managers.guiManager->SetTextureDimensions(hpBars[i].bar, barWidth * (enemyUnits[i]->GetHealth() / enemyUnits[i]->GetMaxHealth()), barHeight);
		}
		else
		{
			CoreInit::managers.guiManager->ToggleRenderableTexture(hpBars[i].bar, false);
			CoreInit::managers.guiManager->ToggleRenderableTexture(hpBars[i].frame, false);			
		}
	}

	for (int i = enemyUnits.size(); i < hpBars.size(); i++)
	{
		CoreInit::managers.guiManager->ToggleRenderableTexture(hpBars[i].bar, false);
		CoreInit::managers.guiManager->ToggleRenderableTexture(hpBars[i].frame, false);
	}

	StopProfile;
}

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
	UpdateHpBars(playerX, playerY);

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
			CoreInit::managers.eventManager->ToggleVisible(bs, true);
			//roomEntities[size_t(p.x)][size_t(p.z)].push_back(bs);

			auto spw = CoreInit::subSystems.window->GetRand() % 100;
			if (spw < 80)
			{	
				auto item = Item::Create();

				Item::Drop(item, p);
				//itemsInRoom.push_back(item);
				CoreInit::managers.dataManager->SetValue(item, "Pickup", false);
				CoreInit::managers.dataManager->SetValue(item, "Room", roomEntity);
				CoreInit::managers.eventManager->RegisterEntitytoEvent(item, "RoomChange");
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

	time += dt;
	CoreInit::subSystems.renderer->GetPipelineHandler()->UpdateConstantBuffer("fog_timeBuffer", &time, sizeof(float));

	StopProfile;
}

bool Room::CheckCollisionInRoom(float xCenterPosition, float yCenterPosition, float xExtent, float yExtent)
{
	StartProfile;
	const int xLeftFloored = int(floor(xCenterPosition - xExtent));
	const int xRightFloored = int(floor(xCenterPosition + xExtent));
	const int yUpFloored = int(floor(yCenterPosition + yExtent));
	const int yDownFloored = int(floor(yCenterPosition - yExtent));


	if (tileValues[xLeftFloored][yDownFloored] && tileValues[xLeftFloored][yDownFloored] != id_Torch)
	{
		ProfileReturnConst(true);
	}
	if (tileValues[xLeftFloored][yUpFloored] && tileValues[xLeftFloored][yUpFloored] != id_Torch)
	{
		ProfileReturnConst(true);
	}

	if (tileValues[xRightFloored][yUpFloored] && tileValues[xRightFloored][yUpFloored] != id_Torch)
	{
		ProfileReturnConst(true);
	}
	if (tileValues[xRightFloored][yDownFloored] && tileValues[xRightFloored][yDownFloored] != id_Torch)
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
			float doorDist = sqrtf((startX - DoorArr[i].posX)*(startX - DoorArr[i].posX) +
				(startY - DoorArr[i].posY)*(startY - DoorArr[i].posY));
			if(doorDist < distance)
			{
				distance = doorDist;
				direction = DirectionToAdjacentRoom(i);
			}
		}
	}
	
	return distance;
}

float SE::Gameplay::Room::DistanceToDoorInDirection(float startX, float startY, DirectionToAdjacentRoom direction) const
{
	float distX = DoorArr[int(direction)].posX - startX;
	float distY = DoorArr[int(direction)].posY - startY;


	return sqrtf(distX*distX + distY*distY);
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

	for (int i = 0; i < enemyUnits.size(); i++)
	{
		float distance = sqrt((projectile.GetXPosition() - enemyUnits[i]->GetXPosition()) * (projectile.GetXPosition() - enemyUnits[i]->GetXPosition()) + (projectile.GetYPosition() - enemyUnits[i]->GetYPosition()) * (projectile.GetYPosition() - enemyUnits[i]->GetYPosition()));
		float tempVal = projectile.GetBoundingRect().radius;
		if (!projectile.CheckIfAlreadyHit(enemyUnits[i]) && projectile.GetBoundingRect().radius != 0.0f && distance < (projectile.GetBoundingRect().radius + sqrt(enemyUnits[i]->GetExtent() * enemyUnits[i]->GetExtent() * 2)))
		{
			enemyUnits[i]->AddDamageEvent(projectile.GetProjectileDamageEvent());
			enemyUnits[i]->AddHealingEvent(projectile.GetProjectileHealingEvent());
			enemyUnits[i]->AddConditionEvent(projectile.GetProjectileConditionEvent());
			cData.type = CollisionType::ENEMY;
			cData.hitUnit = enemyUnits[i]->GetSharedPtr();
			projectile.AddToHit(enemyUnits[i]);
		}
	}

	//if ((enemyCollidedWith = PointCollisionWithEnemy(r.upperLeftX, r.upperLeftY, projectile)) != -1) //check if front left corner of projectile is in a blocked square
	//{
	//	collidedLeft = true;
	//	cData.type = CollisionType::ENEMY;
	//}
	//else if ((enemyCollidedWith = PointCollisionWithEnemy(r.upperRightX, r.upperRightY, projectile)) != -1) //check if front right corner of projectile is in a blocked square
	//{
	//	collidedRight = true;
	//	cData.type = CollisionType::ENEMY;
	//}

	//if (enemyCollidedWith != -1)
	//{
	//	enemyUnits[enemyCollidedWith]->AddDamageEvent(projectile.GetProjectileDamageEvent());
	//	enemyUnits[enemyCollidedWith]->AddHealingEvent(projectile.GetProjectileHealingEvent());
	//	enemyUnits[enemyCollidedWith]->AddConditionEvent(projectile.GetProjectileConditionEvent());
	//	cData.hitUnit = enemyUnits[enemyCollidedWith]->GetSharedPtr();
	//}

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
	char right = tileValues[x + 1][y];
	char left = tileValues[x - 1][y];
	char up = tileValues[x][y + 1];
	char down = tileValues[x][y - 1];
	if (x == 24 || y == 24 || x == 0 || y == 0)
	{
		
		if ((x == 24 && y != 0 && y != 24))
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
			else if (up == (char)13 || up == (char)76)
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
			else if (down == (char)13 || down == (char)76)
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
			else
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::OneSide] });
				CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
			}
		}
		else if ((y == 24 && x != 0 && x != 24))
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
			else if (right == (char)13 || right == (char)76)
			{
				if (side != 7 || side != 35)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
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
			else if (left ==  (char)13 || left == (char)76)
			{			
				if (side != 7 || side != 14)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
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
		else if ((x == 0 && y != 24 && y != 0))
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
			else if (down == (char)13 || down == (char)76)
			{	
				if (side == 1)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::HighWall] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 3)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
				}
				else if (side == 5)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 15)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
			}
			else if (up == (char)13 || up == (char)76)
			{
				if (side == 1)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::HighWall] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 7)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
				}
				else if (side == 5)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 35)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PIDIV2, 0.0);
				}
			}
			else
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
				CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
			}
		}
		else if ((y == 0 && x != 24 && x != 0))
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
			else if (left == (char)13 || left == (char)76)
			{
				if (side == 1)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::HighWall] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 5)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 3)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
				}
				else if (side == 15)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
			}
			else if (right == (char)13 || right == (char)76)
			{
				if (side == 1)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::HighWall] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, 0.0, 0.0);
				}
				else if (side == 2)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, DirectX::XM_PI, 0.0);
				}
				else if (side == 3)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::ThreeSides] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
				}
				else if (side == 6)
				{
					CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Corner] });
					CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
				}
			}
			else
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::Passage] });
				CoreInit::managers.transformManager->SetRotation(ent, 0.0, -DirectX::XM_PIDIV2, 0.0);
			}
		}
		else if ((x == 0 && y == 0) || (x == 24 && y == 0) || (x == 0 && y == 24) || (x == 24 && y == 24))
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(ent, { Meshes[Meshes::HighWall] });
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
	//if (!(x == 24 || y == 24) && (right == 0 || up == 0 || tileValues[x + 1][y + 1] == 0 || tileValues[x - 1][y + 1] == 0 || tileValues[x + 1][y - 1] == 0))
	//	return true;
	return false;
}

void SE::Gameplay::Room::RandomizeWallAndFloorTexture(SE::Utilz::GUID & wallGuid, SE::Utilz::GUID &floorGuid)
{
	auto rand = CoreInit::subSystems.window->GetRand();
	auto randNr = (rand % 6);

	if (IsOutside == true)
	{
		wallGuid = Materials[Materials::OutsideWall]; 
	}
	else
	{
		switch (randNr)
		{
		case 0:
			wallGuid = Materials[Materials::julWall];
			break;
		case 1:
			wallGuid = Materials[Materials::FanzyWall];
			break;
		case 2:
			wallGuid = Materials[Materials::LightStoneWall];
			break;
		case 3:
			wallGuid = Materials[Materials::LightStoneWallWood];
			break;
		case 4:
			wallGuid = Materials[Materials::DarkStoneWall]; 
			break; 
		case 5:
			wallGuid = Materials[Materials::julWall]; 

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
		floorGuid = Materials[Materials::StoneFloor2];
		break;
	case 2:
		floorGuid = Materials[Materials::WoodFloor];
		break;
	}
	
}

void SE::Gameplay::Room::CreateEntities()
{
	int numberOfEntitesPlaced = 0;
	Core::IMaterialManager::CreateInfo cubeInfo;
	
	int DoorCounter = 0;
	int counter = 0;




	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
		{
			if (tileValues[x][y] != id_DeadArea)
			{
				auto ent = CoreInit::managers.entityManager->Create();
				CoreInit::managers.transformManager->Create(ent);
				CoreInit::managers.transformManager->SetPosition(ent, DirectX::XMFLOAT3(x + 0.5f, 0.0f, y + 0.5f));

				CreationArguments args{ ent, x, y, DoorCounter, cubeInfo, wallTexture, floorTexture };
				auto exists = propItemToFunction.find(tileValues[x][y]);
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
	for(int x = 0; x < 25; x++)
		for (int y = 0; y < 25; y++)
			for (auto& e : roomEntities[x][y])
			{
				CoreInit::managers.eventManager->ToggleVisible(e, render);
				CoreInit::managers.eventManager->ToggleShadow(e, render);
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

	for (auto& hpBar : hpBars)
	{
		CoreInit::managers.guiManager->ToggleRenderableTexture(hpBar.frame, render);
		CoreInit::managers.guiManager->ToggleRenderableTexture(hpBar.bar, render);
	}

	beingRendered = render;

	fog.Enable(render);
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

#pragma region MeshesAndMaterials

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
	Meshes[Meshes::Window] = { "Window.mesh" };
	Meshes[Meshes::Window_open] = { "WindowOpen.mesh" };
	Meshes[Meshes::Window_closed] = { "WindowClosed.mesh" };
	Meshes[Meshes::Fireplace] = { "Fireplace.mesh" };
	Meshes[Meshes::Fireplace_open] = { "FireplaceOpen.mesh" };
	Meshes[Meshes::Tree] = { "tree.mesh" }; 
	Meshes[Meshes::Well] = { "well.mesh" };
	Meshes[Meshes::Fireplace_set1] = { "Fireplace_set1.mesh" };
	Meshes[Meshes::Fireplace_set2] = { "Fireplace_set2.mesh" };
	Meshes[Meshes::MiniTable_set1] = { "MiniTable_set1.mesh" };
	Meshes[Meshes::Potatobag_set1] = { "Potatobag_set1.mesh" };
	Meshes[Meshes::PotFloorTorch_set1] = { "PotFloorTorch_set1.mesh" };

	Meshes[Meshes::BagPipeTable_set1] = { "BagPipeTable_set1.mesh" };
	Meshes[Meshes::DinnerTable_set1] = { "DinnerTable_set1.mesh" };
	Meshes[Meshes::Smalltable_crossbowAndBat_set1] = { "Smalltable_crossbowAndBat_set1.mesh" };
	Meshes[Meshes::Smalltable_weapons_set1] = { "Smalltable_weapons_set1.mesh" };
	Meshes[Meshes::statue] = { "statue.mesh" };


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
	Materials[Materials::StoneFloor2] = { "StoneFloor2.mat" };
	Materials[Materials::WoodFloor] = { "WoodFloor.mat" };
	Materials[Materials::FanzyWall] = { "FanzyWall.mat" };
	Materials[Materials::LightStoneWall] = { "LightStoneWall.mat" };
	Materials[Materials::LightStoneWallWood] = { "LightStoneWallWood.mat" };
	Materials[Materials::julWall] = { "julWall.mat" }; 
	Materials[Materials::DarkStoneWall] = { "DarkStoneWall.mat" };
	Materials[Materials::Window] = { "WindowOpen.mat" };
	Materials[Materials::Fireplace] = { "Fireplace.mat" };
	Materials[Materials::Pillar] = { "Pillar_short.mat" };
	Materials[Materials::PotatosackOpen] = { "Potato_Sack_Open.mat" };
	Materials[Materials::PotatosackClosed] = { "Potato_Sack_Closed.mat" };
	Materials[Materials::Well] = { "well.mat" }; 
	Materials[Materials::Fireplace_set1] = { "Fireplace_set1.mat" };
	Materials[Materials::Fireplace_set2] = { "Fireplace_set2.mat" };
	Materials[Materials::MiniTable_set1] = { "MiniTable_set1.mat" };
	Materials[Materials::Potatobag_set1] = { "Potatobag_set1.mat" };
	Materials[Materials::PotFloorTorch_set1] = { "PotFloorTorch_set1.mat" };
	Materials[Materials::BagPipeTable_set1]              = { "BagPipeTable_set1.mat"};
	Materials[Materials::DinnerTable_set1]               = { "DinnerTable_set1.mat"};
	Materials[Materials::Smalltable_crossbowAndBat_set1] = { "Smalltable_crossbowAndBat_set1.mat" };
	Materials[Materials::Smalltable_weapons_set1]        = { "Smalltable_weapons_set1.mat"};
	Materials[Materials::statue]                         = { "statue.mat"};


#pragma endregion


#pragma region RNGprops

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
	PotatoSackOpen.matGuid = Materials[Materials::PotatosackOpen];

	Prop PotatoSackClosed;
	PotatoSackClosed.guid = Meshes[Meshes::Potatosack_closed];
	PotatoSackClosed.matGuid = Materials[Materials::PotatosackClosed];

	Prop Bush;
	Bush.guid = Meshes[Meshes::Bush];
	Bush.matGuid = Materials[Materials::Bush];

	Prop Fireplace;
	Fireplace.guid = Meshes[Meshes::Fireplace];
	Fireplace.matGuid = Materials[Materials::Fireplace];

	Prop FireplaceOpen;
	FireplaceOpen.guid = Meshes[Meshes::Fireplace_open];
	FireplaceOpen.matGuid = Materials[Materials::Fireplace];


	Prop Tree; 
	Tree.guid = Meshes[Meshes::Tree]; 
	Tree.matGuid = Materials[Materials::Wood]; 


	Prop Fireplace_set1;
	Fireplace_set1.guid = Meshes[Meshes::Fireplace_set1];
	Fireplace_set1.matGuid = Materials[Materials::Fireplace_set1];

	Prop Fireplace_set2;
	Fireplace_set2.guid = Meshes[Meshes::Fireplace_set2];
	Fireplace_set2.matGuid = Materials[Materials::Fireplace_set2];

	Prop MiniTable_set1;
	MiniTable_set1.guid = Meshes[Meshes::MiniTable_set1];
	MiniTable_set1.matGuid = Materials[Materials::MiniTable_set1];

	Prop Potatobag_set1;
	Potatobag_set1.guid = Meshes[Meshes::Potatobag_set1];
	Potatobag_set1.matGuid = Materials[Materials::Potatobag_set1];

	Prop PotFloorTorch_set1;
	PotFloorTorch_set1.guid = Meshes[Meshes::PotFloorTorch_set1];
	PotFloorTorch_set1.matGuid = Materials[Materials::PotFloorTorch_set1];


	Prop BagPipeTable_set1;
	BagPipeTable_set1.guid = Meshes[Meshes::BagPipeTable_set1];
	BagPipeTable_set1.matGuid = Materials[Materials::BagPipeTable_set1];

	Prop DinnerTable_set1;
	DinnerTable_set1.guid = Meshes[Meshes::DinnerTable_set1];
	DinnerTable_set1.matGuid = Materials[Materials::DinnerTable_set1];

	Prop Smalltable_crossbowAndBat_set1;
	Smalltable_crossbowAndBat_set1.guid = Meshes[Meshes::Smalltable_crossbowAndBat_set1];
	Smalltable_crossbowAndBat_set1.matGuid = Materials[Materials::Smalltable_crossbowAndBat_set1];

	Prop Smalltable_weapons_set1;
	Smalltable_weapons_set1.guid = Meshes[Meshes::Smalltable_weapons_set1];
	Smalltable_weapons_set1.matGuid = Materials[Materials::Smalltable_weapons_set1];

	Prop statue;
	statue.guid = Meshes[Meshes::statue];
	statue.matGuid = Materials[Materials::Stone];


#pragma endregion



	// 4x4 tile props - add more here
	propVectors[PropTypes::BIGPROPS] = { TableGroup1 };
	propVectors[PropTypes::TABLES]   = { Table_small, Table_round };
	propVectors[PropTypes::MEDIUM]   = { Table_long, Table_long, Table_long,  CandleStick_tri, CandleStick_tri,  Fireplace, FireplaceOpen, Fireplace_set1, Fireplace_set2, DinnerTable_set1, DinnerTable_set1 };
	propVectors[PropTypes::BUSHES]   = { Bush };

	// 1x1 tile props // Add more props here
	propVectors[PropTypes::GENERIC] =
	{ Table_small,
		Table_round,
		PotGroup1,
		PotatoSackClosed,
		PotatoSackOpen,
		FloorTorch,
		MiniTable_set1,
		Potatobag_set1,
		PotFloorTorch_set1,
		BagPipeTable_set1,
		Smalltable_crossbowAndBat_set1,
		Smalltable_weapons_set1,
		statue
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
	propItemToFunction[id_Tree] = [this](CreationArguments &args) {
		this->CreateTree(args);
	};
	propItemToFunction[id_Window] = [this](CreationArguments &args) {
		this->CreateWindows(args);
	};




	pos start;
	loadfromFile(fileName);
	RandomizeWallAndFloorTexture(wallTexture, floorTexture);
	//	CreateEntities();

		// reset temporary tilevalues where a single prop is overlapping another tile 
		//ResetTempTileValues();


	//Used for flowField creation
	char tempMap[25][25];
	for (int x = 0; x < 25; x++)
		for (int y = 0; y < 25; y++)
		{
			if(!(tempMap[x][y] = tileValues[x][y] == id_Torch ? id_Floor : tileValues[x][y]))
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


	roomField = new FlowField(tempMap, 1.0f, start, 0.0f, 0.0f);

	roomEntity = CoreInit::managers.entityManager->Create();


	StopProfile;
}

Room::~Room()
{
	CoreInit::managers.entityManager->Destroy(roomEntity);
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
	loaded = true;
}

void SE::Gameplay::Room::Unload()
{
	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
		{
			for (auto e : roomEntities[x][y])
			{
				CoreInit::managers.entityManager->Destroy(e);
			}
			roomEntities[x][y].clear();
		}
	}
	 
	loaded = false;
	for (auto enemy : enemyUnits)
	{
		if (auto weapon = std::get_if<Core::Entity>(&CoreInit::managers.dataManager->GetValue(enemy->GetEntity(), "Weapon", false)))
		{
			CoreInit::managers.entityManager->Destroy(*weapon);
		}
		enemy->DestroyEntity();
		

	}
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
	case DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_EAST: newX -= 1.5f; break;
	case DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH: newY += 1.5f; break;
	case DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST: newX += 1.5f; break;
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
	if (enemyToRemove == nullptr)
	{
		for (auto enemy : enemyUnits)
			enemy->SetHealth(-1);
	}
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

		posX = DoorArr[int(door)].posX;
		posY = DoorArr[int(door)].posY;
		return true;
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
				char hej = tileValues[x][y];
				if (in[counter] == id_Bush)
				{
					IsOutside = true;
				}
				counter++; 
			}
		}

		fog.Instantiate(tileValues, &time);
	
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});
	
	StopProfile; 
}

float Room::RotatePainting(int x, int y) {
	StartProfile;
	float rotation = 0;


	if ( x < 24 && (tileValues[x + 1][y] == id_Floor))
		rotation = 180;
	else if ( y < 24 && (tileValues[x][y + 1] == id_Floor ))
		rotation = 90;
	else if ( y > 0 &&(tileValues[x][y - 1] == id_Floor ))
		rotation = -90;
	else if ( x > 0  && (tileValues[x - 1][y] == id_Floor ))
		rotation = 0;


	rotation += 270;

	rotation *= 3.1416 / 180;
	ProfileReturnConst(rotation);

}
float Room::WallCheck(int x, int y, float &retX, float &retZ)
{
	StartProfile;
	float rotation = 0;


	if (x - 1 >= 0 && (tileValues[x - 1][y] == id_Wall || tileValues[x - 1][y] == id_Pillar)) {
		retX = 0.40;
		retZ = 0.50;
		rotation = 180;
	}
	else if (y - 1 >= 0 && (tileValues[x][y - 1] == id_Wall || tileValues[x][y - 1] == id_Pillar)) {
		retX = 0.50;
		retZ = 0.40;
		rotation = 90;																
	}
	else if (y + 1 < 25 && (tileValues[x][y + 1] == id_Wall || tileValues[x][y + 1] == id_Pillar)) {
		retX = 0.50;
		retZ = 0.60;
		rotation = -90;																
	}
	else if (x + 1 < 25 && (tileValues[x + 1][y] == id_Wall || tileValues[x + 1][y] == id_Pillar)) {
		retX = 0.60;
		retZ = 0.50;
		rotation = 0;
	}


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
		args2.x = x + 1;
		args2.y = y;
		CreateFloor(args2);


		while (propCheck == false)
		{
			nrOfProps = propVectors[PropTypes::MEDIUM].size();
			rand = CoreInit::subSystems.window->GetRand();
			propId = (rand % nrOfProps);

			if (propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace] ||
				propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace_open] ||
				propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace_set1] ||
				propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace_set2]) {
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
		args2.x = x;
		args2.y = y + 1;
		CreateFloor(args2);
		
		while (propCheck == false)
		{
			nrOfProps = propVectors[PropTypes::MEDIUM].size();
			rand = CoreInit::subSystems.window->GetRand();
			propId = (rand % nrOfProps);
			rot = 1.5708;

			if (propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace] ||
				propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace_open] ||
				propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace_set1] ||
				propVectors[PropTypes::MEDIUM][propId].guid == Meshes[Meshes::Fireplace_set2]) {
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

		args2.x = x;
		args2.y = y + 1;
		CreateFloor(args2);

		args2.x = x + 1;
		args2.y = y;
		CreateFloor(args2);

		args2.x = x + 1;
		args2.y = y + 1;
		CreateFloor(args2);
		
		auto nrOfProps = propVectors[PropTypes::BIGPROPS].size();
		auto rand = CoreInit::subSystems.window->GetRand();
		auto propId = (rand % nrOfProps);

		ret.guid = propVectors[PropTypes::BIGPROPS][propId].guid;
		ret.matGuid = propVectors[PropTypes::BIGPROPS][propId].matGuid;


		ProfileReturn(ret);
	}

	// 1x1 tile props

	if (IsOutside)
	{
		ret.guid = Meshes[Meshes::Well];
		ret.matGuid = Materials[Materials::Well];
	}
	else
	{
		nrOfProps = propVectors[PropTypes::GENERIC].size();
		rand = CoreInit::subSystems.window->GetRand();
		propId = (rand % nrOfProps);

		ret.guid = propVectors[PropTypes::GENERIC][propId].guid;
		ret.matGuid = propVectors[PropTypes::GENERIC][propId].matGuid;

	}
	
	if (ret.guid == Meshes[Meshes::FloorTorch])
	{
		CreateFire(x, y);
	}

	ProfileReturn(ret);
}

void SE::Gameplay::Room::CreateFire(int x, int y) 
{

	auto entFire = CoreInit::managers.entityManager->Create();
	CoreInit::managers.transformManager->Create(entFire);
	CoreInit::managers.transformManager->SetPosition(entFire, DirectX::XMFLOAT3(x + 0.5f, 0.5f, y + 0.5f));
	SE::Core::IParticleSystemManager::CreateInfo info;
	info.systemFile = Utilz::GUID("floorTorchFire.pts");
	CoreInit::managers.particleSystemManager->CreateSystem(entFire, info);
	//CoreInit::managers.particleSystemManager->ToggleVisible(entFire, true);
	roomEntities[x][y].push_back(entFire);

}
bool SE::Gameplay::Room::TorchOnWall(int x, int y)
{
	StartProfile;
	
	bool torchFound = false;

	if (x - 1 >= 0 && tileValues[x - 1][y] == id_Torch ) {
		
		torchFound = true;
	}
	else if (y - 1 >= 0 && tileValues[x][y - 1] == id_Torch) {
		
		torchFound = true;
	}
	else if (y + 1 < 25 && tileValues[x][y + 1] == id_Torch) {
		
		torchFound = true;
	}
	else if (x + 1 < 25 && tileValues[x + 1][y] == id_Torch) {
		
		torchFound = true;
	}

	
	ProfileReturnConst(torchFound);
}
void SE::Gameplay::Room::CreateWindows(CreationArguments & args)
{
	int x = args.x;
	int y = args.y;
	SE::Utilz::GUID WindowGUID = Meshes[Meshes::Window_open];

	float rotation = 0;


	if (x < 24 && (tileValues[x + 1][y] == id_Floor))
		rotation = 180;
	else if (y < 24 && (tileValues[x][y + 1] == id_Floor))
		rotation = 90;
	else if (y > 0 && (tileValues[x][y - 1] == id_Floor)) {
		rotation = -90;
		WindowGUID = Meshes[Meshes::Window_closed];
	}
	else if (x > 0 && (tileValues[x - 1][y] == id_Floor))
		rotation = 0;


	rotation += 270;

	rotation *= 3.1416 / 180;


	Core::IMaterialManager::CreateInfo matInfo;
	matInfo.materialFile = Materials[Materials::Window];
	matInfo.shader = Norm;
	CoreInit::managers.materialManager->Create(args.ent, matInfo);
	CoreInit::managers.transformManager->SetPosition(args.ent, DirectX::XMFLOAT3(args.x + 0.5f, 1.5f, args.y + 0.5f));
	CoreInit::managers.transformManager->SetRotation(args.ent, 0.0f, rotation, 0.0f);
	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { WindowGUID });

	roomEntities[args.x][args.y].push_back(args.ent);


	// Base window mesh (wall)
	auto entWindow = CoreInit::managers.entityManager->Create();
	matInfo.materialFile = args.wallMat;
	CoreInit::managers.transformManager->Create(entWindow);
	CoreInit::managers.transformManager->SetPosition(entWindow, DirectX::XMFLOAT3(args.x + 0.5f, 1.5f, args.y + 0.5f));
	CoreInit::managers.transformManager->SetRotation(entWindow, 0.0f, rotation, 0.0f);
	CoreInit::managers.renderableManager->CreateRenderableObject(entWindow, { Meshes[Meshes::Window] });
	CoreInit::managers.materialManager->Create(entWindow, matInfo);

	roomEntities[args.x][args.y].push_back(entWindow);

}

bool Room::IsWall(int x, int y) const
{
	if(x < 25 && x >= 0 && y < 25 && y >= 0)
	{
		if (tileValues[x][y] == id_Wall)
			return true;
	}
	return false;
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
	roomEntities[args.x][args.y].push_back(args.ent);

	CreateFloor(args);
}

void SE::Gameplay::Room::CreateTree(CreationArguments &args)
{
	Core::IMaterialManager::CreateInfo matInfo;
	matInfo.materialFile = Materials[Materials::Wood];
	matInfo.shader = Norm;
	CoreInit::managers.materialManager->Create(args.ent, matInfo);
	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { Meshes[Meshes::Tree] });

	roomEntities[args.x][args.y].push_back(args.ent);

	CreateFloor(args);
}

void SE::Gameplay::Room::CreateFloor(CreationArguments &args)
{
	Core::IMaterialManager::CreateInfo matInfo;

	// Create floor
	auto entFloor = CoreInit::managers.entityManager->Create();

	CoreInit::managers.transformManager->Create(entFloor);
	CoreInit::managers.transformManager->SetPosition(entFloor, DirectX::XMFLOAT3(args.x + 0.5f, 0.0f, args.y + 0.5f));
	CoreInit::managers.renderableManager->CreateRenderableObject(entFloor, { Meshes[Meshes::Floor] });

	if (tileValues[args.x][args.y] == id_Bush) // bush 
	{
		matInfo.materialFile = Materials[Materials::Dirt];
	}
	else if (IsOutside)
	{
		matInfo.materialFile = Materials[Materials::Grass];
	}
	else
	{
		matInfo.materialFile = args.floorMat;
		

		const Utilz::GUID carpetTextures[] = { "CowWhite.png", "CowBlack.png", "CowBrown.png", "bear.png" };
		const size_t carpetTexturesCount = sizeof(carpetTextures) / sizeof(*carpetTextures);
		const uint32_t carpetToUse = std::rand() % carpetTexturesCount;



		if (tileValues[args.x][args.y] == id_Door1) 
		{
			Core::DecalCreateInfo decalFloorInfo;
			decalFloorInfo.opacity = 1.0f;
			decalFloorInfo.ambiance = 0.1f;
			decalFloorInfo.textureName = carpetTextures[carpetToUse];
			const DirectX::XMFLOAT3 floorForward = CoreInit::managers.transformManager->GetForward(entFloor);
			const DirectX::XMFLOAT3 doorForward = CoreInit::managers.transformManager->GetRight(args.ent);


			DirectX::XMFLOAT4X4 floorDecalTrans;
			DirectX::XMMATRIX floorDecalTranslation = DirectX::XMMatrixTranslation(doorForward.x * 1.5f, 0.1f * floorForward.y, 1.5f * doorForward.z);
			DirectX::XMMATRIX floorDecalScaling = DirectX::XMMatrixScaling(2.00f, 2.00f, 1.05f);
			DirectX::XMMATRIX floorDecalRotation = DirectX::XMMatrixRotationX(DirectX::XM_PI / 2);

			CoreInit::managers.decalManager->Create(entFloor, decalFloorInfo);

			if (doorForward.x < 0.0f) {

				floorDecalRotation *= DirectX::XMMatrixRotationY(FloorCheck(args.x, args.y));
			}

			DirectX::XMStoreFloat4x4(&floorDecalTrans, floorDecalScaling * floorDecalRotation * floorDecalTranslation);

			CoreInit::managers.decalManager->SetLocalTransform(entFloor, (float*)&floorDecalTrans);
		}

		auto rand = CoreInit::subSystems.window->GetRand();
		int randValue = (rand % 500);

		if (tileValues[args.x][args.y] == id_Floor && 
			tileValues[args.x][args.y + 1] == id_Floor && 

			tileValues[args.x][args.y - 1] == id_Floor && 
			tileValues[args.x +1][args.y + 1] == id_Floor &&
			tileValues[args.x +1][args.y -1] == id_Floor &&
			tileValues[args.x + 1][args.y] == id_Floor && 

			tileValues[args.x - 1][args.y] == id_Floor && 
			tileValues[args.x -1][args.y - 1] == id_Floor && 
			tileValues[args.x - 1][args.y + 1] == id_Floor)



		{
			if (0 < randValue && randValue <= 20)
			{
					Core::DecalCreateInfo decalFloorInfo;
					decalFloorInfo.opacity = 1.0f;
					decalFloorInfo.ambiance = 0.1f;
					decalFloorInfo.textureName = carpetTextures[carpetToUse];
					const DirectX::XMFLOAT3 floorForward = CoreInit::managers.transformManager->GetForward(entFloor);

					DirectX::XMFLOAT4X4 floorDecalTrans;
					DirectX::XMMATRIX floorDecalTranslation = DirectX::XMMatrixTranslation(0.5f, 0.1f * floorForward.y, 0.5f);
					DirectX::XMMATRIX floorDecalScaling = DirectX::XMMatrixScaling(2.00f, 2.00f, 0.05f);
					DirectX::XMMATRIX floorDecalRotationX = DirectX::XMMatrixRotationX(DirectX::XM_PI / 2);
					DirectX::XMMATRIX floorDecalRotationY = DirectX::XMMatrixRotationY(randValue);

					CoreInit::managers.decalManager->Create(entFloor, decalFloorInfo);

					DirectX::XMStoreFloat4x4(&floorDecalTrans, (floorDecalScaling) * (floorDecalRotationX *floorDecalRotationY) * floorDecalTranslation);

					CoreInit::managers.decalManager->SetLocalTransform(entFloor, (float*)&floorDecalTrans);

			
			}
		}
		
	}

	
	if (IsOutside == false)
	{
		matInfo.shader = Norm;
	}
	else
	{
		matInfo.shader = BushShader;
	}

	CoreInit::managers.materialManager->Create(entFloor, matInfo);
	//CoreInit::managers.renderableManager->ToggleRenderableObject(entFloor, true);
	roomEntities[args.x][args.y].push_back(entFloor);


	//if (tileValues[args.x][args.y] == id_Torch )
	//{
	//	tileValues[args.x][args.y] = id_Floor;
	//}

}

void SE::Gameplay::Room::CreateTorch(CreationArguments &args)
{
	auto entFire = CoreInit::managers.entityManager->Create();

	float x;
	float z;

	auto ignoreMe = WallCheck(args.x, args.y, x, z);
	CoreInit::managers.transformManager->Create(entFire);
	CoreInit::managers.transformManager->SetPosition(entFire, DirectX::XMFLOAT3(args.x + x, 2.18f, args.y + z));
	SE::Core::IParticleSystemManager::CreateInfo info;
	info.systemFile = Utilz::GUID("torchFlame.pts");
	CoreInit::managers.particleSystemManager->CreateSystem(entFire, info);
	//CoreInit::managers.particleSystemManager->ToggleVisible(entFire, true);
	Core::ILightManager::CreateInfo ci;
	ci.pos = { 0,0,0 };
	ci.color = { 0.85f, 0.67f, 0.24f };
	ci.radius = 5.0f;
	ci.intensity = 1.5f;
	CoreInit::managers.lightManager->Create(entFire, ci);
	roomEntities[args.x][args.y].push_back(entFire);


	// Create torch
	Core::IMaterialManager::CreateInfo matInfo;
	matInfo.materialFile = Materials[Materials::FloorStone];
	matInfo.shader = Norm;
	CoreInit::managers.materialManager->Create(args.ent, matInfo);
	CoreInit::managers.transformManager->SetRotation(args.ent, 0, WallCheck(args.x, args.y, x, z), 0);
	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { Meshes[Meshes::Torch] });
	//CoreInit::managers.renderableManager->ToggleRenderableObject(args.ent, true);

	roomEntities[args.x][args.y].push_back(args.ent);


	CreateFloor(args);
}

void SE::Gameplay::Room::CreatePillar(CreationArguments &args)
{
	Core::IMaterialManager::CreateInfo matInfo;
	matInfo.materialFile = Materials[Materials::Pillar];
	matInfo.shader = Norm;
	CoreInit::managers.materialManager->Create(args.ent, matInfo);
	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { Meshes[Meshes::Pillar_short] });

	roomEntities[args.x][args.y].push_back(args.ent);

	CreateFloor(args);
}

void SE::Gameplay::Room::CreateProp(CreationArguments &args)
{
	int i = args.x;
	int j = args.y;

	float rot;
	Prop prop = GenerateRandomProp(i, j, args, rot);

	Core::IMaterialManager::CreateInfo matInfo;
	matInfo.materialFile = prop.matGuid;
	matInfo.shader = Norm;
	CoreInit::managers.materialManager->Create(args.ent, matInfo);
	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { prop.guid });
//	CoreInit::managers.renderableManager->ToggleRenderableObject(args.ent, true);

	roomEntities[args.x][args.y].push_back(args.ent);


	CreateFloor(args);

	if (prop.guid == Meshes[Meshes::statue])
	{
		float dummyX, dummyY;
		CoreInit::managers.transformManager->Create(args.ent);
		CoreInit::managers.transformManager->SetRotation(args.ent, 0.0f, WallCheck(args.x, args.y, dummyX, dummyY), 0.0f);
	}

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

	CreateWall(args.ent, args.x, args.y);
	matInfo.shader = Norm;

	CoreInit::managers.transformManager->SetPosition(args.ent, DirectX::XMFLOAT3(args.x + 0.5f, 1.5f, args.y + 0.5f));

	CoreInit::managers.materialManager->Create(args.ent, matInfo);

	roomEntities[args.x][args.y].push_back(args.ent);

	auto rand = CoreInit::subSystems.window->GetRand();
	int randValue = (rand % 500); // 2% chance a wall will have painting

	if ((0 < randValue && randValue <= 10) && !TorchOnWall(args.x, args.y))
	{
		const auto PaintingEnt = CoreInit::managers.entityManager->Create();
		Core::IMaterialManager::CreateInfo matInfoPainting;
		matInfoPainting.shader = Norm;
		matInfoPainting.materialFile = Materials[Materials::Wood];
		CoreInit::managers.transformManager->Create(PaintingEnt);
		CoreInit::managers.transformManager->SetPosition(PaintingEnt, DirectX::XMFLOAT3(args.x + 0.5f, 1.0f, args.y + 0.5f));
		CoreInit::managers.transformManager->SetRotation(PaintingEnt, 0.0f, RotatePainting(args.x, args.y), 0.0f);
		CoreInit::managers.renderableManager->CreateRenderableObject(PaintingEnt, { Meshes[Meshes::Painting] });
		CoreInit::managers.materialManager->Create(PaintingEnt, matInfoPainting);
		//CoreInit::managers.renderableManager->ToggleRenderableObject(test, true);
		
		const Utilz::GUID paintingTextures[] = { "painting1.png", "painting2.png" };
		const size_t paintingTexturesCount = sizeof(paintingTextures) / sizeof(*paintingTextures);
		const uint32_t paintingToUse = std::rand() % paintingTexturesCount;

		Core::DecalCreateInfo decalInfo;
		decalInfo.opacity = 1.0f;
		decalInfo.textureName = paintingTextures[paintingToUse];

		CoreInit::managers.decalManager->Create(PaintingEnt, decalInfo);
		//CoreInit::managers.decalManager->ToggleVisible(PaintingEnt, true);

		const DirectX::XMFLOAT3 paintingForward = CoreInit::managers.transformManager->GetForward(PaintingEnt);

		DirectX::XMFLOAT4X4 decalTrans;
		const DirectX::XMMATRIX decalTranslation = DirectX::XMMatrixTranslation(0.0f + paintingForward.x * 0.25f, 0.75f, 0.0f + paintingForward.z * 0.25f);
		const DirectX::XMMATRIX decalScaling = DirectX::XMMatrixScaling(0.945f, 1.14f, 0.65f);
		DirectX::XMStoreFloat4x4(&decalTrans, decalScaling * decalTranslation);

		CoreInit::managers.decalManager->SetLocalTransform(PaintingEnt, (float*)&decalTrans);

		roomEntities[args.x][args.y].push_back(PaintingEnt);

	}

}

void SE::Gameplay::Room::CreateDoor(CreationArguments & args)
{
	Core::IMaterialManager::CreateInfo matInfo;
	int i = args.x;
	int j = args.y;

	CoreInit::managers.renderableManager->CreateRenderableObject(args.ent, { Meshes[Meshes::Door] });
	CoreInit::managers.transformManager->SetRotation(args.ent, 0.0f, FloorCheck(i, j), 0.0f);

		CreateFloor(args);

		matInfo.materialFile = Materials[Materials::DoorMat];
		matInfo.shader = Norm;


		CoreInit::managers.materialManager->Create(args.ent, matInfo);
		roomEntities[args.x][args.y].push_back(args.ent);

		Core::ILightManager::CreateInfo createInfo;
		createInfo.color = DirectX::XMFLOAT3(0.8f, 0.0f, 0.8f);
		createInfo.pos = { args.x + 0.5f, 1.0f, args.y + 0.5f };
		createInfo.intensity = 5.0f;
		createInfo.radius = 3.0f;

		auto lightEnt = CoreInit::managers.entityManager->Create();
		CoreInit::managers.transformManager->Create(lightEnt, { args.x + 0.5f, 1.0f, args.y + 0.5f });
		CoreInit::managers.lightManager->Create(lightEnt, createInfo);
		roomEntities[args.x][args.y].push_back(lightEnt);

		Core::IParticleSystemManager::CreateInfo createPartInfo;
		createPartInfo.systemFile = "voidrift.pts";

		CoreInit::managers.particleSystemManager->CreateSystem(lightEnt, createPartInfo);

		roomEntities[args.x][args.y].push_back(lightEnt);

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
	for (int x = 0; x < 25; x++)
	{
		for (int y = 0; y < 25; y++)
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

void SE::Gameplay::Room::ToggleRenderingOfWallsAndFloor(bool toggle)
{
	for (int x = 0; x < 25; x++)
		for (int y = 0; y < 25; y++)
			for (auto& e : roomEntities[x][y])
				CoreInit::managers.eventManager->ToggleVisible(e, toggle);
}

void Room::CloseDoor(SE::Gameplay::Room::DirectionToAdjacentRoom DoorNr)
{
	if (DoorArr[size_t(DoorNr)].active)
	{
		DoorArr[size_t(DoorNr)].active = false;
		tileValues[DoorArr[size_t(DoorNr)].tileX][DoorArr[size_t(DoorNr)].tileY] = id_Wall;
		if (loaded)
		{
			for (auto& e : roomEntities[DoorArr[size_t(DoorNr)].tileX][DoorArr[size_t(DoorNr)].tileY])
				CoreInit::managers.entityManager->Destroy(e);
			roomEntities[DoorArr[size_t(DoorNr)].tileX][DoorArr[size_t(DoorNr)].tileY].clear();
			
			Core::IMaterialManager::CreateInfo cubeInfo;

			int DoorCounter = 0;

			auto ent = CoreInit::managers.entityManager->Create();
			CoreInit::managers.transformManager->Create(ent);
			CoreInit::managers.transformManager->SetPosition(ent, DirectX::XMFLOAT3(DoorArr[size_t(DoorNr)].tileX + 0.5f, 0.0f, DoorArr[size_t(DoorNr)].tileY + 0.5f));

			CreationArguments args{ ent, DoorArr[size_t(DoorNr)].tileX, DoorArr[size_t(DoorNr)].tileY, DoorCounter, cubeInfo, wallTexture, floorTexture };
			CreateWall2(args);
			if (beingRendered)
			{
				for (auto& e : roomEntities[DoorArr[size_t(DoorNr)].tileX][DoorArr[size_t(DoorNr)].tileY])
					CoreInit::managers.eventManager->ToggleVisible(e, beingRendered);
			}
		}


		
	//	Utilz::GUID temp;

	//	if (DoorArr[int(DoorNr)].side == Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH || DoorArr[int(DoorNr)].side == Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST)
	//		temp = "SimpleNormTransPS.hlsl";
	//	else
	//		temp = "SimpleNormPS.hlsl";

	//	DoorArr[int(DoorNr)].active = false;
	/////	CoreInit::managers.renderableManager->Destroy(roomEntities[DoorArr[int(DoorNr)].doorEntityPos]);
	//	CoreInit::managers.renderableManager->CreateRenderableObject(roomEntities[DoorArr[int(DoorNr)].doorEntityPos], { "HighWall.mesh" });
	//	CoreInit::managers.materialManager->Create(roomEntities[DoorArr[int(DoorNr)].doorEntityPos], {  temp, "HighWall.mat" });
	//	CoreInit::managers.transformManager->Move(roomEntities[DoorArr[int(DoorNr)].doorEntityPos], DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	////	CoreInit::managers.entityManager->DestroyNow(roomEntities[DoorArr[int(DoorNr)].doorEntityPos]);

	}

}

void SE::Gameplay::Room::OpenDoor(DirectionToAdjacentRoom DoorNr)
{
	if (!DoorArr[int(DoorNr)].active)
	{
		DoorArr[size_t(DoorNr)].active = true;
		tileValues[DoorArr[size_t(DoorNr)].tileX][DoorArr[size_t(DoorNr)].tileY] = id_Door1;
		if (loaded)
		{
			for (auto& e : roomEntities[DoorArr[size_t(DoorNr)].tileX][DoorArr[size_t(DoorNr)].tileY])
				CoreInit::managers.entityManager->Destroy(e);
			roomEntities[DoorArr[size_t(DoorNr)].tileX][DoorArr[size_t(DoorNr)].tileY].clear();

			Core::IMaterialManager::CreateInfo cubeInfo;

			int DoorCounter = 0;

			auto ent = CoreInit::managers.entityManager->Create();
			CoreInit::managers.transformManager->Create(ent);
			CoreInit::managers.transformManager->SetPosition(ent, DirectX::XMFLOAT3(DoorArr[size_t(DoorNr)].tileX + 0.5f, 0.0f, DoorArr[size_t(DoorNr)].tileY + 0.5f));

			CreationArguments args{ ent, DoorArr[size_t(DoorNr)].tileX, DoorArr[size_t(DoorNr)].tileY, DoorCounter, cubeInfo, wallTexture, floorTexture };
			CreateDoor(args);

			if (beingRendered)
			{
				for (auto& e : roomEntities[DoorArr[size_t(DoorNr)].tileX][DoorArr[size_t(DoorNr)].tileY])
					CoreInit::managers.eventManager->ToggleVisible(e, beingRendered);
			}
		}

		/*Utilz::GUID temp;

		if (DoorArr[int(DoorNr)].side == Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_SOUTH || DoorArr[int(DoorNr)].side == Room::DirectionToAdjacentRoom::DIRECTION_ADJACENT_ROOM_WEST)
			temp = "SimpleNormTransPS.hlsl";
		else
			temp = "SimpleNormPS.hlsl";

		DoorArr[int(DoorNr)].active = true;
		CoreInit::managers.renderableManager->CreateRenderableObject(roomEntities[DoorArr[int(DoorNr)].doorEntityPos], { "Door.mesh" });
		CoreInit::managers.materialManager->Create(roomEntities[DoorArr[int(DoorNr)].doorEntityPos], { temp, "Door.mat" });
		CoreInit::managers.transformManager->Move(roomEntities[DoorArr[int(DoorNr)].doorEntityPos], DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));*/

	}
}