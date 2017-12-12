#include "GameUnit.h"
#include <Profiler.h>
#include "CoreInit.h"
#include "PlayerUnit.h"

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

void SE::Gameplay::GameUnit::AddForce(float force[2])
{
	this->force[0] += force[0];
	this->force[1] += force[1];
}

void SE::Gameplay::GameUnit::AddForce(float x, float y)
{
	this->force[0] += x;
	this->force[1] += y;
}

void SE::Gameplay::GameUnit::DiminishForce(float dt)
{
	int sign;

	for (int i = 0; i < 2; i++)
	{
		if (force[i] < 0.0f)
			sign = -1;
		else
			sign = 1;

		if (force[i] * sign < sign * dt)
		{
			force[i] = 0;
		}
		else
		{
			force[i] -= sign * dt;
		}
	}
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

void SE::Gameplay::GameUnit::ClearNewStats()
{
	this->newStat = Stats();
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
		CoreInit::managers.animationManager->Start(unitEntity, &deathAnimation, 1, 2.5, Core::AnimationFlags::IMMEDIATE | Core::AnimationFlags::FORCED | Core::AnimationFlags::BLOCKBLENDING);
		SE::Utilz::GUID dSound;
		dSound = std::get<uint32_t>(CoreInit::managers.dataManager->GetValue(unitEntity, SE::Utilz::GUID("deathSoundGUID"), 0));
		CoreInit::managers.audioManager->PlaySound(unitEntity, dSound);
		ProfileReturnConst(true);
	}
	else if(CoreInit::managers.animationManager->IsAnimationPlaying(unitEntity, deathAnimation))
	{
		ProfileReturnConst(true);
	}
	ProfileReturnConst(false);
}

void SE::Gameplay::GameUnit::calcBaseStrChanges()
{
	StartProfile;
	if (baseStat.str > 5)
	{
		int increment = baseStat.str - 5;
		newStat.health = baseStat.health * (1.f + (0.05f * increment));
		newStat.damage = baseStat.damage * (1.f + (0.05f * increment));
	}
	else if (baseStat.str < 5)
	{
		newStat.health = baseStat.health * (1.f - (0.1f * baseStat.str));
		newStat.damage = baseStat.damage * (1.f - (0.1f * baseStat.str));

		if (baseStat.str <= 3)
		{
			newStat.armorCap = 2;
		}
		else if (baseStat.str == 1)
		{
			newStat.armorCap = 1;
		}
	}
	else
	{
		newStat.health = baseStat.health;
		newStat.damage = baseStat.damage;
	}
	StopProfile;
}
void SE::Gameplay::GameUnit::calcBaseAgiChanges()
{
	StartProfile;
	if (baseStat.agi > 5)
	{
		int increment = baseStat.agi - 5;
		newStat.rangedDamage = baseStat.rangedDamage * (1.f + (0.05f * increment));
		newStat.movementSpeed = baseStat.movementSpeed * (1.f + (0.05f * increment));
	}
	else if (baseStat.agi < 5)
	{
		newStat.rangedDamage = baseStat.rangedDamage * (1.f - (0.05f * baseStat.agi));
		newStat.movementSpeed = baseStat.movementSpeed * (1.f - (0.05f * baseStat.agi));
	}
	else
	{
		newStat.rangedDamage = baseStat.rangedDamage;
		newStat.movementSpeed = baseStat.movementSpeed;
	}
	StopProfile;
}
void SE::Gameplay::GameUnit::calcBaseWhiChanges()
{
	StartProfile;
	if (baseStat.whi > 5)
	{
		int increment = baseStat.whi - 5;
		newStat.magicDamage = baseStat.magicDamage * (1.f + (0.05f * increment));
		newStat.magicResistance = baseStat.magicResistance * (1.f + (0.025f * increment));
		newStat.natureResistance = baseStat.natureResistance * (1.f + (0.025f * increment));
		newStat.fireResistance = baseStat.fireResistance * (1.f + (0.025f * increment));
		newStat.waterResistance = baseStat.waterResistance * (1.f + (0.025f * increment));
	}
	else if (baseStat.whi < 5)
	{
		newStat.magicDamage = baseStat.magicDamage * (1.f - (0.05f * baseStat.whi));
		newStat.magicResistance = baseStat.magicResistance * (1.f - (0.05f * baseStat.whi));
		newStat.natureResistance = baseStat.natureResistance * (1.f - (0.05f * baseStat.whi));
		newStat.fireResistance = baseStat.fireResistance * (1.f - (0.05f * baseStat.whi));
		newStat.waterResistance = baseStat.waterResistance * (1.f - (0.05f * baseStat.whi));
	}
	else
	{
		newStat.magicDamage = baseStat.magicDamage;
		newStat.magicResistance = baseStat.magicResistance;
		newStat.natureResistance = baseStat.natureResistance;
		newStat.fireResistance = baseStat.fireResistance;
		newStat.waterResistance = baseStat.waterResistance;
	}
	StopProfile;
}

