#include "BehaviouralTreeFactory.h"
#include "Core/Engine.h"
#include <functional>
#include <Profiler.h>
#include <sstream>
#include <iterator>
#include "Behaviours.h"
#include "BehaviouralTree.h"

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
	else if(dataArray[nodeID].Type == "FlowFieldMovementLeaf")
	{
		finishedBehaviour = CreateFlowFieldMovementLeaf(dataArray, nodeID);
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
		/*Check for Decorators*/
	else if (dataArray[nodeID].Type == "Inverter")
	{
		finishedBehaviour = CreateInverter(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "Repeater")
	{
		finishedBehaviour = CreateRepeater(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "RepeatUntilFail")
	{
		finishedBehaviour = CreateRepeatUntilFail(dataArray, nodeID);
	}
	else if(dataArray[nodeID].Type == "Succeeder")
	{
		finishedBehaviour = CreateSucceeder(dataArray, nodeID);
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

IBehaviour* BehaviouralTreeFactory::CreateSequence(NodeData* dataArray, int nodeID)
{
	StartProfile;
	Sequence* returnBehaviour = new Sequence(nullptr, nullptr);
	for(auto &childID : dataArray[nodeID].childID)
	{
		returnBehaviour->AddChild(CreateFromType(dataArray, childID));
	}

	ProfileReturn(returnBehaviour);
}


IBehaviour* BehaviouralTreeFactory::CreateSelector(NodeData* dataArray, int nodeID)
{
	StartProfile;
	Selector* returnBehaviour = new Selector(nullptr, nullptr);
	for (auto &childID : dataArray[nodeID].childID)
	{
		returnBehaviour->AddChild(CreateFromType(dataArray, childID));
	}

	ProfileReturn(returnBehaviour);
}

IBehaviour* BehaviouralTreeFactory::CreateRandomSequence(NodeData* dataArray, int nodeID)
{
	StartProfile;
	RandomSequence* returnBehaviour = new RandomSequence(nullptr, nullptr);
	for (auto &childID : dataArray[nodeID].childID)
	{
		returnBehaviour->AddChild(CreateFromType(dataArray, childID));
	}

	ProfileReturn(returnBehaviour);
}

IBehaviour* BehaviouralTreeFactory::CreateRandomSelector(NodeData* dataArray, int nodeID)
{
	StartProfile;
	RandomSelector* returnBehaviour = new RandomSelector(nullptr, nullptr);
	for (auto &childID : dataArray[nodeID].childID)
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

int BehaviouralTreeFactory::LoadTreeFromResourceHandler(const Utilz::GUID& GUID, void* data, size_t size)
{
	StartProfile;

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
	}while (lineSplitIt != lineSplit.end());

	/*Build the tree from the dataArray*/
	if(!CreateTreeFromNodeData(GUID, dataArray, numberOfNodes))
	{
		delete[] dataArray;
		ProfileReturnConst(-1);
	}
	/*Return 0 for success, -1 for fail, 1 for refcount*/
	delete[] dataArray;
	ProfileReturnConst(1);
}

bool BehaviouralTreeFactory::LoadTree(const Utilz::GUID& guid)
{
	StartProfile;

	auto& loadedTree = behaviouralTrees.find(guid);
	if(loadedTree != behaviouralTrees.end())
	{
		ProfileReturnConst(true);
	}

	const auto done = Core::Engine::GetInstance().GetResourceHandler()->LoadResource(guid,
	{ this, &BehaviouralTreeFactory::LoadTreeFromResourceHandler });

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
