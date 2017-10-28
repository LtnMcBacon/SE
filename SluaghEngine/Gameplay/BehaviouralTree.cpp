#include "BehaviouralTree.h"
#include "IBehaviour.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;

BehaviouralTree* BehaviouralTree::CopyTree(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const
{
	StartProfile;
	IBehaviour* root = rootBehaviour->CopyBehaviour(gameBlackboard, enemyBlackboard);

	ProfileReturn(new BehaviouralTree(root));
	
}

BehaviouralTree::BehaviouralTree(IBehaviour* root) :
	rootBehaviour(root)
{

}

BehaviouralTree::~BehaviouralTree()
{
	delete rootBehaviour;
}

void BehaviouralTree::Tick()
{
	rootBehaviour->Tick();
}
