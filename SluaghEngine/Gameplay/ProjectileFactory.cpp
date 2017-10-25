#include <Gameplay\ProjectileFactory.h>
#include <Profiler.h>
#include <Core/IEngine.h>
#include "EnemyUnit.h"

#include "CoreInit.h"
void SE::Gameplay::ProjectileFactory::CreateNewProjectile(const ProjectileData& data)
{
	StartProfile;

	LoadNewProjectiles(data);
	
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
	float projectileWidth, projectileHeight, rotationAroundUnit, distanceFromUnit, projectileRotation, rotationPerSec, projectileSpeed, timeToLive;
	std::string meshName;
	float meshScale;
	std::string particleEffect;
	int nrOfBehaviours;
	std::string behaviour;

	GetLine(fileData, line, position);
	fileVersion = (float)atof(line.c_str());
	GetLine(fileData, line, position);
	nrOfProjectilesToParse = atoi(line.c_str());

	for (int i = 0; i < nrOfProjectilesToParse; i++)
	{
		ProjectileData projData = data;

		GetLine(fileData, line, position);
		projectileWidth = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		projectileHeight = (float)atof(line.c_str());

		GetLine(fileData, line, position);
		rotationAroundUnit = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		distanceFromUnit = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		projectileRotation = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		rotationPerSec = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		projectileSpeed = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		timeToLive = (float)atof(line.c_str());

		GetLine(fileData, line, position);
		meshName = line;
		GetLine(fileData, line, position);
		meshScale = (float)atof(line.c_str());
		GetLine(fileData, line, position);
		particleEffect = line;
		GetLine(fileData, line, position);
		nrOfBehaviours = atoi(line.c_str());

		projData.startRotation += rotationAroundUnit;

		projData.startPosX += sinf(projData.startRotation) * distanceFromUnit;
		projData.startPosY += cosf(projData.startRotation) * distanceFromUnit;

		projData.startRotation += projectileRotation;
		
		Rotation rotData;
		rotData.force = rotationPerSec;

		Projectile temp(projData, rotData, projectileSpeed, timeToLive, projectileWidth, projectileHeight, data.target, data.eventDamage, data.eventHealing, data.eventCondition);

		CoreInit::managers.transformManager->SetPosition(temp.GetEntity(), DirectX::XMFLOAT3(projData.startPosX, 0.5f, projData.startPosY));
		CoreInit::managers.transformManager->SetRotation(temp.GetEntity(), 0.0f, projData.startRotation, 0.0f);
		CoreInit::managers.transformManager->SetScale(temp.GetEntity(), DirectX::XMFLOAT3(meshScale, meshScale, meshScale));

		CoreInit::managers.renderableManager->CreateRenderableObject(temp.GetEntity(), { Utilz::GUID(meshName) });
		CoreInit::managers.renderableManager->ToggleRenderableObject(temp.GetEntity(), true);

		for (int j = 0; j < nrOfBehaviours; j++)
		{
			GetLine(fileData, line, position);
			AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, ParseBehaviour(temp, data.ownerUnit,line.c_str()));
		}

		newProjectiles.push_back(temp);

	}

	StopProfile;
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::ParseBehaviour(Projectile & p, std::weak_ptr<GameUnit*> ownerUnit,const char * fileData)
{
	StartProfile;

	const unsigned int maxValueSize = 50;
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
		else if (currentType == 'f' || currentType == 'i' || currentType == 'b')
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
				else if (fileData[position] == 'B' && nrOfBrackets == 1)
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
			parameterData[0] = functionIndex;
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

	const unsigned int maxValueSize = 20;
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

	parameters.push_back(temp);

	StopProfile;
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::IFCaseBehaviour(std::vector<BehaviourParameter> parameters)
{
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

	return Brancher;
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::BounceBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
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

	return bounce;

	//AddBehaviourToProjectile(projectile, type, bounce);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::SpeedModifierBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	float speedModifier = std::get<float>(parameters[0].data);

	auto speed = [speedModifier](Projectile* p, float dt) -> bool
	{
		float speed = p->GetSpeed();
		speed += speed * dt * speedModifier;
		p->SetSpeed(speed);

		return true;
	};

	return speed;

	//AddBehaviourToProjectile(projectile, type, speed);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::RotationModifierBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	float rotationModifier = std::get<float>(parameters[0].data);

	auto rotation = [rotationModifier](Projectile* p, float dt) -> bool
	{
		Rotation rotation = p->GetRotationStyle();
		rotation.force += rotation.force * dt * rotationModifier;
		p->SetRotationStyle(rotation);

		return true;
	};

	return rotation;

	//AddBehaviourToProjectile(projectile, type, rotation);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::RotationInvertionBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	BehaviourData data;
	float intervall = std::get<float>(parameters[0].data);
	data.f = intervall;

	int dataIndex = std::get<Projectile*>(parameters[1].data)->AddBehaviourData(data);

	auto inverter = [dataIndex, intervall](Projectile* p, float dt) -> bool
	{
		float& timer = p->GetBehaviourData(dataIndex).f;

		timer -= dt;

		if (timer <= 0.0f)
		{
			Rotation temp = p->GetRotationStyle();
			temp.force *= -1;
			p->SetRotationStyle(temp);
			timer = intervall;
		}

		return true;
	};

	return inverter;

	//AddBehaviourToProjectile(projectile, type, inverter);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::LifeTimeBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	float timeToIncrease = std::get<float>(parameters[0].data);

	auto timeIncreaser = [timeToIncrease](Projectile* p, float dt) -> bool
	{
		float lifeTime = p->GetLifeTime();
		lifeTime += timeToIncrease;
		p->SetLifeTime(lifeTime);
		p->SetActive(true);

		return false;
	};

	return timeIncreaser;

	//AddBehaviourToProjectile(projectile, type, timeIncreaser);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::TargetClosestEnemyBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
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

		return true;
	};

	return targeter;

	//AddBehaviourToProjectile(projectile, type, targeter);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::TimeConditionRunBehaviour(std::vector<BehaviourParameter> parameters)
{
	BehaviourData data;
	data.f = std::get<float>(parameters[0].data);
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> functions = std::get<std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[1].data);
	int dataIndex = std::get<Projectile*>(parameters[2].data)->AddBehaviourData(data);

	auto timer = [dataIndex, functions](Projectile* p, float dt) -> bool
	{
		float& timer = p->GetBehaviourData(dataIndex).f;

		if (timer > 0.0f)
		{
			timer -= dt;

			return true;
		}
		else
		{
			for(int i = 0; i < functions.size(); i++)
				functions[i](p, dt);

			return false;
		}
	};

	return timer;
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::TimeConditionAddBehaviour(std::vector<BehaviourParameter> parameters)
{
	BehaviourData data;
	data.f = std::get<float>(parameters[0].data);
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> functions = std::get<std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[1].data);
	int dataIndex = std::get<Projectile*>(parameters[2].data)->AddBehaviourData(data);

	auto timer = [dataIndex, functions](Projectile* p, float dt) -> bool
	{
		float& timer = p->GetBehaviourData(dataIndex).f;

		if (timer > 0.0f)
		{
			timer -= dt;

			return true;
		}
		else
		{
			for (int i = 0; i < functions.size(); i++)
			{
				p->AddContinuousFunction(functions[i]);
			}

			return false;
		}
	};

	return timer;
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
TargetPlayerBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
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

	return targeter;
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::LineOfSightConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
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

	return LineOfSight;
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
LockToPlayerBehaviour(std::vector<BehaviourParameter> parameters)
{
	PlayerUnit* player = ptrs.player;
	
	auto LockToPlayer = [player](Projectile* p, float dt) -> bool
	{
		p->SetXPosition(player->GetXPosition());
		p->SetYPosition(player->GetYPosition());
		return false;
	};

	return LockToPlayer;
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::KillSelfBehaviour(std::vector<BehaviourParameter> parameters)
{
	
	auto Suicide = [](Projectile* p, float dt) -> bool
	{
		p->SetActive(false);
		return false;
	};

	return Suicide;
}


std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
StunOwnerUnitBehaviour(std::vector<BehaviourParameter> parameters)
{
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[0].data);
	auto StunOwnerUnit = [ownerPtr](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			ConditionEvent cEvent;
			cEvent.type = ConditionEvent::ConditionTypes::CONDITION_TYPE_STUN;
			cEvent.duration = 2.25f;
			unit->AddConditionEvent(cEvent);
		}

		return false;
	};

	return StunOwnerUnit;
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
LifeStealBehaviour(std::vector<BehaviourParameter> parameters)
{
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

	return LifeSteal;
}

void SE::Gameplay::ProjectileFactory::AddBehaviourToProjectile(Projectile & p, TypeOfFunction type, const std::function<bool(Projectile*projectile, float dt)>& func)
{
	if (type == TypeOfFunction::CONTINUOUS)
	{
		p.AddContinuousFunction(func);
	}
	else if (type == TypeOfFunction::ON_COLLISION)
	{
		p.AddCollisionFunction(func);
	}
	else if(type == TypeOfFunction::ON_DEATH)
	{
		p.AddDeathFunction(func);
	}

}

SE::Gameplay::ProjectileFactory::ProjectileFactory()
{
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::BounceBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SpeedModifierBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::RotationModifierBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::RotationInvertionBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LifeTimeBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::TargetClosestEnemyBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::TimeConditionRunBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::TimeConditionAddBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::StunOwnerUnitBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LifeStealBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::IFCaseBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LineOfSightConditionBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LockToPlayerBehaviour, this, std::placeholders::_1));
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::KillSelfBehaviour, this, std::placeholders::_1));
}

SE::Gameplay::ProjectileFactory::~ProjectileFactory()
{
	StartProfile;
	/*
	* Code body
	*/
	ProfileReturnVoid;
}

