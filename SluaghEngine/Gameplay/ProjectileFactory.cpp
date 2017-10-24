#include <Gameplay\ProjectileFactory.h>
#include <Profiler.h>
#include "CoreInit.h"
SE::Gameplay::Projectile SE::Gameplay::ProjectileFactory::CreateNewProjectile(ProjectileData data)
{
	StartProfile;
	Rotation rot;
	rot.force = 0.0f;

	SE::Gameplay::Projectile temp(data, rot, 0.5f, 6.0f, SE::Gameplay::ValidTarget::ENEMIES, data.eventDamage, data.eventHealing, data.eventCondition);

	CoreInit::managers.transformManager->SetPosition(temp.GetEntity(), DirectX::XMFLOAT3(data.startPosX, 0.5f, data.startPosY));
	CoreInit::managers.transformManager->SetRotation(temp.GetEntity(), 0.0f, data.startRotation, 0.0f);
	CoreInit::managers.transformManager->SetScale(temp.GetEntity(), DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f));

	CoreInit::managers.renderableManager->CreateRenderableObject(temp.GetEntity(), { "Placeholder_Block.mesh" });
	CoreInit::managers.renderableManager->ToggleRenderableObject(temp.GetEntity(), true);

	std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters;
	std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters2;
	parameters.resize(4);
	parameters2.resize(4);

	//AddBehaviourToProjectile(temp, TypeOfFunction::ON_COLLISION, BounceBehaviour(parameters));
	//AddRotationInvertion(temp, TypeOfFunction::CONTINUOUS, 0.25f);
	//AddRotationModifier(temp, TypeOfFunction::CONTINUOUS, 0.2f);
	//AddLifeTime(temp, TypeOfFunction::ON_DEATH, 6.0f);
	//AddRotationModifier(temp, TypeOfFunction::ON_DEATH, -0.4f);

	//parameters[0].data = 60.0f * 0.08f;
	//AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, TargetClosestEnemyBehaviour(parameters));

	//parameters2[0].data = 10.0f;

	//parameters[0].data = 1.0f;
	//std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> tempFuncs;
	//tempFuncs.push_back(SpeedModifierBehaviour(parameters2));
	//parameters[1].data = tempFuncs;
	//parameters[2].data = &temp;
	//AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, TimeConditionAddBehaviour(parameters));

	AddBehaviourToProjectile(temp, TypeOfFunction::ON_COLLISION, ParseBehaviour(temp, "0()"));
	AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, ParseBehaviour(temp, "5(f{4.8})"));
	//AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, ParseBehaviour(temp, "7(f{2.0},L[B1(f{1.0})],P)"));
	AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, ParseBehaviour(temp, "7(f{2.0},L[B1(f{1.0}),B6(f{0.0},L[B1(f{-10.0})],P)],P)")); // 7(f{ 2.0 }, L[B1(f{ 1.0 }), B6(f{ 0.0 }, L[B1(f{ -10.0 })], P)], P)

	ProfileReturnConst(temp);
	
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::ParseBehaviour(Projectile & p, char * fileData)
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

void SE::Gameplay::ProjectileFactory::ParseValue(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> & parameters, char * valueData)
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
					totalRot = rotPerSecond * dt;
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
}

SE::Gameplay::ProjectileFactory::~ProjectileFactory()
{
	StartProfile;
	/*
	* Code body
	*/
	ProfileReturnVoid;
}

