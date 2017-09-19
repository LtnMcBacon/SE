#include "GameUnit.h"
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

	
	DamageEventVector.clear();
	for (auto &damageEvent : NextFrameDamage)
	{
		DamageEventVector.push_back(damageEvent);
	}
	NextFrameDamage.clear();
}
void GameUnit::ClearHealingEvents()
{
	
	HealingEventVector.clear();
	for (auto &healingEvent : NextFrameHealing)
	{
		HealingEventVector.push_back(healingEvent);
	}
	NextFrameHealing.clear();
}
void GameUnit::ClearConditionEvents()
{

	ConditionEventVector.clear();

	for (auto &conditionEvent : NextFrameCondition)
	{
		ConditionEventVector.push_back(conditionEvent);
	}
	NextFrameCondition.clear();
}


