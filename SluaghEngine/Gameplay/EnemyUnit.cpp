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
			CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, Utilz::GUID("DefaultAttackSound.wav"));
		}

		for (int i = 0; i < ConditionEventVector.size(); i++)
		{
			if(ConditionEventVector[i].type == ConditionEvent::ConditionTypes::CONDITION_TYPE_STUN)
			{
				myBlackboard->activeCondition = ConditionEvent::ConditionTypes::CONDITION_TYPE_STUN;
				this->stunDuration += ConditionEventVector[i].duration;
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
			myBlackboard->activeCondition = ConditionEvent::ConditionTypes::CONDITION_TYPE_NONE;
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

