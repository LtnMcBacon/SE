#include "BehavioursTest.h"
#include <Profiler.h>
using namespace SE;
using namespace Test;


BehavioursTest::BehavioursTest()
{
}

BehavioursTest::~BehavioursTest()
{
}

class TestLeaf : public Gameplay::IBehaviour
{
public:
	TestLeaf(Gameplay::EnemyBlackboard* enemyBlackboard, Gameplay::GameBlackboard* gameBlackboard,
	         Gameplay::Status myStatus, int ticksBeforeSuccess) :
		IBehaviour(enemyBlackboard, gameBlackboard),
		ticksBeforeSuccess(ticksBeforeSuccess)
	{
		this->myStatus = myStatus;
	};


protected:
	inline Gameplay::Status Update() override
	{
		if (ticksBeforeSuccess == 0)
		{
			myStatus = Gameplay::Status::BEHAVIOUR_SUCCESS;
		}
		else
			ticksBeforeSuccess--;
		return myStatus;
	}

public:
	IBehaviour* CopyBehaviour(Gameplay::GameBlackboard* gameBlackboard, Gameplay::EnemyBlackboard* enemyBlackboard) const
	override
	{
		return nullptr;
	};
private:
	int ticksBeforeSuccess;
};

std::string BehaviourToString(SE::Gameplay::Status status)
{
	StartProfile;
	switch(status)
	{
	case SE::Gameplay::Status::BEHAVIOUR_SUCCESS:
		ProfileReturnConst("Success");
	case Gameplay::Status::BEHAVIOUR_FAILURE:
		ProfileReturnConst("Failure");
	case Gameplay::Status::BEHAVIOUR_RUNNING: 
		ProfileReturnConst("Running");
	case Gameplay::Status::BEHAVIOUR_SUSPENDED: 
		ProfileReturnConst("Suspended");
	case Gameplay::Status::BEHAVIOUR_INVALID: 
		ProfileReturnConst("Invalid");
	default: 
		break;
	}
	ProfileReturnConst("Something went wrong");
}

bool RunTest(const std::string& test, std::vector<SE::Gameplay::Status> &expectedValues, Gameplay::IBehaviour* toTest, SE::DevConsole::IConsole* console)
{
	for (auto expected : expectedValues)
	{
		auto result = toTest->Tick();

		if (result != expected)
		{
			auto print = "BehaviourTest failed on the ''" + test + "'' test.\n";
			console->Print(print.c_str());
			print = "Expected value: " + BehaviourToString(expected) +
				" Returned value: " + BehaviourToString(result) + "\n";
			console->Print(print.c_str());
			return false;
		}
	}
	return true;
}

bool BehavioursTest::Run(SE::DevConsole::IConsole* console)
{
	StartProfile;
	bool passed = true;
	std::vector<SE::Gameplay::Status> expectedValues;
	/*Test Selector*/
	SE::Gameplay::Selector Selector(nullptr, nullptr);
	Selector.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_FAILURE, 10));
	Selector.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_FAILURE, 8));
	Selector.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_FAILURE, 5));
	Selector.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_RUNNING, 3));
	Selector.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_FAILURE, 7));

	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_RUNNING);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_RUNNING);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_RUNNING);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUCCESS);

	passed &= RunTest("Selector", expectedValues, &Selector, console);
	
	/*Test Sequence*/
	expectedValues.clear();

	SE::Gameplay::Sequence Sequence(nullptr, nullptr);
	Sequence.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_FAILURE, 3));
	Sequence.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_RUNNING, 2));
	Sequence.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_SUSPENDED, 4));
	Sequence.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_RUNNING, 2));
	Sequence.AddChild(new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_SUCCESS, 0));

	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_FAILURE);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_FAILURE);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_FAILURE);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_RUNNING);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_RUNNING);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUSPENDED);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUSPENDED);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUSPENDED);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUSPENDED);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_RUNNING);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_RUNNING);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUCCESS);

	passed &= RunTest("Sequence", expectedValues, &Sequence, console);

	/*Test Inverter*/
	expectedValues.clear();
	SE::Gameplay::Inverter Inverter(nullptr, nullptr, new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_FAILURE, 3));

	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUCCESS);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUCCESS);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUCCESS);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_FAILURE);

	passed &= RunTest("Inverter", expectedValues, &Inverter, console);

	/*Test Repeater*/
	expectedValues.clear();
	SE::Gameplay::Repeater Repeater(
		nullptr,
		nullptr,
		new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_FAILURE, 3),
		20
	);

	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_FAILURE);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_FAILURE);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_FAILURE);
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUCCESS);

	passed &= RunTest("Repeater", expectedValues, &Repeater, console);

	/*Test RepeatUntilFail*/
	expectedValues.clear();
	expectedValues.push_back(SE::Gameplay::Status::BEHAVIOUR_SUCCESS);

	SE::Gameplay::RepeatUntilFail repeatUntilFail(nullptr, nullptr, 
		new Gameplay::Inverter(nullptr, nullptr, 
			new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_FAILURE, 5)
		)
	);

	passed &= RunTest("RepeatUntilFail", expectedValues, &repeatUntilFail, console);

	/*Test Succeeder*/
	SE::Gameplay::Succeeder Succeeder(nullptr, nullptr,
		new TestLeaf(nullptr, nullptr, SE::Gameplay::Status::BEHAVIOUR_FAILURE, 2));

	passed &= RunTest("Succeeder", expectedValues, &Succeeder, console);

	ProfileReturn(passed);
}
