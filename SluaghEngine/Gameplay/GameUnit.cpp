#include "GameUnit.h"
#include <Profiler.h>
#include <Core/EntityManager.h>
#include <Core/TransformManager.h>
#include <Core/Engine.h>
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
	this->unitEntity = Core::Engine::GetInstance().GetEntityManager().Create();
	Core::Engine::GetInstance().GetTransformManager().Create(this->unitEntity, DirectX::XMFLOAT3(xPos, 0.f, yPos));
	Core::Engine::GetInstance().GetTransformManager().SetRotation(this->unitEntity, 0.f, 0.f, 0.f);
	mySelf = std::make_shared<GameUnit*>(this);
}

GameUnit::~GameUnit()
{
	//Core::Engine::GetInstance().GetEntityManager().Destroy(unitEntity);

}

void GameUnit::DestroyEntity()
{
	SE::Core::Engine::GetInstance().GetEntityManager().Destroy(unitEntity);
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
	Core::Engine::GetInstance().GetTransformManager().SetPosition(this->unitEntity, { xPos, 0.0f, yPos });
	auto temp = Core::Engine::GetInstance().GetTransformManager().GetPosition(this->unitEntity);
	StopProfile;
}

void GameUnit::PositionEntity(float xPos, float yPos)
{
	StartProfile;
	this->xPos = xPos;
	this->yPos = yPos;
	Core::Engine::GetInstance().GetTransformManager().SetPosition(this->unitEntity, { xPos, 0.0f, yPos });
	StopProfile;
}


