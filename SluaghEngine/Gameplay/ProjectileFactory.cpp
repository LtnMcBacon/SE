#include <Gameplay\ProjectileFactory.h>
#include <Profiler.h>
#include <Core/IEngine.h>
#include <Gameplay/ProjectileManager.h>
#include <Gameplay/PlayerUnit.h>
#include <Gameplay/Room.h>
#include "EnemyUnit.h"

#include "CoreInit.h"
void SE::Gameplay::ProjectileFactory::CreateNewProjectile(const ProjectileData& data)
{
	StartProfile;

	std::map<SE::Utilz::GUID, std::vector<LoadedProjectile>>::const_iterator got = loadedProjectiles.find(data.fileNameGuid);

	if (got == loadedProjectiles.end())
	{
		LoadNewProjectiles(data);
	}
	else
	{
		for (int i = 0; i < got->second.size(); i++)
		{
			ProjectileData projData = data;
			LoadedProjectile loaded = got->second[i];

			projData.startRotation += loaded.rotationAroundUnit;

			projData.startPosX += sinf(projData.startRotation) * loaded.distanceFromUnit;
			projData.startPosY += cosf(projData.startRotation) * loaded.distanceFromUnit;

			projData.startRotation += loaded.projectileRotation;

			Rotation rotData;
			rotData.force = loaded.rotationPerSec;

			ProjectileCreationData inputData;
			inputData.rot = rotData;
			inputData.projectileSpeed = loaded.projectileSpeed;
			inputData.projectileLifeTime = loaded.timeToLive;
			inputData.height = loaded.projectileHeight;
			inputData.width = loaded.projectileWidth;
			inputData.projectileTarget = projData.target;
			inputData.generation = projectileGeneration;

			Projectile temp(inputData, projData);

			CoreInit::managers.transformManager->SetPosition(temp.GetEntity(), DirectX::XMFLOAT3(projData.startPosX, 0.5f, projData.startPosY));
			CoreInit::managers.transformManager->SetRotation(temp.GetEntity(), 0.0f, projData.startRotation, 0.0f);
			CoreInit::managers.transformManager->SetScale(temp.GetEntity(), DirectX::XMFLOAT3(loaded.meshScale, loaded.meshScale, loaded.meshScale));

			auto owner = data.ownerUnit.lock();
			if (loaded.boundToOwner && owner)
			{
				auto unit = *owner.get();
				CoreInit::managers.transformManager->BindChild(unit->GetEntity(), temp.GetEntity());
			}

			Core::IMaterialManager::CreateInfo projectileInfo;
			Utilz::GUID material = Utilz::GUID("Cube.mat");
			Utilz::GUID shader = Utilz::GUID("SimpleNormMapPS.hlsl");
			projectileInfo.shader = shader;
			projectileInfo.materialFile = material;
			CoreInit::managers.materialManager->Create(temp.GetEntity(), projectileInfo);

			CoreInit::managers.renderableManager->CreateRenderableObject(temp.GetEntity(), { Utilz::GUID(loaded.meshName) });
			CoreInit::managers.renderableManager->ToggleRenderableObject(temp.GetEntity(), true);

			for (int j = 0; j < loaded.nrOfBehaviours; j++)
			{
				temp.AddBehaviourFunction(ParseBehaviour(temp, data.ownerUnit, got->second[i].behaviours[j].c_str()));
			}

			newProjectiles.push_back(temp);
			
		}
	}

	projectileGeneration++;
	
	StopProfile;
	
}

void SE::Gameplay::ProjectileFactory::GetLine(const std::string& file, std::string& line, int& pos)
{
	StartProfile;

	int nrOfLetters = 0;
	int startPos = pos;
	while (startPos < file.size() && file[startPos] != '\r' && file[startPos + 1] != '\n')
	{
		nrOfLetters++;
		startPos++;
	}

	line = std::string(file.begin() + pos, file.begin() + pos + nrOfLetters);
	pos += nrOfLetters + 2;

	StopProfile;
}

