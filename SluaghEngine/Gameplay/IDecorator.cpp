#include "IDecorator.h"
using namespace SE;
using namespace Gameplay;

IDecorator::IDecorator(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, IBehaviour* child) :
	IBehaviour(enemyBlackboard, gameBlackboard),
	myChild(child)
{

}

IDecorator::~IDecorator()
{
	delete myChild;
}
