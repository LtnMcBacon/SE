#include "BehavioursTest.h"
using namespace SE;
using namespace Test;


BehavioursTest::BehavioursTest()
{

}

BehavioursTest::~BehavioursTest()
{

}

class TestLeaf : Gameplay::IBehaviour
{
public:
	TestLeaf(Gameplay::EnemyBlackboard* enemyBlackboard, Gameplay::GameBlackboard* gameBlackboard, Gameplay::Status myStatus) :
		IBehaviour(enemyBlackboard, gameBlackboard)
	{
		this->myStatus = myStatus;
	};
protected:
	inline Gameplay::Status Update() override
	{
		return myStatus;
	}
private:

};

bool BehavioursTest::Run(SE::Utilz::IConsoleBackend* console)
{
	/*Test Selector*/

	/*Test Sequence*/

	/*Test Inverter*/

	/*Test Repeater*/

	/*Test RepatUntilFail*/

	/*Test Succeeder*/
}