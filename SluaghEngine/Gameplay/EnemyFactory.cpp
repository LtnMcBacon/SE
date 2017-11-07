#include "EnemyFactory.h"
#include <Profiler.h>
#include "CoreInit.h"
#include <Gameplay/BehaviouralTreeFactory.h>
#include "EnemyUnit.h"
#include "EnemyBlackboard.h"

using namespace SE;
using namespace Gameplay;

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

	inline std::string GetLineData(std::vector<std::string>::iterator& line)
	{
		return split(line[0], ':')[1];
	}

	inline int GetLineDataAsInt(std::vector<std::string>::iterator& line)
	{
		return std::stoi(GetLineData(line));
	}
}

EnemyUnit* EnemyFactory::CreateEnemy(Utilz::GUID GUID, GameBlackboard* gameBlackboard, bool useVariation)
{
	StartProfile;
	EnemyUnit* createdEnemy = nullptr;

	auto const enemyCreationData = enemyData.find(GUID);
	if (enemyCreationData == enemyData.end())
		ProfileReturn(nullptr);
	int enemyHealth = enemyCreationData->second.baseHealth;
	if (useVariation)
	{
		int healthVariation = enemyCreationData->second.baseHealthVariation;
		enemyHealth += CoreInit::subSystems.window->GetRand() % (healthVariation * 2 + 1) - healthVariation;
	}

	createdEnemy = new EnemyUnit(nullptr, 0.0f, 0.0f, enemyHealth);
	/*To do: Add the rest of the data here!*/

	EnemyBlackboard* enemyBlackboard = new EnemyBlackboard;

	createdEnemy->SetBehaviouralTree(SEBTFactory->BuildBehaviouralTree(
		enemyCreationData->second.behaviouralTreeGUID,
		gameBlackboard,
		enemyBlackboard));

	createdEnemy->SetEnemyBlackboard(enemyBlackboard);

	/*Fix with managers*/
	CoreInit::managers.renderableManager->CreateRenderableObject(createdEnemy->GetEntity(),
	{ enemyCreationData->second.meshGUID });
	// temp material

	Core::IMaterialManager::CreateInfo enemyInfo;
	Utilz::GUID material = Utilz::GUID("Cube.mat");
	Utilz::GUID shader = Utilz::GUID("SimpleNormMapPS.hlsl");
	enemyInfo.shader = shader;
	enemyInfo.materialFile = material;
	CoreInit::managers.materialManager->Create(createdEnemy->GetEntity(), enemyInfo);
	// end temp material
	CoreInit::managers.renderableManager->ToggleRenderableObject(createdEnemy->GetEntity(), true);

	ProfileReturn(createdEnemy);
}

void EnemyFactory::CreateEnemies(EnemyToCreateDescription* descriptions, GameBlackboard* gameBlackboard,
								 int numberOfEnemies, EnemyUnit** unitArray)
{
	StartProfile;
	int numberOfCreatedEnemies = 0;
	int descCounter = 0;
	while(numberOfCreatedEnemies != numberOfEnemies)
	{
		for(int i = 0; i < descriptions[descCounter].nrOfEnemiesWithThisDescription; i++)
		{
			unitArray[numberOfCreatedEnemies++] = CreateEnemy(descriptions[descCounter].GUID,
				gameBlackboard, descriptions[descCounter].useVariation);
		}
		descCounter++;
	}
	StopProfile;
}

EnemyFactory::EnemyFactory()
{
	this->SEBTFactory = new BehaviouralTreeFactory();
}

EnemyFactory::~EnemyFactory()
{
	delete this->SEBTFactory;
}

bool EnemyFactory::LoadEnemyIntoMemory(Utilz::GUID GUID)
{
	StartProfile;

	auto& loadedTree = enemyData.find(GUID);
	if (loadedTree != enemyData.end())
	{
		ProfileReturnConst(true);
	}

	const auto done = CoreInit::subSystems.resourceHandler->LoadResource(GUID,
		[this](auto guid, auto data, auto size) {
		if (!size)
			return ResourceHandler::InvokeReturn::FAIL;
		std::string const EnemyFileData((char*)data, size);
		auto lineByLineData = split(EnemyFileData, '\n');

		auto line = lineByLineData.begin();
		line->pop_back();
		EnemyDescriptionStruct loadedEnemy;
		auto temp = Utilz::GUID("Placeholder_Block.mesh");
		loadedEnemy.meshGUID = Utilz::GUID(GetLineData(line));
		++line;
		line->pop_back();
		loadedEnemy.behaviouralTreeGUID = Utilz::GUID(GetLineData(line));
		++line;
		line->pop_back();
		loadedEnemy.baseDamage = GetLineDataAsInt(line);
		++line;
		line->pop_back();
		loadedEnemy.baseDamageVariation = GetLineDataAsInt(line);
		++line;
		line->pop_back();
		loadedEnemy.baseHealth = GetLineDataAsInt(line);
		++line;
		line->pop_back();
		loadedEnemy.baseHealthVariation = GetLineDataAsInt(line);
		++line;
		line->pop_back();
		loadedEnemy.physicalResistance = GetLineDataAsInt(line);
		++line;
		line->pop_back();
		loadedEnemy.magicalResistance = GetLineDataAsInt(line);
		++line;
		line->pop_back();
		loadedEnemy.natureResistance = GetLineDataAsInt(line);
		++line;
		line->pop_back();
		loadedEnemy.fireResistance = GetLineDataAsInt(line);
		++line;
		line->pop_back();
		loadedEnemy.waterResistance = GetLineDataAsInt(line);

		if (!SEBTFactory->LoadTree(loadedEnemy.behaviouralTreeGUID))
			return ResourceHandler::InvokeReturn::FAIL;

		/*Load mesh here?*/

		enemyData[guid] = loadedEnemy;

		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	});

	if (done != -1)
		ProfileReturnConst(true);
	ProfileReturnConst(false);
}
