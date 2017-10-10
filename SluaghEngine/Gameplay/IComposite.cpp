#include "IComposite.h"
#include <Profiler.h>
using namespace SE;
using namespace Gameplay;

IComposite::IComposite(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard) :
	IBehaviour(enemyBlackboard, gameBlackboard)
{

}

void IComposite::AddChild(IBehaviour* childToAdd)
{
	myChildren.push_back(childToAdd);
}

IComposite::~IComposite()
{
	for (auto child : myChildren)
		delete child;
}
