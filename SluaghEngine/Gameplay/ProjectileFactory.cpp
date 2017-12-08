#include <Gameplay\ProjectileFactory.h>
#include <Profiler.h>
#include <Core/IEngine.h>
#include <Gameplay/ProjectileManager.h>
#include <Gameplay/PlayerUnit.h>
#include <Gameplay/Room.h>
#include "EnemyUnit.h"
#include "CoreInit.h"
#include <DirectXMath.h>
using namespace DirectX;

void SE::Gameplay::ProjectileFactory::CreateNewProjectile(const ProjectileData& data)
{
	StartProfile;

	std::map<SE::Utilz::GUID, std::vector<LoadedProjectile>>::const_iterator got = loadedProjectiles.find(
		data.fileNameGuid);

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

			if (loaded.particleEffect != "NONE" && loaded.particleEffect != "")
			{
				CoreInit::managers.particleSystemManager->CreateSystem(temp.GetEntity(), {loaded.particleEffect});
				CoreInit::managers.particleSystemManager->ToggleVisible(temp.GetEntity(), true);
			}

			auto owner = data.ownerUnit.lock();
			if (loaded.boundToOwner && owner)
			{
				auto unit = *owner.get();
				CoreInit::managers.transformManager->BindChild(unit->GetEntity(), temp.GetEntity());
			}

			Core::IMaterialManager::CreateInfo projectileInfo;
			Utilz::GUID shader = Utilz::GUID("SimpleLightPS.hlsl");
			projectileInfo.shader = shader;

			if (loaded.materialName != "NONE" && loaded.materialName != "")
				projectileInfo.materialFile = loaded.materialName;;

			CoreInit::managers.materialManager->Create(temp.GetEntity(), projectileInfo);

			if (loaded.meshName != "NONE.mesh" && loaded.meshName != ".mesh" && loaded.meshName != "NONE" && loaded.meshName != "")
			{
				CoreInit::managers.renderableManager->CreateRenderableObject(temp.GetEntity(), { Utilz::GUID(loaded.meshName) });
				CoreInit::managers.renderableManager->ToggleRenderableObject(temp.GetEntity(), true);
			}

			if (loaded.soundName != "NONE" && loaded.soundName != "")
			{
				Core::IAudioManager::CreateInfo audioInfo;
				audioInfo.soundFile = loaded.soundName;
				audioInfo.soundType = SE::Audio::StereoPanSound;
				CoreInit::managers.audioManager->Create(temp.GetEntity(), audioInfo);
				CoreInit::managers.audioManager->PlaySound(temp.GetEntity(), loaded.soundName);
			}

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

	if (file[pos] == '\n')
	{
		pos++;
		line = "";
		ProfileReturnVoid;
	}

	int nrOfLetters = 0;
	int startPos = pos;
	int toNextLine = 2;
	while (startPos < file.size() && file[startPos] != '\r' && file[startPos + 1] != '\n')
	{
		nrOfLetters++;
		startPos++;
	}

	if ( startPos < file.size() && file[startPos] != '\r')
	{
		nrOfLetters++;
		startPos++;
		toNextLine = 1;
	}

	line = std::string(file.begin() + pos, file.begin() + pos + nrOfLetters);
	pos += nrOfLetters + toNextLine;

	StopProfile;
}

