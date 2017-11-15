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
	CoreInit::managers.transformManager->Create(this->unitEntity, DirectX::XMFLOAT3(xPos, 0.0f, yPos));
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
	
	CoreInit::managers.transformManager->Move(this->unitEntity, DirectX::XMFLOAT3{ xMovement, 0.f, yMovement });
	DirectX::XMFLOAT3 pos = CoreInit::managers.transformManager->GetPosition(this->unitEntity);
	xPos = pos.x;
	yPos = pos.z;
	StopProfile;
}

void GameUnit::PositionEntity(float xPos, float yPos)
{
	StartProfile;
	this->xPos = xPos;
	this->yPos = yPos;
	CoreInit::managers.transformManager->SetPosition(this->unitEntity, { xPos, zPos, yPos });
	DirectX::XMFLOAT3 pos = CoreInit::managers.transformManager->GetPosition(this->unitEntity);
	StopProfile;
}

bool SE::Gameplay::GameUnit::IsAlive()
{
	StartProfile;
	if(health > 0.f)
		ProfileReturnConst(true);
	if (!deathAnimationRunning)
	{
		deathAnimationRunning = true;
		CoreInit::managers.animationManager->Start(unitEntity, &deathAnimation, 1, 5, Core::AnimationFlags::IMMEDIATE | Core::AnimationFlags::FORCED | Core::AnimationFlags::BLOCKBLENDING);
		ProfileReturnConst(true);
	}
	else if(CoreInit::managers.animationManager->IsAnimationPlaying(unitEntity, deathAnimation))
	{
		ProfileReturnConst(true);
	}
	ProfileReturnConst(false);
}


