#include "BehaviouralTreeFactory.h"
#include "CoreInit.h"
#include <functional>
#include <Profiler.h>
#include <sstream>
#include <iterator>
#include "Behaviours.h"
#include "BehaviouralTree.h"

#undef min
#undef max

namespace
{
	/*Found at: https://stackoverflow.com/questions/236129/most-elegant-way-to-split-a-string */
	template <typename Out>
	void split(const std::string& s, char delim, Out result)
	{
		StartProfile;
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim))
		{
			*(result++) = item;
		}
		StopProfile;
	}

	std::vector<std::string> split(const std::string& s, char delim)
	{
		StartProfile;
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		ProfileReturn(elems);
	}
}

using namespace SE;
using namespace Gameplay;

IBehaviour* BehaviouralTreeFactory::CreateFromType(NodeData* dataArray, int nodeID)
{
	StartProfile;
	IBehaviour* finishedBehaviour = nullptr;
	/*Check for leaves*/
	if (dataArray[nodeID].Type == "Leaf")
	{
		finishedBehaviour = CreateLeaf(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "FlowFieldMovementLeaf")
	{
		finishedBehaviour = CreateFlowFieldMovementLeaf(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "MoveTowardsPlayerLeaf")
	{
		finishedBehaviour = CreateMoveTowardsPlayerLeaf(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "TimerCondition")
	{
		finishedBehaviour = CreateTimerConditionLeaf(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "RangeToPlayerCondition")
	{
		finishedBehaviour = CreateRangeToPlayerConditionLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "ObstacleOnPositionCondition")
	{
		finishedBehaviour = CreateObstacleOnPositionConditionLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "AttackCooldownZeroCondition")
	{
		finishedBehaviour = CreateAttackCooldownZeroConditionLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "ChannelingCondition")
	{
		finishedBehaviour = CreateChannelingConditionLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "AnimationRunningCondition")
	{
		finishedBehaviour = CreateAnimationRunningLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "StunnedCondition")
	{
		finishedBehaviour = CreateStunnedConditionLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "InTheSameRoomAsThePlayerCondition")
	{
		finishedBehaviour = CreateInTheSameRoomAsThePlayerConditionLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "CurrentAnimationAllowsBlendingCondition")
	{
		finishedBehaviour = CreateCurrentAnimationAllowsBlendingConditionLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "MakeInvulnerableLeaf")
	{
		finishedBehaviour = CreateMakeInvulnerableLeaf(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "MakeVulnerableLeaf")
	{
		finishedBehaviour = CreateMakeVulnerableLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "GlaistigAttackLeaf")
	{
		finishedBehaviour = CreateGlaistigAttackLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "LeapAtPlayerLeaf")
	{
		finishedBehaviour = CreateLeapAtPlayerLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "LineOfSightCondition")
	{
		finishedBehaviour = CreateLineOfSightConditionLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "DistanceToDoorCondition")
	{
		finishedBehaviour = CreateDistanceToDoorConditionLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "ResetAttackCooldownLeaf")
	{
		finishedBehaviour = CreateResetAttackCooldownLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "AddTimeToAttackCooldownLeaf")
	{
		finishedBehaviour = CreateAddTimeToAttackCooldownLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "TickDownAttackCooldownLeaf")
	{
		finishedBehaviour = CreateTickDownAttackCooldownLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "AnimationLeaf")
	{
		finishedBehaviour = CreateAnimationLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "FaceThePlayerLeaf")
	{
		finishedBehaviour = CreateFaceThePlayerLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "WhileChannelingLeaf")
	{
		finishedBehaviour = CreateWhileChannelingLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "BodachMeleeAttackLeaf")
	{
		finishedBehaviour = CreateBodachMeleeAttackLeaf(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "PechMeleeAttackLeaf")
	{
		finishedBehaviour = CreatePechMeleeAttackLeaf(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "PechRangedAttackLeaf")
	{
		finishedBehaviour = CreatePechRangedAttackLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "StopChannelingLeaf")
	{
		finishedBehaviour = CreateStopChannelingLeaf(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "StopAnimationsLeaf")
	{
		finishedBehaviour = CreateStopAnimationsLeaf(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "PauseAnimationsLeaf")
	{
		finishedBehaviour = CreatePauseAnimationsLeaf(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "ChangeRoomLeaf")
	{
		finishedBehaviour = CreateChangeRoomLeaf(dataArray, nodeID);
	}
		/*Check for Composites*/
	else if (dataArray[nodeID].Type == "Sequence")
	{
		finishedBehaviour = CreateSequence(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "Selector")
	{
		finishedBehaviour = CreateSelector(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "RandomSequence")
	{
		finishedBehaviour = CreateRandomSequence(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "RandomSelector")
	{
		finishedBehaviour = CreateRandomSelector(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "RestartingSequence")
	{
		finishedBehaviour = CreateRestartingSequence(dataArray, nodeID);
	}
		/*Check for Decorators*/
	else if (dataArray[nodeID].Type == "Inverter")
	{
		finishedBehaviour = CreateInverter(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "Repeater")
	{
		finishedBehaviour = CreateRepeater(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "RepeatUntilFail")
	{
		finishedBehaviour = CreateRepeatUntilFail(dataArray, nodeID);
	}
	else if (dataArray[nodeID].Type == "Succeeder")
	{
		finishedBehaviour = CreateSucceeder(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "FailUntilSuccess")
	{
		finishedBehaviour = CreateFailUntilSuccess(dataArray, nodeID);
	}

	ProfileReturn(finishedBehaviour);
}

IBehaviour* BehaviouralTreeFactory::CreateLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;

	ProfileReturn(new SucceederLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateFlowFieldMovementLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;

	ProfileReturn(new FlowFieldMovementLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateMoveTowardsPlayerLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;

	ProfileReturn(new MoveTowardsPlayerLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateGlaistigAttackLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	
	ProfileReturn(new GlaistigAttackLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateResetAttackCooldownLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new ResetAttackCooldownLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateAddTimeToAttackCooldownLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	float time = std::stof(dataArray[nodeID].nodeData[0]);
	ProfileReturn(new AddTimeToAttackCooldownLeaf(nullptr, nullptr, time));
}

IBehaviour* BehaviouralTreeFactory::CreateTickDownAttackCooldownLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new TickDownAttackCooldownLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateTimerConditionLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	float timer = std::stof(dataArray[nodeID].nodeData[0]);
	ProfileReturn(new TimerCondition(nullptr, nullptr, timer));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateLineOfSightConditionLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;

	ProfileReturnConst(new LineOfSightCondition(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateRangeToPlayerConditionLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	float min = std::stof(dataArray[nodeID].nodeData[0]);
	float max = std::stof(dataArray[nodeID].nodeData[1]);
	ProfileReturn(new RangeToPlayerCondition(nullptr, nullptr, min, max));
}

IBehaviour* BehaviouralTreeFactory::CreateAttackCooldownZeroConditionLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new AttackCooldownZeroCondition(nullptr, nullptr));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateStunnedConditionLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn( new StunnedCondition(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateObstacleOnPositionConditionLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn( new ObstacleOnPositionCondition(nullptr, nullptr));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateChannelingConditionLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn( new ChannelingCondition(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateInTheSameRoomAsThePlayerConditionLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new InTheSameRoomAsThePlayerCondition(nullptr, nullptr));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateCurrentAnimationAllowsBlendingConditionLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new CurrentAnimationAllowsBlendingCondition(nullptr, nullptr));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateDistanceToDoorConditionLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new DistanceToDoorCondition(nullptr, nullptr, std::stof(dataArray[nodeID].nodeData[0])));
}

IBehaviour* BehaviouralTreeFactory::CreateAnimationRunningLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	int counter = 0;
	int nrOfGUIDs = stoi(dataArray[nodeID].nodeData[counter++]);
	std::vector<Utilz::GUID> guids;
	for(int i = 0; i < nrOfGUIDs; i++)
	{
		guids.push_back(dataArray[nodeID].nodeData[counter++]);
	}
	ProfileReturn(new AnimationRunningCondition(nullptr, nullptr, guids));
}

IBehaviour* BehaviouralTreeFactory::CreateMakeInvulnerableLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn( new MakeInvulnerableLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateMakeVulnerableLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn( new MakeVulnerableLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateFaceThePlayerLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn( new FaceThePlayerLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateLeapAtPlayerLeaf(NodeData* dataArray, int nodeID)
{

	StartProfile;
	ProfileReturn(new LeapAtPlayerLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateBodachMeleeAttackLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new BodachMeleeAttackLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreatePechMeleeAttackLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new PechMeleeAttackLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreatePechRangedAttackLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new PechRangedAttackLeaf(nullptr, nullptr));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateAnimationLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;

	std::vector<Utilz::GUID> animations;
	Core::AnimationFlags definedFlags = Core::AnimationFlags::CURRENT;

	int counter = 0;
	int numberOfLayers = stoi(dataArray[nodeID].nodeData[counter++]);

	for(int i = 0; i < numberOfLayers; i++)
	{
		animations.push_back(dataArray[nodeID].nodeData[counter++]);
	
	}
	float duration = std::stof(dataArray[nodeID].nodeData[counter++]);
	int numberOfAnimationFlags = std::stoi(dataArray[nodeID].nodeData[counter++]);
	for (int i = 0; i < numberOfAnimationFlags; i++)
	{
		std::string flag = dataArray[nodeID].nodeData[counter++];
		std::transform(flag.begin(), flag.end(), flag.begin(), toupper);
		if(flag == "BLENDTO")
		{
			definedFlags |= Core::AnimationFlags::BLENDTO;
		}
		else if(flag == "BLENDTOANDBACK")
		{
			definedFlags |= Core::AnimationFlags::BLENDTOANDBACK;
		}
		else if(flag == "IMMEDIATE")
		{
			definedFlags |= Core::AnimationFlags::IMMEDIATE;
		}
		else if (flag == "LOOP")
		{
			definedFlags |= Core::AnimationFlags::LOOP;
		}
		else if (flag == "CURRENT")
		{
			definedFlags |= Core::AnimationFlags::CURRENT;
		}
		else if (flag == "FORCED")
		{
			definedFlags |= Core::AnimationFlags::FORCED;
		}
		else if (flag == "BLOCKBLENDING")
		{
			definedFlags |= Core::AnimationFlags::BLOCKBLENDING;
		}
		else if (flag == "FORCEBLENDING")
		{
			definedFlags |= Core::AnimationFlags::FORCEBLENDING;
		}
	}
	
	ProfileReturn(new AnimationLeaf(nullptr, nullptr, animations, duration, definedFlags));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateStopChannelingLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new StopChannelingLeaf(nullptr, nullptr));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateStopAnimationsLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new StopAnimationsLeaf(nullptr, nullptr));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreatePauseAnimationsLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new PauseAnimationsLeaf(nullptr, nullptr));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateWhileChannelingLeaf(NodeData * dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn( new WhileChannelingLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateChangeRoomLeaf(NodeData* dataArray, int nodeID)
{
	StartProfile;
	ProfileReturn(new ChangeRoomLeaf(nullptr, nullptr));
}

IBehaviour* BehaviouralTreeFactory::CreateSequence(NodeData* dataArray, int nodeID)
{
	StartProfile;
	Sequence* returnBehaviour = new Sequence(nullptr, nullptr);
	for (auto& childID : dataArray[nodeID].childID)
	{
		returnBehaviour->AddChild(CreateFromType(dataArray, childID));
	}

	ProfileReturn(returnBehaviour);
}


IBehaviour* BehaviouralTreeFactory::CreateSelector(NodeData* dataArray, int nodeID)
{
	StartProfile;
	Selector* returnBehaviour = new Selector(nullptr, nullptr);
	for (auto& childID : dataArray[nodeID].childID)
	{
		returnBehaviour->AddChild(CreateFromType(dataArray, childID));
	}

	ProfileReturn(returnBehaviour);
}

IBehaviour* BehaviouralTreeFactory::CreateRandomSequence(NodeData* dataArray, int nodeID)
{
	StartProfile;
	RandomSequence* returnBehaviour = new RandomSequence(nullptr, nullptr);
	for (auto& childID : dataArray[nodeID].childID)
	{
		returnBehaviour->AddChild(CreateFromType(dataArray, childID));
	}

	ProfileReturn(returnBehaviour);
}

IBehaviour* BehaviouralTreeFactory::CreateRandomSelector(NodeData* dataArray, int nodeID)
{
	StartProfile;
	RandomSelector* returnBehaviour = new RandomSelector(nullptr, nullptr);
	for (auto& childID : dataArray[nodeID].childID)
	{
		returnBehaviour->AddChild(CreateFromType(dataArray, childID));
	}

	ProfileReturn(returnBehaviour);
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateRestartingSequence(NodeData * dataArray, int nodeID)
{
	StartProfile;
	RestartingSequence* returnBehaviour = new RestartingSequence(nullptr, nullptr);
	for (auto& childID : dataArray[nodeID].childID)
	{
		returnBehaviour->AddChild(CreateFromType(dataArray, childID));
	}

	ProfileReturn(returnBehaviour);
}

IBehaviour* BehaviouralTreeFactory::CreateInverter(NodeData* dataArray, int nodeID)
{
	StartProfile;
	IBehaviour* childNode = CreateFromType(dataArray, dataArray[nodeID].childID[0]);

	ProfileReturn(new Inverter(nullptr, nullptr, childNode));
}

IBehaviour* BehaviouralTreeFactory::CreateRepeater(NodeData* dataArray, int nodeID)
{
	StartProfile;
	IBehaviour* childNode = CreateFromType(dataArray, dataArray[nodeID].childID[0]);
	int repeatCount = std::stoi(dataArray[nodeID].nodeData[0]);

	ProfileReturn(new Repeater(nullptr, nullptr, childNode, uint_fast8_t(repeatCount)));
}

IBehaviour* BehaviouralTreeFactory::CreateRepeatUntilFail(NodeData* dataArray, int nodeID)
{
	StartProfile;
	IBehaviour* childNode = CreateFromType(dataArray, dataArray[nodeID].childID[0]);

	ProfileReturn(new RepeatUntilFail(nullptr, nullptr, childNode));
}

IBehaviour* BehaviouralTreeFactory::CreateSucceeder(NodeData* dataArray, int nodeID)
{
	StartProfile;
	IBehaviour* childNode = CreateFromType(dataArray, dataArray[nodeID].childID[0]);

	ProfileReturn(new Succeeder(nullptr, nullptr, childNode));
}

IBehaviour * SE::Gameplay::BehaviouralTreeFactory::CreateFailUntilSuccess(NodeData * dataArray, int nodeID)
{
	StartProfile;
	IBehaviour* childNode = CreateFromType(dataArray, dataArray[nodeID].childID[0]);

	ProfileReturn(new FailUntilSuccess(nullptr, nullptr, childNode));
}

bool BehaviouralTreeFactory::CreateTreeFromNodeData(const Utilz::GUID& GUID, NodeData* dataArray, size_t size)
{
	StartProfile;
	IBehaviour* root = CreateSelector(dataArray, 0);
	if (root)
	{
		BehaviouralTree* behaviourTree = new BehaviouralTree(root);
		behaviouralTrees[GUID] = behaviourTree;
		ProfileReturnConst(true);
	}

	ProfileReturnConst(false);
}

bool BehaviouralTreeFactory::LoadTree(const Utilz::GUID& guid)
{
	StartProfile;

	auto& loadedTree = behaviouralTrees.find(guid);
	if (loadedTree != behaviouralTrees.end())
	{
		ProfileReturnConst(true);
	}

	const auto done = CoreInit::subSystems.resourceHandler->LoadResource(guid,
		[this](auto guid, auto data, auto size) {
		std::string stringData((char*)data, size);
		stringData.erase(std::remove(stringData.begin(), stringData.end(), '\r'), stringData.end());

		auto lineSplit = split(stringData, '\n');
		auto lineSplitIt = lineSplit.begin();
		int numberOfNodes = std::stoi(*lineSplitIt);
		++lineSplitIt;
		NodeData* dataArray = new NodeData[numberOfNodes];
		do
		{
			int id = std::stoi(*lineSplitIt);
			++lineSplitIt;
			dataArray[id].Type = *lineSplitIt;
			++lineSplitIt;
			dataArray[id].nodeData = split(*lineSplitIt, ',');

			++lineSplitIt;
			auto childs = split(*lineSplitIt, ',');
			for (auto const& info : childs)
			{
				dataArray[id].childID.push_back(std::stoi(info));
			}

			++lineSplitIt;
		} while (lineSplitIt != lineSplit.end());

		/*Build the tree from the dataArray*/
		if (!CreateTreeFromNodeData(guid, dataArray, numberOfNodes))
		{
			delete[] dataArray;
			return ResourceHandler::InvokeReturn::FAIL;
		}
		/*ProfileReturn 0 for success, -1 for fail, 1 for refcount*/
		delete[] dataArray;
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});

	if (done != -1)
	ProfileReturnConst(true);
	ProfileReturnConst(false);
}

BehaviouralTree* BehaviouralTreeFactory::BuildBehaviouralTree(const Utilz::GUID& GUID, GameBlackboard* gameBlackboard,
															  EnemyBlackboard* enemyBlackboard)
{
	StartProfile;

	_ASSERT(gameBlackboard);
	_ASSERT(enemyBlackboard);

	const auto loadedTree = behaviouralTrees.find(GUID);
	if (loadedTree == behaviouralTrees.end())
	{
		ProfileReturnConst(nullptr);
	}


	BehaviouralTree* toPass = loadedTree->second->CopyTree(gameBlackboard, enemyBlackboard);

	ProfileReturn(toPass);
}

BehaviouralTreeFactory::BehaviouralTreeFactory()
{
}

BehaviouralTreeFactory::~BehaviouralTreeFactory()
{
	for (auto it : behaviouralTrees)
		delete it.second;
}