void SE::Gameplay::ProjectileFactory::LoadNewProjectiles(const ProjectileData& data)
{
	StartProfile;

	std::string fileData;

	auto lambda = [&fileData](const SE::Utilz::GUID& GUID, void* data, size_t size)
	{
		fileData = std::string((char*)data, size);
		return ResourceHandler::InvokeReturn::SUCCESS | ResourceHandler::InvokeReturn::DEC_RAM;
	};

	const auto done = CoreInit::subSystems.resourceHandler->LoadResource(data.fileNameGuid, lambda);

	std::string line;
	int position = 0;
	std::string fileVersion = "0.0";
	int nrOfProjectilesToParse = -1;
	LoadedProjectile loaded;

	GetLine(fileData, line, position);
	fileVersion = line;
	GetLine(fileData, line, position);
	nrOfProjectilesToParse = atoi(line.c_str());
	auto tempTest = this->ptrs.player;

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

		if (fileVersion < "1.1")
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

		if (fileVersion < "1.4")
		{
			loaded.materialName = "Cube.mat";
			//loaded.soundName = "DefaultAttackSound.wav";
			loaded.soundName = "NONE";
		}
		else
		{
			GetLine(fileData, line, position);
			loaded.materialName = line;
			GetLine(fileData, line, position);
			loaded.soundName = line;
		}

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

		CoreInit::managers.transformManager->SetPosition(temp.GetEntity(),
		    DirectX::XMFLOAT3(projData.startPosX, projData.startPosZ, projData.startPosY));
		CoreInit::managers.transformManager->SetRotation(temp.GetEntity(), 0.0f, projData.startRotation, 0.0f);
		CoreInit::managers.transformManager->SetScale(temp.GetEntity(), 
			DirectX::XMFLOAT3(loaded.meshScale, loaded.meshScale,loaded.meshScale));

		if (loaded.particleEffect != "NONE" && loaded.particleEffect != "")
		{
			CoreInit::managers.particleSystemManager->CreateSystem(temp.GetEntity(), {loaded.particleEffect});
			CoreInit::managers.particleSystemManager->ToggleVisible(temp.GetEntity(), true);
		}

		auto owner = data.ownerUnit.lock();
		if (loaded.boundToOwner && owner)
		{
			auto unit = *owner.get();
			CoreInit::managers.transformManager->BindChild(unit->GetEntity(), temp.GetEntity());
		}

		Core::IMaterialManager::CreateInfo projectileInfo;
		Utilz::GUID shader = Utilz::GUID("SimpleLightPS.hlsl");
		Utilz::GUID material = loaded.materialName;

		projectileInfo.shader = shader;

		if(loaded.materialName != "NONE" && loaded.materialName != "")
			projectileInfo.materialFile = material;

		CoreInit::managers.materialManager->Create(temp.GetEntity(), projectileInfo);

		if (loaded.soundName != "NONE" && loaded.soundName != "")
		{
			Core::IAudioManager::CreateInfo audioInfo;
			audioInfo.soundFile = loaded.soundName;
			audioInfo.soundType = SE::Audio::StereoPanSound;
			CoreInit::managers.audioManager->Create(temp.GetEntity(), audioInfo);
			CoreInit::managers.audioManager->PlaySound(temp.GetEntity(), loaded.soundName);
		}

		if (loaded.meshName != "NONE.mesh" && loaded.meshName != ".mesh" && loaded.meshName != "NONE" && loaded.meshName != "")
		{
			CoreInit::managers.renderableManager->CreateRenderableObject(temp.GetEntity(), { Utilz::GUID(loaded.meshName) });
			CoreInit::managers.renderableManager->ToggleRenderableObject(temp.GetEntity(), true);
		}

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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::ParseBehaviour(
	Projectile& p, std::weak_ptr<GameUnit*> ownerUnit, const char* fileData)
{
	StartProfile;

	const unsigned int maxValueSize = 200;
	unsigned int functionToReturnIndex = 0;
	unsigned int counter = 0;
	do
	{
		functionToReturnIndex *= pow(10, counter);

		functionToReturnIndex += fileData[counter] - 48;
	}
	while (fileData[++counter] != '(');

	std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters;
	int position = counter + 1;
	char parameterData[maxValueSize];
	char currentType = 0;


	while (fileData[position] != ')')
	{
		currentType = fileData[position];
		if (currentType == 'O')
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
			}
			while (nrOfBrackets > 0);

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
				}
				while (fileData[++counter] != '(');
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
				}
				while (nrOfParantheses > 0);
				for (int i = powCounter - 1; i > 0; i--)
				{
					parameterData[i] = functionIndex % int(pow(10, i)) + 48;
					functionIndex = functionIndex / 10;
				}
				parameterData[0] = functionIndex + 48;
				memcpy(parameterData + powCounter, fileData + innerStartPosition, currentPosition - innerStartPosition + 1);

				functionsInList.push_back(ParseBehaviour(p, ownerUnit, parameterData));

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
			}
			while (fileData[++counter] != '(');
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
			}
			while (nrOfParantheses > 0);

			for (int i = powCounter - 1; i > 0; i--)
			{
				parameterData[i] = functionIndex % int(pow(10, i)) + 48;
				functionIndex = functionIndex / 10;
			}
			parameterData[0] = functionIndex + 48;
			memcpy(parameterData + powCounter, fileData + startPosition, position - startPosition + 1);

			BehaviourParameter temp;
			temp.data = std::vector<std::function<bool(Projectile* projectile, float dt)>>();
			std::get<std::vector<std::function<bool(Projectile* projectile, float dt)>>>(temp.data).push_back(
				ParseBehaviour(p, ownerUnit, parameterData));
			parameters.push_back(temp);
		}

		if (fileData[position + 1] == ',')
			position += 2;
		else
			position++;
	}

	ProfileReturn(behaviourFunctions[functionToReturnIndex](parameters));
}