void SE::Gameplay::GameUnit::calcNewStrChanges()
{
	StartProfile;
	if (newStat.str > 5)
	{
		int increment = newStat.str - 5;
		newStat.health = baseStat.health * (1.f + (0.05f * increment));
		newStat.damage = baseStat.damage * (1.f + (0.05f * increment));
	}
	else if (newStat.str < 5)
	{
		newStat.health = baseStat.health * (1.f - (0.1f * newStat.str));
		newStat.damage = baseStat.damage * (1.f - (0.1f * newStat.str));

		if (newStat.str <= 3)
		{
			newStat.armorCap = 2;
		}
		else if (newStat.str == 1)
		{
			newStat.armorCap = 1;
		}
	}
	else
	{
		newStat.health = baseStat.health;
		newStat.damage = baseStat.damage;
	}
	StopProfile;
}

void SE::Gameplay::GameUnit::calcNewAgiChanges()
{
	StartProfile;
	if (newStat.agi > 5)
	{
		int increment = newStat.agi - 5;
		newStat.rangedDamage = baseStat.rangedDamage * (1.f + (0.05f * increment));
		newStat.movementSpeed = baseStat.movementSpeed * (1.f + (0.05f * increment));
	}
	else if (newStat.agi < 5)
	{
		newStat.rangedDamage = baseStat.rangedDamage * (1.f - (0.05f * newStat.agi));
		newStat.movementSpeed = baseStat.movementSpeed * (1.f - (0.05f * newStat.agi));
	}
	else
	{
		newStat.rangedDamage = baseStat.rangedDamage;
		newStat.movementSpeed = baseStat.movementSpeed;
	}
	StopProfile;
}

void SE::Gameplay::GameUnit::calcNewWhiChanges()
{
	StartProfile;
	if (newStat.whi > 5)
	{
		int increment = newStat.whi - 5;
		newStat.magicDamage = baseStat.magicDamage * (1.f + (0.05f * increment));
		newStat.magicResistance = baseStat.magicResistance * (1.f + (0.025f * increment));
		newStat.natureResistance = baseStat.natureResistance * (1.f + (0.025f * increment));
		newStat.fireResistance = baseStat.fireResistance * (1.f + (0.025f * increment));
		newStat.waterResistance = baseStat.waterResistance * (1.f + (0.025f * increment));
	}
	else if (newStat.whi < 5)
	{
		newStat.magicDamage = baseStat.magicDamage * (1.f - (0.05f * newStat.whi));
		newStat.magicResistance = baseStat.magicResistance * (1.f - (0.05f * newStat.whi));
		newStat.natureResistance = baseStat.natureResistance * (1.f - (0.05f * newStat.whi));
		newStat.fireResistance = baseStat.fireResistance * (1.f - (0.05f * newStat.whi));
		newStat.waterResistance = baseStat.waterResistance * (1.f - (0.05f * newStat.whi));
	}
	else
	{
		newStat.magicDamage = baseStat.magicDamage;
		newStat.magicResistance = baseStat.magicResistance;
		newStat.natureResistance = baseStat.natureResistance;
		newStat.fireResistance = baseStat.fireResistance;
		newStat.waterResistance = baseStat.waterResistance;
	}
	StopProfile;
}
