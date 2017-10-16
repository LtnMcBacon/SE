#include <Gameplay\ProjectileFactory.h>
#include <Profiler.h>
#include <Core\CollisionManager.h>
#include "Core/Engine.h"

SE::Gameplay::Projectile SE::Gameplay::ProjectileFactory::CreateNewProjectile(ProjectileData data)
{
	StartProfile;
	Rotation rot;
	rot.force = 0.0f;

	SE::Gameplay::Projectile temp(data, rot, 6.0f, 5.0f, SE::Gameplay::ValidTarget::ENEMIES, data.eventDamage, data.eventHealing, data.eventCondition);

	auto& e = Core::Engine::GetInstance();
	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();

	tm.SetPosition(temp.GetEntity(), DirectX::XMFLOAT3(data.startPosX, 0.5f, data.startPosY));
	tm.SetRotation(temp.GetEntity(), 0.0f, data.startRotation, 0.0f);
	tm.SetScale(temp.GetEntity(), DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f));

	rm.CreateRenderableObject(temp.GetEntity(), Utilz::GUID("Placeholder_Block.mesh"));
	rm.ToggleRenderableObject(temp.GetEntity(), true);

	//AddBounce(temp, TypeOfFunction::ON_COLLISION);
	//AddRotationInvertion(temp, TypeOfFunction::CONTINUOUS, 0.25f);
	//AddRotationModifier(temp, TypeOfFunction::CONTINUOUS, 0.2f);
	//AddLifeTime(temp, TypeOfFunction::ON_DEATH, 6.0f);
	//AddRotationModifier(temp, TypeOfFunction::ON_DEATH, -0.4f);
	AddTargeting(temp, TypeOfFunction::CONTINUOUS, 60.0f * 0.08f);

	ProfileReturnConst(temp);

}

void SE::Gameplay::ProjectileFactory::AddBounce(Projectile& projectile, TypeOfFunction type)
{
	auto bounce = [](Projectile* p, float dt) -> bool
	{
		if (p->GetCollisionType() != CollisionType::OBJECT)
			return false;

		DirectX::XMFLOAT3 currentDir = Core::Engine::GetInstance().GetTransformManager().GetForward(p->GetEntity());
		DirectX::XMVECTOR newDir = DirectX::XMVector3Reflect({ currentDir.x, currentDir.y, currentDir.z, 0.0f }, { p->GetCollisionVectorX(), 0.0f, p->GetCollisionVectorY(), 0.0f });
		Core::Engine::GetInstance().GetTransformManager().SetForward(p->GetEntity(), newDir);
		p->SetActive(true);
		//p->UpdateMovement(dt);
		//p->UpdateMovement(dt);
		return false;
	};

	AddBehaviourToProjectile(projectile, type, bounce);
}

void SE::Gameplay::ProjectileFactory::AddSpeedModifier(Projectile & projectile, TypeOfFunction type, float speedModifier)
{
	auto speed = [speedModifier](Projectile* p, float dt) -> bool
	{
		float speed = p->GetSpeed();
		speed += speed * dt * speedModifier;
		p->SetSpeed(speed);

		return true;
	};

	AddBehaviourToProjectile(projectile, type, speed);
}

void SE::Gameplay::ProjectileFactory::AddRotationModifier(Projectile & projectile, TypeOfFunction type, float rotationModifier)
{
	auto rotation = [rotationModifier](Projectile* p, float dt) -> bool
	{
		Rotation rotation = p->GetRotationStyle();
		rotation.force += rotation.force * dt * rotationModifier;
		p->SetRotationStyle(rotation);

		return true;
	};

	AddBehaviourToProjectile(projectile, type, rotation);
}

void SE::Gameplay::ProjectileFactory::AddRotationInvertion(Projectile & projectile, TypeOfFunction type, float intervall)
{
	BehaviourData data;
	data.f = intervall;

	int dataIndex = projectile.AddBehaviourData(data);

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

	AddBehaviourToProjectile(projectile, type, inverter);
}

void SE::Gameplay::ProjectileFactory::AddLifeTime(Projectile & projectile, TypeOfFunction type, float timeToIncrease)
{

	auto timeIncreaser = [timeToIncrease](Projectile* p, float dt) -> bool
	{
		float lifeTime = p->GetLifeTime();
		lifeTime += timeToIncrease;
		p->SetLifeTime(lifeTime);
		p->SetActive(true);

		return false;
	};

	AddBehaviourToProjectile(projectile, type, timeIncreaser);
}

void SE::Gameplay::ProjectileFactory::AddTargeting(Projectile & projectile, TypeOfFunction type, float rotPerSecond)
{
	Room* currentRoom = *ptrs.currentRoom;

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
			int sign;

			if (totalRot < 0.0f)
				sign = 1.0f;
			else
				sign = -1.0f;

			float rotAmount = sign * rotPerSecond * dt;

			//tm.Rotate(p->GetEntity(), 0.0f, totalRot, 0.0f);

			Rotation test;
			test.force = rotAmount;
			test.style = RotationStyle::SELF;
			p->SetRotationStyle(test);

			/*if (rotAmount > totalRot)
			{
				tm.Rotate(p->GetEntity(), 0.0f, totalRot, 0.0f);
			}
			else
			{
				tm.Rotate(p->GetEntity(), 0.0f, rotAmount, 0.0f);
			}*/

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

	AddBehaviourToProjectile(projectile, type, targeter);
}

void SE::Gameplay::ProjectileFactory::AddBehaviourToProjectile(Projectile & p, TypeOfFunction type, std::function<bool(Projectile*projectile, float dt)> func)
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

}

SE::Gameplay::ProjectileFactory::~ProjectileFactory()
{
	StartProfile;
	/*
	* Code body
	*/
	ProfileReturnVoid;
}