void SE::Gameplay::ProjectileFactory::ParseValue(
	std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter>& parameters, const char* valueData)
{
	StartProfile;

	const unsigned int maxValueSize = 50;
	unsigned int iterator = 2;
	char value[40] = {NULL};
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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::IFCaseBehaviour(
	std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> conditions = std::get<std::vector<std
		::function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[0].data);
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> ifTrue = std::get<std::vector<std::
		function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[1].data);
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> ifFalse = std::get<std::vector<std::
		function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[2].data);

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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
ANDConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> arguments = std::get<std::vector<std::
		function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[0].data);

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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
ORConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::vector<std::function<bool(SE::Gameplay::Projectile* projectile, float dt)>> arguments = std::get<std::vector<std::
		function<bool(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[0].data);

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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::BounceBehaviour(
	std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	auto bounce = [](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() != CollisionType::OBJECT)
			return false;

		DirectX::XMFLOAT3 currentDir = CoreInit::managers.transformManager->GetForward(p->GetEntity());
		DirectX::XMVECTOR newDir = DirectX::XMVector3Reflect({currentDir.x, currentDir.y, currentDir.z, 0.0f},
		                                                     {
			                                                     p->GetCollisionVectorX(), 0.0f, p->GetCollisionVectorY(), 0.0f
		                                                     });
		CoreInit::managers.transformManager->SetForward(p->GetEntity(), newDir);
		p->SetActive(true);
		return false;
	};

	ProfileReturnConst(bounce);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
SpeedAddDynamicBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	float speedModifier = std::get<float>(parameters[0].data);

	auto speed = [speedModifier](Projectile* p, float dt) -> bool
	{
		float speed = p->GetSpeed();
		speed += dt * speedModifier;
		p->SetSpeed(speed);

		return false;
	};

	ProfileReturnConst(speed);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
RotationModifierBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
RotationInvertionBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
LifeTimeAddStaticBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
TargetClosestEnemyBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	Room* currentRoom = *ptrs.currentRoom;
	float rotPerSecond = std::get<float>(parameters[0].data);
	PlayerUnit* player = ptrs.player;

	auto targeter = [currentRoom, rotPerSecond, player](Projectile* p, float dt) -> bool
	{
		float xTarget, yTarget;

		if (p->GetValidTarget() == ValidTarget::ENEMIES)
		{
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
		}
		else if (p->GetValidTarget() == ValidTarget::PLAYER)
		{
			xTarget = player->GetXPosition(),
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
		}

		return false;
	};

	ProfileReturnConst(targeter);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
TimeConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	BehaviourData data1, data2;
	float startTime = data1.f = std::get<float>(parameters[0].data);
	data2.i = std::get<int>(parameters[1].data);
	int dataIndexTimer = std::get<Projectile*>(parameters[2].data)->AddBehaviourData(data1);
	int dataIndexRepeat = std::get<Projectile*>(parameters[2].data)->AddBehaviourData(data2);

	auto timer = [startTime, dataIndexTimer, dataIndexRepeat](Projectile* p, float dt) -> bool
	{
		int& timesToRepeat = p->GetBehaviourData(dataIndexRepeat).i;

		if (timesToRepeat <= 0)
			return false;

		float& timer = p->GetBehaviourData(dataIndexTimer).f;

		if (timer > 0.0f)
		{
			timer -= dt;

			return true;
		}
		else
		{
			if (timesToRepeat > 0)
			{
				timer = startTime;
				timesToRepeat--;
			}

			return false;
		}
	};

	ProfileReturnConst(timer);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
TargetPlayerBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	StartProfile;
	PlayerUnit* player = ptrs.player;
	float rotPerSecond = std::get<float>(parameters[0].data);

	auto targeter = [rotPerSecond, player](Projectile* p, float dt) -> bool
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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
LineOfSightConditionBehaviour(std::vector<BehaviourParameter> parameters)
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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::KillSelfBehaviour(
	std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto Suicide = [](Projectile* p, float dt) -> bool
	{
		p->SetActive(false);
		return false;
	};

	ProfileReturnConst(Suicide);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
CollidedConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto Collided = [](Projectile* p, float dt) -> bool
	{
		return (p->GetCollisionType() != CollisionType::NONE);
	};

	ProfileReturnConst(Collided);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
IsAliveConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto IsAlive = [](Projectile* p, float dt) -> bool
	{
		return p->GetActive();
	};

	ProfileReturnConst(IsAlive);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
CloseToEnemyConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	float minDistance = std::get<float>(parameters[0].data);
	Room* currentRoom = *ptrs.currentRoom;
	PlayerUnit* player = ptrs.player;

	auto Close = [minDistance, currentRoom, player](Projectile* p, float dt) -> bool
	{
		float xTarget, yTarget;

		if (p->GetValidTarget() == ValidTarget::ENEMIES || p->GetValidTarget() == ValidTarget::EVERYONE)
		{
			currentRoom->GetClosestEnemy(p->GetXPosition(), p->GetYPosition(), xTarget, yTarget);
		}
		else
		{
			xTarget = player->GetXPosition();
			yTarget = player->GetYPosition();
		}

		float distance = sqrt(
			(xTarget - p->GetXPosition()) * (xTarget - p->GetXPosition()) + (yTarget - p->GetYPosition()) * (yTarget - p->
				GetYPosition()));

		return distance < minDistance;
	};

	ProfileReturnConst(Close);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
CloseToPlayerConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	float minDistance = std::get<float>(parameters[0].data);
	SE::Gameplay::PlayerUnit* player = ptrs.player;

	auto Suicide = [minDistance, player](Projectile* p, float dt) -> bool
	{
		float distance = sqrt(
			(player->GetXPosition() - p->GetXPosition()) * (player->GetXPosition() - p->GetXPosition()) + (player->GetYPosition()
				- p->GetYPosition()) * (player->GetYPosition() - p->GetYPosition()));

		return distance < minDistance;
	};

	ProfileReturnConst(Suicide);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
CreateProjectilesBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	Utilz::GUID fileName = std::get<std::string>(parameters[0].data);

	auto ProjectileCreator = [this, fileName](Projectile* p, float dt) -> bool
	{
		ProjectileData temp;

		temp.startRotation = CoreInit::managers.transformManager->GetRotation(p->GetEntity()).y;
		temp.startPosX = p->GetXPosition();
		temp.startPosY = p->GetYPosition();
		temp.target = p->GetValidTarget();
		temp.eventDamage = p->GetDamageEvent();
		temp.eventDamage.amount = temp.eventDamage.originalAmount;
		temp.ownerUnit = p->GetSharedPtr();
		temp.fileNameGuid = fileName;

		this->CreateNewProjectile(temp);

		return false;
	};


	ProfileReturnConst(ProjectileCreator);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::InverterBehaviour(
	std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> condition = std::get<std::vector<std::function<bool
		(SE::Gameplay::Projectile* projectile, float dt)>>>(parameters[0].data)[0];

	auto Close = [condition](Projectile* p, float dt) -> bool
	{
		return !condition(p, dt);
	};

	ProfileReturnConst(Close);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
KillGenerationBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	auto pManager = ptrs.pManager;

	auto KillGeneration = [pManager](Projectile* p, float dt) mutable -> bool
	{
		auto& projectiles = pManager->GetAllProjectiles();
		for (auto& projectile : projectiles)
		{
			if (projectile.GetGeneration() == p->GetGeneration())
				projectile.SetActive(false);
		}
		return false;
	};

	ProfileReturnConst(KillGeneration);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::SetAliveBehaviour(
	std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	auto SetAlive = [](Projectile* p, float dt) mutable -> bool
	{
		p->SetActive(true);
		return false;
	};

	ProfileReturnConst(SetAlive);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
CollidedWithEnemyConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	//StartProfile;
	auto CollisionCheck = [](Projectile* p, float dt) -> bool
	{
		if ((p->GetCollisionType() == CollisionType::ENEMY && p->GetValidTarget() == ValidTarget::ENEMIES) || (p->GetCollisionType() == CollisionType::PLAYER && p->GetValidTarget() == ValidTarget::PLAYER))
			return true;

		return false;
	};

	return(CollisionCheck);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
CollidedWithObjectConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	//StartProfile;
	auto CollisionCheck = [](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() == CollisionType::OBJECT)
			return true;
		return false;
	};

	return(CollisionCheck);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
CollidedWithPlayerConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	//StartProfile;
	auto CollisionCheck = [](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() == CollisionType::PLAYER)
			return true;
		return false;
	};

	return (CollisionCheck);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
SetDamageBasedOnDTBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto DamageDTModifier = [](Projectile* p, float dt) -> bool
	{
		SE::Gameplay::DamageEvent temp = p->GetDamageEvent();
		temp.amount = dt * temp.originalAmount;
		p->SetDamageEvent(temp);
		p->EmptyHitVector();

		return false;
	};

	ProfileReturnConst(DamageDTModifier);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
UserHealthAboveConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	//StartProfile;
	float minHealth = std::get<float>(parameters[0].data);
	std::weak_ptr<GameUnit*> owner = std::get<std::weak_ptr<GameUnit*>>(parameters[1].data);

	auto AboveHealth = [minHealth, owner](Projectile* p, float dt) -> bool
	{
		if (auto target = owner.lock())
		{
			auto ownerPtr = *target.get();

			if (ownerPtr->GetHealth() >= minHealth)
				return true;
		}

		return false;
	};

	return (AboveHealth);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
SetActualDamageBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;

	float toSetTo = std::get<float>(parameters[0].data);

	auto DamageSetter = [toSetTo](Projectile* p, float dt) -> bool
	{
		SE::Gameplay::DamageEvent temp = p->GetDamageEvent();
		temp.amount = toSetTo;
		p->SetDamageEvent(temp);

		return false;
	};

	ProfileReturnConst(DamageSetter);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::KnockbackBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float force = std::get<float>(parameters[0].data);
	auto Knockback = [force](Projectile* p, float dt) -> bool
	{
		const auto& vec = p->GetUnitsHit();
		
		for (int i = 0; i < vec.size(); i++)
		{
			float xForce = vec[i]->GetXPosition() - p->GetXPosition();
			float yForce = vec[i]->GetYPosition() - p->GetYPosition();

			float totVec = sqrt((xForce * xForce) + (yForce * yForce));
			xForce /= totVec;
			yForce /= totVec;

			vec[i]->AddForce(xForce * force, yForce * force);
		}

		return false;
	};

	ProfileReturnConst(Knockback);
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
			ConditionEvent::ConditionType conditionType;
			conditionType.unionType = 0;
			conditionType.condition.boon = Boons::CONDITIONAL_BOONS_STUN;
			ConditionEvent cEvent(conditionType, 0.f, duration);
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
				auto& unit = *owner.get();
				HealingEvent hEvent;
				hEvent.amount = p->GetProjectileDamageEvent().amount * percent;
				hEvent.type = HealingEvent::SourceType::SOURCE_TYPE_ENEMY_HIT;
				unit->AddHealingEvent(hEvent);
			}

		return false;
	};

	ProfileReturnConst(LifeSteal);
}

