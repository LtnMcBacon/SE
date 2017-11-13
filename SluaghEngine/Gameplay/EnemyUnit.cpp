#include "EnemyUnit.h"
#include <Profiler.h>
#include "Flowfield.h"
#include "CoreInit.h"
#include <Gameplay/EnemyBlackboard.h>
#include <Gameplay/BehaviouralTree.h>

void SE::Gameplay::EnemyUnit::ResolveEvents()
{
	StartProfile;
	
	// only basic at the moment
	if (!myBlackboard->invurnerable)
	{
		for (int i = 0; i < DamageEventVector.size(); i++)
		{
			this->health -= DamageEventVector[i].amount;
		}

		for (int i = 0; i < ConditionEventVector.size(); i++)
		{
			if (ConditionEventVector[i].isBane == false)
			{
				if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_DAMAGE)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_DAMAGE;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_KNOCKBACK)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_KNOCKBACK;

				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_STUN)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_STUN;
					this->stunDuration += ConditionEventVector[i].duration;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_ROOT)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_ROOT;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_PROTECTION)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_PROTECTION;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_PHYSICAL_RESISTANCE)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_PHYSICAL_RESISTANCE;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_MAGICAL_RESISTANCE)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_MAGICAL_RESISTANCE;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_FIRE_RESISTANCE)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_FIRE_RESISTANCE;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_WATER_RESISTANCE)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_WATER_RESISTANCE;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_NATURE_RESISTANCE)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_NATURE_RESISTANCE;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_CASTSPEED)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_CASTSPEED;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_SWIFTNESS)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_SWIFTNESS;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_SLOW)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_SLOW;
				}
				else if (ConditionEventVector[i].boon == Boons::CONDITIONAL_BOONS_INVULNERABILITY)
				{
					myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_INVULNERABILITY;
				}
			}
			else
			{
				if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_DAMAGE)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_DAMAGE;

				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_STUN)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_STUN;
					this->stunDuration += ConditionEventVector[i].duration;
				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_ROOT)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_ROOT;
				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_BLOODLETTING)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_BLOODLETTING;
				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_UNCOVER)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_UNCOVER;
				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_PHYSICAL_WEAKNESS)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_PHYSICAL_WEAKNESS;
				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_MAGICAL_WEAKNESS)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_MAGICAL_WEAKNESS;
				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_FIRE_WEAKNESS)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_FIRE_WEAKNESS;
				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_WATER_WEAKNESS)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_WATER_WEAKNESS;
				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_NATURE_WEAKNESS)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_NATURE_WEAKNESS;
				}
				else if (ConditionEventVector[i].bane == Banes::CONDITIONAL_BANES_SLOW)
				{
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_SLOW;
				}
			}
		}
	}
	DamageEventVector.clear();
	ConditionEventVector.clear();



	ProfileReturnVoid;

}

void SE::Gameplay::EnemyUnit::DecideAction()
{
	StartProfile;
	/*
	* Code body
	*/
	entityAction = EnemyActions::ENEMY_ACTION_MOVE;
	
	if (myBehaviouralTree)
	{
		myBlackboard->extents = 0.25;
		myBlackboard->ownerPointer = this;
		myBlackboard->checkedThisFrame = false;
		myBehaviouralTree->Tick();
	}

	ProfileReturnVoid;
}

void SE::Gameplay::EnemyUnit::PerformAction(float dt)
{
	StartProfile;
	/*
	* Code body
	*/
	ProfileReturnVoid;
}

void SE::Gameplay::EnemyUnit::Update(float dt)
{
	StartProfile;
	/*
	* Code body
	*/
	ResolveEvents();
	DecideAction();
	if (stunDuration > 0.f)
	{
		stunDuration -= dt;
		if(stunDuration <= 0.f)
		{
			myBlackboard->activeBoon = Boons::CONDITIONAL_BOONS_NONE;
			stunDuration = 0.f;
		}
	}
	PerformAction(dt);
	ProfileReturnVoid;
}

void SE::Gameplay::EnemyUnit::AddForce(float force[2])
{
	forcesToApply[0] += force[0];
	forcesToApply[1] += force[1];
}

SE::Gameplay::EnemyUnit::EnemyUnit(const FlowField* roomFlowField, float xPos, float yPos, float maxHealth) :
	GameUnit(xPos, yPos, maxHealth),
	flowFieldForRoom(roomFlowField),
	extraSampleCoords{xPos, yPos},
	previousMovement{0,0},
	sample(0)
{
	extents = 0.25f; /*Should not be hardcoded! Obviously*/
	radius = sqrt(extents*extents + extents*extents);
}

SE::Gameplay::EnemyUnit::~EnemyUnit()
{
	CoreInit::managers.entityManager->Destroy(this->unitEntity);
	/*
	* Code body
	*/
	delete myBehaviouralTree;
	delete myBlackboard;
}

