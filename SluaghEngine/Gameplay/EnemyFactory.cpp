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

void EnemyFactory::CreateEnemies(const EnemyCreationStruct &descriptions, GameBlackboard* gameBlackboard, EnemyUnit** unitArray)
{
	StartProfile;
	int numberOfCreatedEnemies = 0;
	for(auto desc : descriptions.information)
	{
		EnemyType type;
		if(desc.type == ENEMY_TYPE_RANDOM)
		{
			type = EnemyType(CoreInit::subSystems.window->GetRand() % 3);
		}
		else
		{
			type = desc.type;
		}
		auto newEnemy = CreateEnemyDataForEnemyType(type, desc.useVariation);
		newEnemy->SetZPosition(1.5f);
		newEnemy->PositionEntity(desc.startX, desc.startY);
		newEnemy->SetBehaviouralTree(CreateBehaviouralTreeForEnemyType(type, gameBlackboard, newEnemy->GetEnemyBlackboard()));
		newEnemy->SetEntity(CreateEntityDataForEnemyType(type));
		unitArray[numberOfCreatedEnemies++] = newEnemy;
	}
	
	StopProfile;
}

Core::Entity EnemyFactory::CreateEntityDataForEnemyType(EnemyType type)
{
	StartProfile;
	auto const enemyCreationData = enemyData.find(type);
	auto newEntity = CoreInit::managers.entityManager->Create();

	CoreInit::managers.transformManager->Create(newEntity);

	if (enemyCreationData != enemyData.end())
	{
		
		/*Fix with managers*/
		Core::IAnimationManager::CreateInfo cInfo;
		cInfo.animationCount = 0;
		cInfo.animations = nullptr;
		cInfo.mesh = enemyCreationData->second.meshGUID;
		cInfo.skeleton = enemyCreationData->second.skeletonGUID;

				CoreInit::managers.animationManager->CreateAnimatedObject(newEntity, cInfo);
	//	CoreInit::managers.animationManager->ToggleVisible(newEntity, true);

		Core::IMaterialManager::CreateInfo enemyInfo;
		enemyInfo.materialFile = enemyCreationData->second.materialGUID;
		enemyInfo.shader = enemyCreationData->second.shaderGUID;
		CoreInit::managers.materialManager->Create(newEntity, enemyInfo);

		CoreInit::managers.audioManager->Create(newEntity, { enemyCreationData->second.deathSoundGUID, SE::Audio::StereoVoiceSound });
		CoreInit::managers.dataManager->SetValue(newEntity, SE::Utilz::GUID("deathSoundGUID"), static_cast<uint32_t>(enemyCreationData->second.deathSoundGUID.id));


		if (type == ENEMY_TYPE_NUCKELAVEE)
		{
			//Move up
			CoreInit::managers.transformManager->Move(newEntity, DirectX::XMFLOAT3{ 0, 0.8f, 0 });

			//Insert entity for sword here.
			auto swordEntity = CoreInit::managers.entityManager->Create();

			Core::IRenderableManager::CreateInfo swordInfo;
			swordInfo.meshGUID = "Sword.mesh";
			//swordInfo.shadow = true;
			swordInfo.transparent = false;

			CoreInit::managers.transformManager->Create(swordEntity);
			CoreInit::managers.transformManager->SetPosition(swordEntity, DirectX::XMFLOAT3{ 0.2f, -0.1f, -0.5f });
			CoreInit::managers.transformManager->Rotate(swordEntity, 3.0f, -0.4f, 1.3f);
			CoreInit::managers.dataManager->SetValue(newEntity, "Weapon", swordEntity);
			CoreInit::managers.renderableManager->CreateRenderableObject(swordEntity, swordInfo);
			CoreInit::managers.renderableManager->ToggleRenderableObject(swordEntity, true);

			CoreInit::managers.animationManager->AttachToEntity(newEntity, swordEntity, "LHand", 0);
		}

		ProfileReturnConst( newEntity);
	}

	ProfileReturnConst(newEntity);
}