void SE::Gameplay::ProjectileFactory::LoadNewProjectiles(const ProjectileData & data)
{
	StartProfile;

	std::string fileData;

	auto lambda = [&fileData](const SE::Utilz::GUID & GUID, void * data, size_t size)
	{
		fileData = std::string((char*)data, size);
		return ResourceHandler::InvokeReturn::DecreaseRefcount;
	};

	const auto done = CoreInit::subSystems.resourceHandler->LoadResource(data.fileNameGuid, lambda);

	std::string line;
	int position = 0;
	float fileVersion = -1.0f;
	int nrOfProjectilesToParse = -1;
	LoadedProjectile loaded;

	GetLine(fileData, line, position);
	fileVersion = (float)atof(line.c_str());
	GetLine(fileData, line, position);
	nrOfProjectilesToParse = atoi(line.c_str());

	for (int i = 0; i < nrOfProjectilesToParse; i++)
	{
		ProjectileData projData = data;

		GetLine(fileData, line, position);
		loaded.projectileWidth = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		loaded.projectileHeight = (float)atof(line.c_str());

		GetLine(fileData, line, position);
		loaded.rotationAroundUnit = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		loaded.distanceFromUnit = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		loaded.projectileRotation = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		loaded.rotationPerSec = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		loaded.projectileSpeed = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		loaded.timeToLive = (float)atof(line.c_str());

		if (fileVersion < 1.1)
		{
			loaded.boundToOwner = false;
		}
		else
		{
			GetLine(fileData, line, position);
			loaded.boundToOwner = line[0] - 48;
		}

		GetLine(fileData, line, position);
		loaded.meshName = line;
		GetLine(fileData, line, position);
		loaded.meshScale = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		loaded.particleEffect = line;
		GetLine(fileData, line, position);
		loaded.nrOfBehaviours = atoi(line.c_str());

		projData.startRotation += loaded.rotationAroundUnit;

		projData.startPosX += sinf(projData.startRotation) * loaded.distanceFromUnit;
		projData.startPosY += cosf(projData.startRotation) * loaded.distanceFromUnit;

		projData.startRotation += loaded.projectileRotation;
		
		Rotation rotData;
		rotData.force = loaded.rotationPerSec;

		ProjectileCreationData inputData;
		inputData.rot = rotData;
		inputData.projectileSpeed = loaded.projectileSpeed;
		inputData.projectileLifeTime = loaded.timeToLive;
		inputData.height = loaded.projectileHeight;
		inputData.width = loaded.projectileWidth;
		inputData.projectileTarget = projData.target;
		inputData.generation = projectileGeneration;

		Projectile temp(inputData, projData);

		CoreInit::managers.transformManager->SetPosition(temp.GetEntity(), DirectX::XMFLOAT3(projData.startPosX, 0.5f, projData.startPosY));
		CoreInit::managers.transformManager->SetRotation(temp.GetEntity(), 0.0f, projData.startRotation, 0.0f);
		CoreInit::managers.transformManager->SetScale(temp.GetEntity(), DirectX::XMFLOAT3(loaded.meshScale, loaded.meshScale, loaded.meshScale));

		auto owner = data.ownerUnit.lock();
		if (loaded.boundToOwner && owner)
		{
			auto unit = *owner.get();
			CoreInit::managers.transformManager->BindChild(unit->GetEntity(), temp.GetEntity());
		}

		Core::IMaterialManager::CreateInfo projectileInfo;
		Utilz::GUID material = Utilz::GUID("Placeholder_Block.mat");
		Utilz::GUID shader = Utilz::GUID("SimpleNormMapPS.hlsl");
		projectileInfo.shader = shader;
		projectileInfo.materialFile = material;
		CoreInit::managers.materialManager->Create(temp.GetEntity(), projectileInfo);

		CoreInit::managers.renderableManager->CreateRenderableObject(temp.GetEntity(), { Utilz::GUID(loaded.meshName) });
		CoreInit::managers.renderableManager->ToggleRenderableObject(temp.GetEntity(), true);

		for (int j = 0; j < loaded.nrOfBehaviours; j++)
		{
			GetLine(fileData, line, position);
			temp.AddBehaviourFunction(ParseBehaviour(temp, data.ownerUnit, line.c_str()));
			loaded.behaviours.push_back(line.c_str());
		}

		newProjectiles.push_back(temp);

		loadedProjectiles[data.fileNameGuid].push_back(loaded);

	}

	StopProfile;
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::ParseBehaviour(Projectile & p, std::weak_ptr<GameUnit*> ownerUnit,const char * fileData)
{
	StartProfile;

	const unsigned int maxValueSize = 200;
	unsigned int functionToReturnIndex = 0;
	unsigned int counter = 0;
	do
	{
		functionToReturnIndex *= pow(10,counter);

		functionToReturnIndex += fileData[counter] - 48;

	}while(fileData[++counter] != '(');

	std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters;
	int position = counter + 1;
	char parameterData[maxValueSize];
	char currentType = 0;


	while (fileData[position] != ')')
	{
		currentType = fileData[position];
		if(currentType == 'O')
		{
			BehaviourParameter temp;
			temp.data = ownerUnit;
			parameters.push_back(temp);
		}
		if (currentType == 'P')
		{
			BehaviourParameter temp;
			temp.data = &p;
			parameters.push_back(temp);
		}
		else if (currentType == 'f' || currentType == 'i' || currentType == 'b' || currentType == 's')
		{
			unsigned int startPosition = position;

			while (fileData[position] != '}')
			{
				position++;
			}

			memcpy(parameterData, fileData + startPosition, position - startPosition + 1);

			ParseValue(parameters, parameterData);
		}
		else if (currentType == 'L')
		{
			unsigned int nrOfBrackets = 1;
			position++;
			unsigned int startPosition = position;
			unsigned int currentPosition = position + 1;
			unsigned int nrOfFunctionsInList = 0;
			unsigned int nrOfParantheses = 0;

			do
			{
				position++;

				if (fileData[position] == '[')
				{
					nrOfBrackets++;
				}
				else if (fileData[position] == ']')
				{
					nrOfBrackets--;
				}
				else if (fileData[position] == '(')
				{
					nrOfParantheses++;
				}
				else if (fileData[position] == ')')
				{
					nrOfParantheses--;
				}
				else if (fileData[position] == 'B' && nrOfBrackets == 1 && nrOfParantheses == 0)
				{
					nrOfFunctionsInList++;
				}

			} while (nrOfBrackets > 0);

			std::vector<std::function<bool(Projectile* projectile, float dt)>> functionsInList;

			for (int i = 0; i < nrOfFunctionsInList; i++)
			{
				unsigned int nrOfParantheses = 1;
				int powCounter = 0;
				counter = currentPosition + 1;
				unsigned int functionIndex = 0;
				do
				{
					functionIndex *= pow(10, powCounter);

					functionIndex += fileData[counter] - 48;
					powCounter++;

				} while (fileData[++counter] != '(');
				currentPosition += powCounter + 1;
				unsigned int innerStartPosition = currentPosition;

				do
				{
					currentPosition++;

					if (fileData[currentPosition] == '(')
					{
						nrOfParantheses++;
					}
					else if (fileData[currentPosition] == ')')
					{
						nrOfParantheses--;
					}

				} while (nrOfParantheses > 0);
				for (int i = powCounter - 1; i > 0; i--)
				{
					parameterData[i] = functionIndex % int(pow(10, i)) + 48;
					functionIndex = functionIndex / 10;
				}
				parameterData[0] = functionIndex+48;
				memcpy(parameterData + powCounter, fileData + innerStartPosition, currentPosition - innerStartPosition + 1);
			
				functionsInList.push_back(ParseBehaviour(p, ownerUnit,parameterData));

				currentPosition += 2;
			}

			BehaviourParameter temp;
			temp.data = functionsInList;
			parameters.push_back(temp);

		}
		else if (currentType == 'B')
		{
			unsigned int nrOfParantheses = 1;
			counter = position + 1;
			int powCounter = 0;
			unsigned int functionIndex = 0;
			do
			{
				functionIndex *= pow(10, powCounter);

				functionIndex += fileData[counter] - 48;
				powCounter++;

			} while (fileData[++counter] != '(');
			position += powCounter + 1;
			unsigned int startPosition = position;

			do
			{
				position++;

				if (fileData[position] == '(')
				{
					nrOfParantheses++;
				}
				else if (fileData[position] == ')')
				{
					nrOfParantheses--;
				}
				
			} while (nrOfParantheses > 0);

			for (int i = powCounter-1; i > 0; i--)
			{
				parameterData[i] = functionIndex % int(pow(10, i-1)) + 48;
				functionIndex = functionIndex / 10;
			}
			parameterData[0] = functionIndex + 48;
			memcpy(parameterData + powCounter, fileData + startPosition, position - startPosition + 1);

			BehaviourParameter temp;
			temp.data = std::vector<std::function<bool(Projectile* projectile, float dt)>>();
			std::get<std::vector<std::function<bool(Projectile* projectile, float dt)>>>(temp.data).push_back(ParseBehaviour(p, ownerUnit, parameterData));
			parameters.push_back(temp);

		}

		if (fileData[position + 1] == ',')
			position += 2;
		else
			position++;
	}

	ProfileReturn(behaviourFunctions[functionToReturnIndex](parameters));
}

void SE::Gameplay::ProjectileFactory::ParseValue(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> & parameters, const char * valueData)
{
	StartProfile;

	const unsigned int maxValueSize = 50;
	unsigned int iterator = 2;
	char value[20] = { NULL };
	BehaviourParameter temp;

	if (valueData[0] == 'f')
	{
		while (valueData[iterator] != '}')
		{
			value[iterator - 2] = valueData[iterator];
			iterator++;
		}

		temp.data = float(atof(value));

	}
	else if (valueData[0] == 'i')
	{
		while (valueData[iterator] != '}')
		{
			value[iterator - 2] = valueData[iterator];
			iterator++;
		}

		temp.data = atoi(value);
	}
	else if (valueData[0] == 'b')
	{
		if (valueData[2] == '1')
			temp.data = true;
		else
			temp.data = false;
	}
	else if (valueData[0] == 's')
	{
		while (valueData[iterator] != '}')
		{
			value[iterator - 2] = valueData[iterator];
			iterator++;
		}

		temp.data = std::string(value, iterator - 2);
	}

	parameters.push_back(temp);

	StopProfile;
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::IFCaseBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> conditions = std::get<std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[0].data);
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> ifTrue = std::get<std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[1].data);
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> ifFalse = std::get<std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[2].data);

	auto Brancher = [conditions, ifTrue, ifFalse](Projectile* p, float dt) -> bool
	{
		bool allTrue = true;

		for (int i = 0; i < conditions.size(); i++)
		{
			if (!conditions[i](p, dt))
			{
				allTrue = false;
				break;
			}
		}

		if (allTrue)
		{
			for (int i = 0; i < ifTrue.size(); i++)
			{
				ifTrue[i](p, dt);
			}
		}
		else
		{
			for (int i = 0; i < ifFalse.size(); i++)
			{
				ifFalse[i](p, dt);
			}
		}

		return true;
	};

	ProfileReturnConst(Brancher);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::ANDConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> arguments = std::get<std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[0].data);

	auto AND = [arguments](Projectile* p, float dt) -> bool
	{
		for (int i = 0; i < arguments.size(); i++)
		{
			if (!arguments[i](p, dt))
			{
				return false;
			}
		}

		return true;
	};

	ProfileReturnConst(AND);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::ORConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> arguments = std::get<std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[0].data);

	auto OR = [arguments](Projectile* p, float dt) -> bool
	{
		for (int i = 0; i < arguments.size(); i++)
		{
			if (arguments[i](p, dt))
			{
				return true;
			}
		}

		return false;
	};

	ProfileReturnConst(OR);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::BounceBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	auto bounce = [](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() != CollisionType::OBJECT)
			return false;

		DirectX::XMFLOAT3 currentDir = CoreInit::managers.transformManager->GetForward(p->GetEntity());
		DirectX::XMVECTOR newDir = DirectX::XMVector3Reflect({ currentDir.x, currentDir.y, currentDir.z, 0.0f }, { p->GetCollisionVectorX(), 0.0f, p->GetCollisionVectorY(), 0.0f });
		CoreInit::managers.transformManager->SetForward(p->GetEntity(), newDir);
		p->SetActive(true);
		return false;
	};

	ProfileReturnConst(bounce);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::SpeedAddDynamicBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	float speedModifier = std::get<float>(parameters[0].data);

	auto speed = [speedModifier](Projectile* p, float dt) -> bool
	{
		float speed = p->GetSpeed();
		speed += speed * dt * speedModifier;
		p->SetSpeed(speed);

		return false;
	};

	ProfileReturnConst(speed);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::RotationModifierBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	float rotationModifier = std::get<float>(parameters[0].data);

	auto rotation = [rotationModifier](Projectile* p, float dt) -> bool
	{
		Rotation rotation = p->GetRotationStyle();
		rotation.force += rotation.force * dt * rotationModifier;
		p->SetRotationStyle(rotation);

		return false;
	};

	ProfileReturnConst(rotation);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::RotationInvertionBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;


	auto inverter = [](Projectile* p, float dt) -> bool
	{
		Rotation temp = p->GetRotationStyle();
		temp.force *= -1;
		p->SetRotationStyle(temp);

		return false;
	};

	ProfileReturnConst(inverter);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::LifeTimeAddStaticBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	float timeToIncrease = std::get<float>(parameters[0].data);

	auto timeIncreaser = [timeToIncrease](Projectile* p, float dt) -> bool
	{
		float lifeTime = p->GetLifeTime();
		lifeTime += timeToIncrease;
		p->SetLifeTime(lifeTime);
		p->SetActive(true);

		return false;
	};

	ProfileReturnConst(timeIncreaser);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::TargetClosestEnemyBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	Room* currentRoom = *ptrs.currentRoom;
	float rotPerSecond = std::get<float>(parameters[0].data);

	auto targeter = [currentRoom, rotPerSecond](Projectile* p, float dt) -> bool
	{
		float xTarget, yTarget;

		if (currentRoom->GetClosestEnemy(p->GetXPosition(), p->GetYPosition(), xTarget, yTarget))
		{
			DirectX::XMFLOAT3 forward = CoreInit::managers.transformManager->GetForward(p->GetEntity());

			xTarget -= p->GetXPosition();
			yTarget -= p->GetYPosition();

			float totalRot = atan2f(yTarget, xTarget) - atan2f(forward.z, forward.x);
			int sign = -1;

			if (totalRot < 0)
				totalRot = DirectX::XM_2PI + totalRot;

			// If the rotation needed is greater than PI, we should rotate counter clockwise
				if (totalRot > DirectX::XM_PI)
					sign = 1;

			if (totalRot > 0.0000025)
			{
				if (totalRot > rotPerSecond * dt)
					totalRot = rotPerSecond;
				totalRot *= sign;

			}

			Rotation test;
			test.force = totalRot;
			test.style = RotationStyle::SELF;
			p->SetRotationStyle(test);


		}
		else
		{
			Rotation test;
			test.force = 0.0f;
			test.style = RotationStyle::SELF;
			p->SetRotationStyle(test);
		}

		return false;
	};

	ProfileReturnConst(targeter);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::TimeConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	BehaviourData data;
	float startTime = data.f = std::get<float>(parameters[0].data);
	bool repeat = std::get<bool>(parameters[1].data);
	int dataIndex = std::get<Projectile*>(parameters[2].data)->AddBehaviourData(data);

	auto timer = [startTime, dataIndex, repeat](Projectile* p, float dt) -> bool
	{
		float& timer = p->GetBehaviourData(dataIndex).f;

		if (timer > 0.0f)
		{
			timer -= dt;

			return true;
		}
		else
		{
			if (repeat)
				timer = startTime;

			return false;
		}
	};

	ProfileReturnConst(timer);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
TargetPlayerBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	Room* currentRoom = *ptrs.currentRoom;
	PlayerUnit* player = ptrs.player;
	float rotPerSecond = std::get<float>(parameters[0].data);

	auto targeter = [currentRoom, rotPerSecond, player](Projectile* p, float dt) -> bool
	{
		float xTarget = player->GetXPosition(), 
		yTarget = player->GetYPosition();

		DirectX::XMFLOAT3 forward = CoreInit::managers.transformManager->GetForward(p->GetEntity());

		xTarget -= p->GetXPosition();
		yTarget -= p->GetYPosition();

		float totalRot = atan2f(yTarget, xTarget) - atan2f(forward.z, forward.x);
		int sign = -1;

		if (totalRot < 0)
			totalRot = DirectX::XM_2PI + totalRot;

		// If the rotation needed is greater than PI, we should rotate counter clockwise
		if (totalRot > DirectX::XM_PI)
			sign = 1;

		if (totalRot > 0.0000025)
		{
			if (totalRot > rotPerSecond * dt)
				totalRot = rotPerSecond;
			totalRot *= sign;

		}

		Rotation test;
		test.force = totalRot;
		test.style = RotationStyle::SELF;
		p->SetRotationStyle(test);

		return false;
	};

	ProfileReturnConst(targeter);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::LineOfSightConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::weak_ptr<GameUnit*> owner = std::get<std::weak_ptr<GameUnit*>>(parameters[0].data);
	Room* currentRoom = *ptrs.currentRoom;

	auto LineOfSight = [owner, currentRoom](Projectile* p, float dt) -> bool
	{
		if (auto target = owner.lock())
		{
			auto ownerPtr = *target.get();
			return currentRoom->CheckLineOfSightBetweenPoints(
				ownerPtr->GetXPosition(),
				ownerPtr->GetYPosition(),
				p->GetXPosition(),
				p->GetYPosition()
			);
		}
		else
			return false;
	};

	ProfileReturnConst(LineOfSight);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
LockToPlayerBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	PlayerUnit* player = ptrs.player;
	
	auto LockToPlayer = [player](Projectile* p, float dt) -> bool
	{
		p->PositionEntity(player->GetXPosition(), player->GetYPosition());
		return false;
	};

	ProfileReturnConst(LockToPlayer);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::KillSelfBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto Suicide = [](Projectile* p, float dt) -> bool
	{
		p->SetActive(false);
		return false;
	};

	ProfileReturnConst(Suicide);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::CollidedConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto Collided = [](Projectile* p, float dt) -> bool
	{
		return (p->GetCollisionType() != CollisionType::NONE);
	};

	ProfileReturnConst(Collided);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::IsAliveConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto IsAlive = [](Projectile* p, float dt) -> bool
	{
		return p->GetActive();
	};

	ProfileReturnConst(IsAlive);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::CloseToEnemyConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	float minDistance = std::get<float>(parameters[0].data);
	Room* currentRoom = *ptrs.currentRoom;

	auto Close = [minDistance, currentRoom](Projectile* p, float dt) -> bool
	{
		float xTarget, yTarget;
		currentRoom->GetClosestEnemy(p->GetXPosition(), p->GetYPosition(), xTarget, yTarget);

		float distance = sqrt((xTarget - p->GetXPosition()) * (xTarget - p->GetXPosition()) + (yTarget - p->GetYPosition()) * (yTarget - p->GetYPosition()));

		return distance < minDistance;
	};

	ProfileReturnConst(Close);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::CloseToPlayerConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	float minDistance = std::get<float>(parameters[0].data);
	SE::Gameplay::PlayerUnit* player = ptrs.player;

	auto Suicide = [minDistance, player](Projectile* p, float dt) -> bool
	{

		float distance = sqrt((player->GetXPosition() - p->GetXPosition()) * (player->GetXPosition() - p->GetXPosition()) + (player->GetYPosition() - p->GetYPosition()) * (player->GetYPosition() - p->GetYPosition()));

		return distance < minDistance;
	};

	ProfileReturnConst(Suicide);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::CreateProjectilesBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	std::string fileName = std::get<std::string>(parameters[0].data);

	auto ProjectileCreator = [this, fileName](Projectile* p, float dt) -> bool
	{
		ProjectileData temp;

		temp.startRotation = CoreInit::managers.transformManager->GetRotation(p->GetEntity()).y;
		temp.startPosX = p->GetXPosition();
		temp.startPosY = p->GetYPosition();
		temp.target = p->GetValidTarget();
		temp.eventDamage = DamageEvent(DamageEvent::DamageSources::DAMAGE_SOURCE_RANGED, DamageEvent::DamageTypes::DAMAGE_TYPE_PHYSICAL, 2);
		temp.ownerUnit = p->GetSharedPtr();
		temp.fileNameGuid = fileName;

		this->CreateNewProjectile(temp);
		
		return false;
	};



	ProfileReturnConst(ProjectileCreator);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::InverterBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> condition = std::get<std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[0].data)[0];

	auto Close = [condition](Projectile* p, float dt) -> bool
	{
		return !condition(p, dt);
	};

	ProfileReturnConst(Close);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::KillGenerationBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	auto pManager = ptrs.pManager;

	auto KillGeneration = [pManager](Projectile* p, float dt) mutable -> bool
	{
		auto &projectiles = pManager->GetAllProjectiles();
		for(auto &projectile : projectiles)
		{
			if (projectile.GetGeneration() == p->GetGeneration())
				projectile.SetActive(false);
		}
		return false;
	};

	ProfileReturnConst(KillGeneration);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetAliveBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	
	auto SetAlive = [](Projectile* p, float dt) mutable -> bool
	{
		p->SetActive(true);
		return false;
	};

	ProfileReturnConst(SetAlive);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::CollidedWithEnemyConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto CollisionCheck = [](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() == CollisionType::ENEMY)
			return true;
		return false;
	};

	ProfileReturnConst(CollisionCheck);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::CollidedWithObjectConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto CollisionCheck = [](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() == CollisionType::OBJECT)
			return true;
		return false;
	};

	ProfileReturnConst(CollisionCheck);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::CollidedWithPlayerConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto CollisionCheck = [](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() == CollisionType::PLAYER)
			return true;
		return false;
	};

	ProfileReturnConst(CollisionCheck);
}


