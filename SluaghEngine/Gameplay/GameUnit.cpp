#include "GameUnit.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;

GameUnit::GameUnit()
{

}

GameUnit::~GameUnit()
{

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


