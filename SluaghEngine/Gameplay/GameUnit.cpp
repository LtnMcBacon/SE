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
	for (size_t i = 0; i < NextFrameDamage.size; i++)
	{
		DamageEventVector.push_back(NextFrameDamage[i]);
	}
	NextFrameDamage.clear();
}
void GameUnit::ClearHealingEvents()
{
	
	HealingEventVector.clear();
	for (size_t i = 0; i < NextFrameHealing.size; i++)
	{
		HealingEventVector.push_back(NextFrameHealing[i]);
	}
	NextFrameHealing.clear();
}
void GameUnit::ClearConditionEvents()
{

	ConditionEventVector.clear();

	for (size_t i = 0; i < NextFrameCondition.size; i++)
	{
		ConditionEventVector.push_back(NextFrameCondition[i]);
	}
	NextFrameCondition.clear();
}