std::function<bool(SE::Gameplay::Projectile * projectile, float dt)> SE::Gameplay::ProjectileFactory::
CreateParticlesBetweenProjectileAndOwnerBehaviour(std::vector<BehaviourParameter> parameters)
{
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[0].data);
	auto CreateParticles = [ownerPtr](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			float ownerPos[3];
			auto pos = CoreInit::managers.transformManager->GetPosition(unit->GetEntity());
			ownerPos[0] = pos.x;
			ownerPos[1] = pos.y+1.f;
			ownerPos[2] = pos.z;
			CoreInit::managers.particleSystemManager->UpdateSystemEndPosition(p->GetEntity(), ownerPos);
		}
		return false;
	};

	return (CreateParticles);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::RangeToOwnerConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[0].data);
	float maxDistance = std::get<float>(parameters[1].data);
	auto RangeToOwnerCondition = [ownerPtr, maxDistance](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			float distance[2];
			distance[0] = p->GetXPosition() - unit->GetXPosition();
			distance[1] = p->GetYPosition() - unit->GetYPosition();

			return sqrtf(distance[0] * distance[0] + distance[1] * distance[1]) < maxDistance;

		}
		return false;
	};

	ProfileReturnConst(RangeToOwnerCondition);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::OwnerIsAliveConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[0].data);
	auto OwnerAliveCondition = [ownerPtr](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			return unit->GetHealth() > 0.f;

		}
		return false;
	};

	ProfileReturnConst(OwnerAliveCondition);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetOriginalDamageBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float damageAmount = std::get<float>(parameters[0].data);
	auto SetOriginalDamage = [damageAmount](Projectile* p, float dt) -> bool
	{
		SE::Gameplay::DamageEvent temp = p->GetDamageEvent();
		temp.amount = temp.originalAmount = damageAmount;
		p->SetDamageEvent(temp);

		return false;
	};

	ProfileReturnConst(SetOriginalDamage);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetOriginalHealingBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float healingAmount = std::get<float>(parameters[0].data);
	auto SetOriginalHealing = [healingAmount](Projectile* p, float dt) -> bool
	{
		SE::Gameplay::HealingEvent temp = p->GetHealingEvent();
		temp.amount = temp.originalAmount = healingAmount;
		p->SetHealingEvent(temp);

		return false;
	};

	ProfileReturnConst(SetOriginalHealing);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetValidTargetBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	char target = std::get<std::string>(parameters[0].data)[0];
	auto SetTarget = [target](Projectile* p, float dt) -> bool
	{
		if (target == 'p')
		{
			p->SetValidTarget(ValidTarget::PLAYER);
		}
		else if (target == 'e')
		{
			p->SetValidTarget(ValidTarget::ENEMIES);
		}
		else
		{
			p->SetValidTarget(ValidTarget::EVERYONE);
		}

		return false;
	};

	ProfileReturnConst(SetTarget);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::CreateDecalBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	Core::DecalCreateInfo decalInfo;
	decalInfo.textureName = std::get<std::string>(parameters[0].data);
	decalInfo.opacity = std::get<float>(parameters[1].data);

	float parentScale = 1.0f / CoreInit::managers.transformManager->GetScale(std::get<Projectile*>(parameters[2].data)->GetEntity()).x;
	float height = CoreInit::managers.transformManager->GetPosition(std::get<Projectile*>(parameters[2].data)->GetEntity()).y;

	DirectX::XMFLOAT4X4 tempMat;
	DirectX::XMStoreFloat4x4(&tempMat, DirectX::XMMatrixScaling(parentScale, parentScale, parentScale * 1.1f) * DirectX::XMMatrixRotationX(1.57079632679) * DirectX::XMMatrixTranslation(0.0f, -height, 0.0f));

	CoreInit::managers.decalManager->Create(std::get<Projectile*>(parameters[2].data)->GetEntity(), decalInfo);
	CoreInit::managers.decalManager->ToggleVisible(std::get<Projectile*>(parameters[2].data)->GetEntity(), true);
	//CoreInit::managers.transformManager->SetRotation(std::get<Projectile*>(parameters[2].data)->GetEntity(), 1.57079632679, 0.0f, 0.0f);
	//CoreInit::managers.transformManager->SetScale(std::get<Projectile*>(parameters[2].data)->GetEntity(), { 3.0f, 3.0f, 3.0f });
	CoreInit::managers.decalManager->SetLocalTransform(std::get<Projectile*>(parameters[2].data)->GetEntity(), (float*)&tempMat);

	auto Decal = [](Projectile* p, float dt) -> bool
	{
		return false;
	};

	ProfileReturnConst(Decal);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetDecalOpacityBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float amount = std::get<float>(parameters[0].data);

	auto OpacitySetter = [amount](Projectile* p, float dt) -> bool
	{
		CoreInit::managers.decalManager->SetOpacity(p->GetEntity(), amount);
		return false;
	};

	ProfileReturnConst(OpacitySetter);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::ModifyDecalOpacityBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float amount = std::get<float>(parameters[0].data);

	auto OpacityModifier = [amount](Projectile* p, float dt) -> bool
	{
		CoreInit::managers.decalManager->ModifyOpacity(p->GetEntity(), amount * dt);
		return false;
	};

	ProfileReturnConst(OpacityModifier);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::HealOwnerStaticBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float amount = std::get<float>(parameters[0].data);
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[1].data);

	auto staticHeal = [amount, ownerPtr](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			SE::Gameplay::HealingEvent hEvent;
			hEvent.amount = amount;
			hEvent.originalAmount = hEvent.amount;
			hEvent.type = SE::Gameplay::HealingEvent::SourceType::SOURCE_TYPE_ENEMY_HIT; // wild assumption

			unit->AddHealingEvent(hEvent);
		}

		return false;
	};

	ProfileReturnConst(staticHeal);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::HealOwnerDynamicBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float amount = std::get<float>(parameters[0].data);
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[1].data);

	auto dynamicHeal = [amount, ownerPtr](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			SE::Gameplay::HealingEvent hEvent;
			hEvent.amount = amount * dt;
			hEvent.originalAmount = hEvent.amount;
			hEvent.type = SE::Gameplay::HealingEvent::SourceType::SOURCE_TYPE_ENEMY_HIT; // wild assumption

			unit->AddHealingEvent(hEvent);
		}

		return false;
	};

	ProfileReturnConst(dynamicHeal);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::HealOwnerPercentageStaticBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float percentage = std::get<float>(parameters[0].data);
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[1].data);

	auto staticPercentageHeal = [percentage, ownerPtr](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			SE::Gameplay::HealingEvent hEvent;
			hEvent.amount = percentage * unit->GetMaxHealth();
			hEvent.originalAmount = hEvent.amount;
			hEvent.type = SE::Gameplay::HealingEvent::SourceType::SOURCE_TYPE_ENEMY_HIT; // wild assumption

			unit->AddHealingEvent(hEvent);
		}

		return false;
	};

	ProfileReturnConst(staticPercentageHeal);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::HealOwnerPercentageDynamicBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float percentage = std::get<float>(parameters[0].data) / 100;
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[1].data);

	auto dynamicPercentageHeal = [percentage, ownerPtr](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			SE::Gameplay::HealingEvent hEvent;
			hEvent.amount = percentage * unit->GetMaxHealth() * dt;
			hEvent.originalAmount = hEvent.amount;
			hEvent.type = SE::Gameplay::HealingEvent::SourceType::SOURCE_TYPE_ENEMY_HIT; // wild assumption

			unit->AddHealingEvent(hEvent);
		}

		return false;
	};

	ProfileReturnConst(dynamicPercentageHeal);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetOwnerHealthStaticBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float amount = std::get<float>(parameters[0].data);
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[1].data);

	auto healthSetStatic = [amount, ownerPtr](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			unit->SetHealth(amount);
		}

		return false;
	};

	ProfileReturnConst(healthSetStatic);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetOwnerHealthPercentBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float amount = std::get<float>(parameters[0].data);
	std::weak_ptr<GameUnit*> ownerPtr = std::get<std::weak_ptr<GameUnit*>>(parameters[1].data);

	auto healthSetPercentage = [amount, ownerPtr](Projectile* p, float dt) -> bool
	{
		if (auto owner = ownerPtr.lock())
		{
			auto unit = *owner.get();
			unit->SetHealth(amount * unit->GetMaxHealth());
		}

		return false;
	};

	ProfileReturnConst(healthSetPercentage);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetPositionToMouseBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	auto window = CoreInit::subSystems.window;

	auto setToMousePos = [window](Projectile* p, float dt) -> bool
	{
		int mouseX, mouseY;
		window->GetMousePos(mouseX, mouseY);

		DirectX::XMVECTOR rayO = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMVECTOR rayD;

		auto width = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "width", 1280);
		auto height = CoreInit::subSystems.optionsHandler->GetOptionInt("Window", "height", 720);
		CoreInit::managers.cameraManager->WorldSpaceRayFromScreenPos(mouseX, mouseY, width, height, rayO, rayD);

		float distance = DirectX::XMVectorGetY(rayO) / -DirectX::XMVectorGetY(rayD);

		auto clickPos = rayO + rayD * distance;

		p->PositionEntity(DirectX::XMVectorGetX(clickPos), DirectX::XMVectorGetZ(clickPos));

		return false;
	};

	ProfileReturnConst(setToMousePos);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::MinimumLifeTimeConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float minimumLife = std::get<float>(parameters[0].data);

	auto lifeTimeChecker = [minimumLife](Projectile* p, float dt) -> bool
	{
		return minimumLife <= p->GetLifeTime();
	};

	ProfileReturnConst(lifeTimeChecker);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetHeightBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float newHeight = std::get<float>(parameters[0].data);

	auto heightSetter = [newHeight](Projectile* p, float dt) -> bool
	{
		p->SetExtentY(newHeight);

		return false;
	};

	ProfileReturnConst(heightSetter);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::SetWidthBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float newWidth = std::get<float>(parameters[0].data);

	auto widthSetter = [newWidth](Projectile* p, float dt) -> bool
	{
		p->SetExtentX(newWidth);

		return false;
	};

	ProfileReturnConst(widthSetter);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::TargetOwnerBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float rotPerSecond = std::get<float>(parameters[0].data);

	auto targeter = [rotPerSecond](Projectile* p, float dt) -> bool
	{
		if (auto target = p->GetOwnerPtr().lock())
		{
			auto ownerPtr = *target.get();
			float xTarget = ownerPtr->GetXPosition(), yTarget = ownerPtr->GetYPosition();

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

		return false;
	};

	ProfileReturnConst(targeter);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::CollidedWithOwnerConditionBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile
	auto CollisionCheck = [](Projectile* p, float dt) -> bool
	{
		if (auto owner = p->GetOwnerPtr().lock())
		{
			auto unit = *owner.get();

			for (int i = 0; i < p->GetUnitsHit().size(); i++)
			{
				if (unit == p->GetUnitsHit()[i])
					return true;
			}
		}
		return false;
	};
	ProfileReturnConst(CollisionCheck);
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::LockToOwnerBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	PlayerUnit* player = ptrs.player;

	auto LockToOwner = [player](Projectile* p, float dt) -> bool
	{
		if (auto owner = p->GetOwnerPtr().lock())
		{
			auto unit = *owner.get();
			p->PositionEntity(unit->GetXPosition(), unit->GetYPosition());

		}

		return false;
	};

	ProfileReturnConst(LockToOwner);
}

