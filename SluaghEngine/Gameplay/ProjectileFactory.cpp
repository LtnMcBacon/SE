#include <Gameplay\ProjectileFactory.h>
#include <Profiler.h>
#include <Core\CollisionManager.h>
#include "Core/Engine.h"
#include "EnemyUnit.h"

void SE::Gameplay::ProjectileFactory::CreateNewProjectile(const ProjectileData& data)
{
	StartProfile;
	//Rotation rot;
	//rot.force = 0.0f;

	//SE::Gameplay::Projectile temp(data, rot, 0.5f, 6.0f, 0.1f, 0.1f, SE::Gameplay::ValidTarget::ENEMIES, data.eventDamage, data.eventHealing, data.eventCondition);

	//auto& e = Core::Engine::GetInstance();
	//auto& em = e.GetEntityManager();
	//auto& rm = e.GetRenderableManager();
	//auto& tm = e.GetTransformManager();

	//tm.SetPosition(temp.GetEntity(), DirectX::XMFLOAT3(data.startPosX, 0.5f, data.startPosY));
	//tm.SetRotation(temp.GetEntity(), 0.0f, data.startRotation, 0.0f);
	//tm.SetScale(temp.GetEntity(), DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f));

	//rm.CreateRenderableObject(temp.GetEntity(), Utilz::GUID("Placeholder_Block.mesh"));
	//rm.ToggleRenderableObject(temp.GetEntity(), true);

	//std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters;
	//std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters2;
	//parameters.resize(4);
	//parameters2.resize(4);

	//AddBehaviourToProjectile(temp, TypeOfFunction::ON_COLLISION, BounceBehaviour(parameters));
	//AddRotationInvertion(temp, TypeOfFunction::CONTINUOUS, 0.25f);
	//AddRotationModifier(temp, TypeOfFunction::CONTINUOUS, 0.2f);
	//AddLifeTime(temp, TypeOfFunction::ON_DEATH, 6.0f);
	//AddRotationModifier(temp, TypeOfFunction::ON_DEATH, -0.4f);

	/*parameters[0].behaviour.f = 60.0f * 0.08f;
	AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, TargetClosestEnemyBehaviour(parameters));*/
	//parameters[0].data = 60.0f * 0.08f;
	//AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, TargetClosestEnemyBehaviour(parameters));

	//parameters2[0].data = 10.0f;

	//parameters[0].data = 1.0f;
	//std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> tempFuncs;
	//tempFuncs.push_back(SpeedModifierBehaviour(parameters2));
	//parameters[1].data = tempFuncs;
	//parameters[2].data = &temp;
	//AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, TimeConditionAddBehaviour(parameters));

	//AddBehaviourToProjectile(temp, TypeOfFunction::ON_COLLISION, ParseBehaviour(temp, "0()"));
	//AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, ParseBehaviour(temp, "5(f{4.8})"));
	//AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, ParseBehaviour(temp, "7(f{2.0},L[B1(f{1.0})],P)"));
	//AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, ParseBehaviour(temp, "7(f{2.0},L[B1(f{1.0}),B6(f{0.0},L[B1(f{-10.0})],P)],P)")); // 7(f{ 2.0 }, L[B1(f{ 1.0 }), B6(f{ 0.0 }, L[B1(f{ -10.0 })], P)], P)

	LoadNewProjectiles(data);
	//newProjectiles.push_back(temp);

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

	const auto done = Core::Engine::GetInstance().GetResourceHandler()->LoadResource(data.fileNameGuid, lambda);

	auto& e = Core::Engine::GetInstance();
	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();

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

		tm.SetPosition(temp.GetEntity(), DirectX::XMFLOAT3(projData.startPosX, 0.5f, projData.startPosY));
		tm.SetRotation(temp.GetEntity(), 0.0f, projData.startRotation, 0.0f);
		tm.SetScale(temp.GetEntity(), DirectX::XMFLOAT3(meshScale, meshScale, meshScale));

		rm.CreateRenderableObject(temp.GetEntity(), Utilz::GUID(meshName));
		rm.ToggleRenderableObject(temp.GetEntity(), true);

		for (int j = 0; j < nrOfBehaviours; j++)
		{
			GetLine(fileData, line, position);
			AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, ParseBehaviour(temp, line.c_str()));
		}

		newProjectiles.push_back(temp);

	}

	StopProfile;
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::ParseBehaviour(Projectile & p, const char * fileData)
{
	StartProfile;

	const unsigned int maxValueSize = 50;
	unsigned int functionToReturnIndex = fileData[0] - 48;
	std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters;
	int position = 2;
	char parameterData[maxValueSize];
	char currentType = 0;


	while (fileData[position] != ')')
	{
		currentType = fileData[position];

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
				char functionIndex = fileData[currentPosition + 1];
				currentPosition += 2;
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

				parameterData[0] = functionIndex;
				memcpy(parameterData + 1, fileData + innerStartPosition, currentPosition - innerStartPosition + 1);
			
				functionsInList.push_back(ParseBehaviour(p, parameterData));

				currentPosition += 2;
			}

			BehaviourParameter temp;
			temp.data = functionsInList;
			parameters.push_back(temp);

		}
		else if (currentType == 'B')
		{
			unsigned int nrOfParantheses = 1;
			char functionIndex = fileData[position + 1];
			position += 2;
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

			parameterData[0] = functionIndex;
			memcpy(parameterData + 1, fileData + startPosition, position - startPosition + 1);

			BehaviourParameter temp;
			temp.data = std::vector<std::function<bool(Projectile* projectile, float dt)>>();
			std::get<std::vector<std::function<bool(Projectile* projectile, float dt)>>>(temp.data).push_back(ParseBehaviour(p, parameterData));
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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::BounceBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	auto bounce = [](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() != CollisionType::OBJECT)
			return false;

		DirectX::XMFLOAT3 currentDir = Core::Engine::GetInstance().GetTransformManager().GetForward(p->GetEntity());
		DirectX::XMVECTOR newDir = DirectX::XMVector3Reflect({ currentDir.x, currentDir.y, currentDir.z, 0.0f }, { p->GetCollisionVectorX(), 0.0f, p->GetCollisionVectorY(), 0.0f });
		Core::Engine::GetInstance().GetTransformManager().SetForward(p->GetEntity(), newDir);
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
			auto& tm = Core::Engine::GetInstance().GetTransformManager();
			DirectX::XMFLOAT3 forward = tm.GetForward(p->GetEntity());

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

		auto& tm = Core::Engine::GetInstance().GetTransformManager();
		DirectX::XMFLOAT3 forward = tm.GetForward(p->GetEntity());

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

		return true;
	};

	return targeter;
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::LineOfSightConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	PlayerUnit* player = ptrs.player;
	Room* currentRoom = *ptrs.currentRoom;


	auto LineOfSight = [player, currentRoom](Projectile* p, float dt) -> bool
	{
		return currentRoom->CheckLineOfSightBetweenPoints(
			player->GetXPosition(),
			player->GetYPosition(),
			p->GetXPosition(),
			p->GetYPosition()
		);
	};

	return LineOfSight;
}


std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
StunOwnerUnitBehaviour(std::vector<BehaviourParameter> parameters)
{
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[0].data);
	auto StunOwnerUnit = [ownerPtr](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() == CollisionType::ENEMY)
			if (auto owner = ownerPtr.lock())
			{
				auto unit = *owner.get();
				ConditionEvent cEvent;
				cEvent.type = ConditionEvent::ConditionTypes::CONDITION_TYPE_STUN;
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
}

SE::Gameplay::ProjectileFactory::~ProjectileFactory()
{
	StartProfile;
	/*
	* Code body
	*/
	ProfileReturnVoid;
}

