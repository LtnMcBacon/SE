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
	for (int i = 0; i < DamageEventVector.size(); i++)
	{
		this->health -= DamageEventVector[i].amount;
	}

	DamageEventVector.clear();

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
	extends = 0.25f; /*Should not be hardcoded! Obviously*/
	radius = sqrt(extends*extends + extends*extends);
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