std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
StunOwnerUnitBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[0].data);
	float duration = std::get<float>(parameters[1].data);
	auto StunOwnerUnit = [ownerPtr, duration](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			ConditionEvent cEvent;
			cEvent.type = ConditionEvent::ConditionTypes::CONDITION_TYPE_STUN;
			cEvent.duration = duration;
			unit->AddConditionEvent(cEvent);
		}

		return false;
	};

	ProfileReturnConst(StunOwnerUnit);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
LifeStealBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[0].data);
	float percent = std::get<float>(parameters[1].data);
	auto LifeSteal = [ownerPtr, percent](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() != CollisionType::OBJECT)
			if (auto owner = ownerPtr.lock())
			{
				auto unit = *owner.get();
				HealingEvent hEvent;
				hEvent.amount = p->GetProjectileDamageEvent().amount*percent;
				hEvent.type = HealingEvent::SourceType::SOURCE_TYPE_ENEMY_HIT;
				unit->AddHealingEvent(hEvent);
			}

		return false;
	};

	ProfileReturnConst(LifeSteal);
}

SE::Gameplay::ProjectileFactory::ProjectileFactory()
{
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::BounceBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SpeedAddDynamicBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::RotationModifierBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::RotationInvertionBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LifeTimeAddStaticBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::TargetClosestEnemyBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::TimeConditionBehaviour, this, std::placeholders::_1)); // f, b, p
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::StunOwnerUnitBehaviour, this, std::placeholders::_1)); // o, f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LifeStealBehaviour, this, std::placeholders::_1)); // o, f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::IFCaseBehaviour, this, std::placeholders::_1)); // v, v, v
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LineOfSightConditionBehaviour, this, std::placeholders::_1)); // o
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LockToPlayerBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::KillSelfBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::ANDConditionBehaviour, this, std::placeholders::_1)); // v
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::ORConditionBehaviour, this, std::placeholders::_1)); // v
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CollidedConditionBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::IsAliveConditionBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CloseToEnemyConditionBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CloseToPlayerConditionBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CreateProjectilesBehaviour, this, std::placeholders::_1)); // s
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::InverterBehaviour, this, std::placeholders::_1)); // B
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::KillGenerationBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetAliveBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CollidedWithEnemyConditionBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CollidedWithObjectConditionBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CollidedWithPlayerConditionBehaviour, this, std::placeholders::_1)); // 

}

SE::Gameplay::ProjectileFactory::~ProjectileFactory()
{
	StartProfile;
	/*
	* Code body
	*/
	int nrOfBehaviours = behaviourFunctions.size();
	for (int i = 0; i < nrOfBehaviours; i++)
	{
		behaviourFunctions.pop_back();
	}

	ProfileReturnVoid;
}

