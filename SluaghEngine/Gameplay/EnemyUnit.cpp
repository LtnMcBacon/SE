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
				switch (ConditionEventVector[i].boon)
				{
				case Boons::CONDITIONAL_BOONS_NONE:
					break;
				case Boons::CONDITIONAL_BOONS_DAMAGE:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_DAMAGE;
					break;
				case Boons::CONDITIONAL_BOONS_KNOCKBACK:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_KNOCKBACK;
					break;
				case Boons::CONDITIONAL_BOONS_STUN:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_STUN;
					this->stunDuration += ConditionEventVector[i].duration;
					break;
				case Boons::CONDITIONAL_BOONS_ROOT:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_ROOT;
					break;
				case Boons::CONDITIONAL_BOONS_PROTECTION:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_PROTECTION;
					break;
				case Boons::CONDITIONAL_BOONS_PHYSICAL_RESISTANCE:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_PHYSICAL_RESISTANCE;
					break;
				case Boons::CONDITIONAL_BOONS_MAGICAL_RESISTANCE:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_MAGICAL_RESISTANCE;
					break;
				case Boons::CONDITIONAL_BOONS_FIRE_RESISTANCE:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_FIRE_RESISTANCE;
					break;
				case Boons::CONDITIONAL_BOONS_WATER_RESISTANCE:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_WATER_RESISTANCE;
					break;
				case Boons::CONDITIONAL_BOONS_NATURE_RESISTANCE:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_NATURE_RESISTANCE;
					break;
				case Boons::CONDITIONAL_BOONS_CASTSPEED:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_CASTSPEED;
					break;
				case Boons::CONDITIONAL_BOONS_SWIFTNESS:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_SWIFTNESS;
					break;
				case Boons::CONDITIONAL_BOONS_SLOW:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_SLOW;
					break;
				case Boons::CONDITIONAL_BOONS_INVULNERABILITY:
					myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_INVULNERABILITY;
					break;
				}
			}
			else
			{
				/*switch (ConditionEventVector[i].bane)
				{
				case Banes::CONDITIONAL_BANES_NONE:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_NONE;
					break;
				case Banes::CONDITIONAL_BANES_DAMAGE:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_DAMAGE;
					break;
				case Banes::CONDITIONAL_BANES_STUN:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_STUN;
					break;
				case Banes::CONDITIONAL_BANES_ROOT:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_ROOT;
					break;
				case Banes::CONDITIONAL_BANES_BLOODLETTING:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_BLOODLETTING;
					break;
				case Banes::CONDITIONAL_BANES_UNCOVER:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_UNCOVER;
					break;
				case Banes::CONDITIONAL_BANES_PHYSICAL_WEAKNESS:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_PHYSICAL_WEAKNESS;
					break;
				case Banes::CONDITIONAL_BANES_MAGICAL_WEAKNESS:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_MAGICAL_WEAKNESS;
					break;
				case Banes::CONDITIONAL_BANES_FIRE_WEAKNESS:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_FIRE_WEAKNESS;
					break;
				case Banes::CONDITIONAL_BANES_WATER_WEAKNESS:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_WATER_WEAKNESS;
					break;
				case Banes::CONDITIONAL_BANES_NATURE_WEAKNESS:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_NATURE_WEAKNESS;
					break;
				case Banes::CONDITIONAL_BANES_SLOW:
					myBlackboard->activeBane = Banes::CONDITIONAL_BANES_SLOW;
					break;
				}*/
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
			myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_NONE;
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

