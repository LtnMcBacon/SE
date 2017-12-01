#include "BehaviouralTree.h"
#include "IBehaviour.h"
#include <Profiler.h>
#include "EnemyBlackboard.h"
#include "CoreInit.h"
#include "../Core/ImGuiConsole.h"
using namespace SE;
using namespace Gameplay;

Utilz::CPUTimeCluster BehaviouralTree::behaviouralTimer;
bool BehaviouralTree::firstInitialize = true;
size_t BehaviouralTree::numberOfActiveTrees = 0;
bool BehaviouralTree::firstTick = true;
size_t BehaviouralTree::memoryUsage = 0;

BehaviouralTree* BehaviouralTree::CopyTree(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const
{
	StartProfile;
	if(firstInitialize)
	{
		firstInitialize = false;
		CoreInit::subSystems.devConsole->AddFrameCallback(
			[this]() {

			ImGui::SetCurrentContext((ImGuiContext*)CoreInit::subSystems.devConsole->GetContext());

			firstTick = true;
			static bool BehaviouralTree;
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("AI"))
				{
					ImGui::MenuItem("BehaviouralTree Information", nullptr, &BehaviouralTree);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			if (BehaviouralTree)
			{
				auto times = behaviouralTimer.GetTime("BehaviouralTreeUpdate");
				ImGui::TextUnformatted("AI Time/Frame (ms):"); ImGui::SameLine(0, 33); ImGui::TextUnformatted(std::to_string(times).c_str());
				ImGui::TextUnformatted("Active Trees:"); ImGui::SameLine(0, 39); ImGui::TextUnformatted(std::to_string(numberOfActiveTrees).c_str());
				ImGui::TextUnformatted("Time/Tree (ms):"); ImGui::SameLine(0, 37); ImGui::TextUnformatted(std::to_string(times/numberOfActiveTrees).c_str());
				ImGui::TextUnformatted("Trees memory usage(bytes):"); ImGui::SameLine(0, 26); ImGui::TextUnformatted(std::to_string(memoryUsage).c_str());
				ImGui::TextUnformatted("Average memory/tree(bytes):"); ImGui::SameLine(0, 25); ImGui::TextUnformatted(std::to_string(memoryUsage/float(numberOfActiveTrees)).c_str());

			}
		}
		);
	}
	IBehaviour* root = rootBehaviour->CopyBehaviour(gameBlackboard, enemyBlackboard);

	ProfileReturn(new BehaviouralTree(root));
	
}

BehaviouralTree::BehaviouralTree(IBehaviour* root) :
	rootBehaviour(root)
{
	memoryUsage += rootBehaviour->SizeOfBehaviour();
}

BehaviouralTree::~BehaviouralTree()
{
	memoryUsage -= rootBehaviour->SizeOfBehaviour();
	delete rootBehaviour;
}

void BehaviouralTree::Tick()
{
	if (firstTick)
	{
		numberOfActiveTrees = 0;
		firstTick = false;
	}
	behaviouralTimer.Start("BehaviouralTreeUpdate");
	rootBehaviour->Tick();
	behaviouralTimer.Stop("BehaviouralTreeUpdate");
	numberOfActiveTrees++;
}

size_t BehaviouralTree::SizeOfTree()
{
	return rootBehaviour->SizeOfBehaviour() + sizeof(EnemyBlackboard);
}
