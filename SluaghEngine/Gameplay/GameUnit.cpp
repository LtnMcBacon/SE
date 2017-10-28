#include "GameUnit.h"
#include <Profiler.h>
#include "CoreInit.h"
using namespace SE;
using namespace Gameplay;

GameUnit::GameUnit()
{
}

GameUnit::GameUnit(float xPos, float yPos, float maxHealth) :
	xPos(xPos),
	yPos(yPos),
	health(maxHealth)
{
	this->unitEntity = CoreInit::managers.entityManager->Create();
	CoreInit::managers.transformManager->Create(this->unitEntity, DirectX::XMFLOAT3(xPos, 1.5f, yPos));
	mySelf = std::make_shared<GameUnit*>(this);
	zPos = 0.f;
}

GameUnit::~GameUnit()
{
	//Core::Engine::GetInstance().GetEntityManager().Destroy(unitEntity);

}

void GameUnit::DestroyEntity()
{
	CoreInit::managers.entityManager->DestroyNow(unitEntity);
}

void GameUnit::ClearDamageEvents()
{
	StartProfile;
	DamageEventVector.clear();
	for (auto &damageEvent : NextFrameDamage)
	{
		DamageEventVector.push_back(damageEvent);
	}
	NextFrameDamage.clear();
	StopProfile;
}
void GameUnit::ClearHealingEvents()
{
	StartProfile;
	HealingEventVector.clear();
	for (auto &healingEvent : NextFrameHealing)
	{
		HealingEventVector.push_back(healingEvent);
	}
	NextFrameHealing.clear();
	StopProfile;
}
void GameUnit::ClearConditionEvents()
{
	StartProfile;
	ConditionEventVector.clear();

	for (auto &conditionEvent : NextFrameCondition)
	{
		ConditionEventVector.push_back(conditionEvent);
	}
	NextFrameCondition.clear();
	StopProfile;
}

void GameUnit::MoveEntity(float xMovement, float yMovement)
{
	StartProfile;
	xPos += xMovement;
	yPos += yMovement;
	
	CoreInit::managers.transformManager->SetPosition(this->unitEntity, { xPos, zPos, yPos });
	StopProfile;
}

void GameUnit::PositionEntity(float xPos, float yPos)
{
	StartProfile;
	this->xPos = xPos;
	this->yPos = yPos;
	CoreInit::managers.transformManager->SetPosition(this->unitEntity, { xPos, 0.0f, yPos });
	StopProfile;
}