std::function<bool(SE::Gameplay::Projectile*projectile, float dt)> SE::Gameplay::ProjectileFactory::IncreaseExtentsDynamicallyBehaviour(std::vector<BehaviourParameter> parameters)
{
	StartProfile;
	float toAdd = std::get<float>(parameters[0].data);

	auto extentIncreaser = [toAdd](Projectile* p, float dt) -> bool
	{
		float newWidth = p->GetExtentX() + toAdd * dt;
		float newHeight = p->GetExtentY() + toAdd * dt;

		p->SetExtentX(newWidth);
		p->SetExtentY(newHeight);

		return false;
	};

	ProfileReturnConst(extentIncreaser);
}

SE::Gameplay::ProjectileFactory::ProjectileFactory()
{
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::BounceBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SpeedAddDynamicBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::RotationModifierBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::RotationInvertionBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LifeTimeAddStaticBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::TargetClosestEnemyBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::TimeConditionBehaviour, this, std::placeholders::_1)); // f, i, p
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
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetDamageBasedOnDTBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::UserHealthAboveConditionBehaviour, this, std::placeholders::_1)); // f, o
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetActualDamageBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CreateParticlesBetweenProjectileAndOwnerBehaviour, this, std::placeholders::_1)); // o
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::KnockbackBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::RangeToOwnerConditionBehaviour, this, std::placeholders::_1)); // o, f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::OwnerIsAliveConditionBehaviour, this, std::placeholders::_1)); // o, f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetOriginalDamageBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetOriginalHealingBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetValidTargetBehaviour, this, std::placeholders::_1)); // s
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::TargetPlayerBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CreateDecalBehaviour, this, std::placeholders::_1)); // s, f, p
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetDecalOpacityBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::ModifyDecalOpacityBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::HealOwnerStaticBehaviour, this, std::placeholders::_1)); // f, o
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::HealOwnerDynamicBehaviour, this, std::placeholders::_1)); // f, o
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::HealOwnerPercentageStaticBehaviour, this, std::placeholders::_1)); // f, o
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::HealOwnerPercentageDynamicBehaviour, this, std::placeholders::_1)); // f, o
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetOwnerHealthStaticBehaviour, this, std::placeholders::_1)); // f, o
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetOwnerHealthPercentBehaviour, this, std::placeholders::_1)); // f, o
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetPositionToMouseBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::MinimumLifeTimeConditionBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetHeightBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::SetWidthBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::TargetOwnerBehaviour, this, std::placeholders::_1)); // f
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::CollidedWithOwnerConditionBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::LockToOwnerBehaviour, this, std::placeholders::_1)); // 
	behaviourFunctions.push_back(std::bind(&ProjectileFactory::IncreaseExtentsDynamicallyBehaviour, this, std::placeholders::_1)); // f

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