EnemyUnit* EnemyFactory::CreateEnemyDataForEnemyType(EnemyType type, bool useVariation)
{
	StartProfile;

	auto const enemyCreationData = enemyData.find(type);

	if (enemyCreationData != enemyData.end())
	{
		EnemyUnit* createdEnemy = nullptr;
		int enemyHealth = enemyCreationData->second.baseHealth;
		if (useVariation)
		{
			int healthVariation = enemyCreationData->second.baseHealthVariation;
			enemyHealth += CoreInit::subSystems.window->GetRand() % (healthVariation * 2 + 1) - healthVariation;
		}
	

		createdEnemy = new EnemyUnit(type, nullptr, 0.f, 0.f, enemyHealth);
		createdEnemy->SetDeathAnimation(enemyCreationData->second.deathAnimationGUID);
		/*To do: Add the rest of the data here!*/

		EnemyBlackboard* enemyBlackboard = new EnemyBlackboard;

		createdEnemy->SetEnemyBlackboard(enemyBlackboard);




		ProfileReturnConst(createdEnemy);
	}
	ProfileReturnConst(nullptr);
}

BehaviouralTree* EnemyFactory::CreateBehaviouralTreeForEnemyType(EnemyType type, GameBlackboard* gameBlackboard,
	EnemyBlackboard* enemyBlackboard)
{
	StartProfile;
	auto const enemyCreationData = enemyData.find(type);

	if (enemyCreationData != enemyData.end())
	{
		ProfileReturnConst(SEBTFactory->BuildBehaviouralTree(
			enemyCreationData->second.behaviouralTreeGUID,
			gameBlackboard,
			enemyBlackboard));
	}
	ProfileReturnConst(nullptr);
}

EnemyFactory::EnemyFactory()
{
	this->SEBTFactory = new BehaviouralTreeFactory();
	this->enemyTypes["Bodach.SEC"] = ENEMY_TYPE_BODACH;
	this->enemyTypes["Glaistig.SEC"] = ENEMY_TYPE_GLAISTIG;
	this->enemyTypes["Nuckelavee.SEC"] = ENEMY_TYPE_NUCKELAVEE;
	/*this->enemyTypes["PechMelee.SEC"] = ENEMY_TYPE_PECH_MELEE;
	this->enemyTypes["PechRanged.SEC"] = ENEMY_TYPE_PECH_RANGED;*/
	this->LoadEnemyIntoMemory("Bodach.SEC");
	this->LoadEnemyIntoMemory("Glaistig.SEC");
	this->LoadEnemyIntoMemory("Nuckelavee.SEC");
	/*this->LoadEnemyIntoMemory("PechMelee.SEC");
	this->LoadEnemyIntoMemory("PechRanged.SEC");*/
}

EnemyFactory::~EnemyFactory()
{
	delete this->SEBTFactory;
}

bool EnemyFactory::LoadEnemyIntoMemory(Utilz::GUID GUID)
{
	StartProfile;

	auto& loadedTree = enemyData.find(GUIDToEnemyType(GUID));
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
		loadedEnemy.skeletonGUID = Utilz::GUID(GetLineData(line));
		++line;
		line->pop_back();
		loadedEnemy.deathAnimationGUID = Utilz::GUID(GetLineData(line));
		++line;
		line->pop_back();
		loadedEnemy.behaviouralTreeGUID = Utilz::GUID(GetLineData(line));
		++line;
		line->pop_back();
		loadedEnemy.materialGUID = Utilz::GUID(GetLineData(line));
		++line;
		line->pop_back();
		loadedEnemy.shaderGUID = Utilz::GUID(GetLineData(line));
		++line;
		line->pop_back();
		loadedEnemy.deathSoundGUID = Utilz::GUID(GetLineData(line));
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

		enemyData[this->GUIDToEnemyType(guid)] = loadedEnemy;

		return ResourceHandler::InvokeReturn::SUCCESS;
	});

	if (done != -1)
		ProfileReturnConst(true);
	ProfileReturnConst(false);
}
