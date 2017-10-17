#include <Gameplay\ProjectileFactory.h>
#include <Profiler.h>
#include <Core\CollisionManager.h>
#include "Core/Engine.h"
#include "EnemyUnit.h"

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

	std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters;
	parameters.resize(3);


	AddBehaviourToProjectile(temp, TypeOfFunction::ON_COLLISION, BounceBehaviour(parameters));
	//AddRotationInvertion(temp, TypeOfFunction::CONTINUOUS, 0.25f);
	//AddRotationModifier(temp, TypeOfFunction::CONTINUOUS, 0.2f);
	//AddLifeTime(temp, TypeOfFunction::ON_DEATH, 6.0f);
	//AddRotationModifier(temp, TypeOfFunction::ON_DEATH, -0.4f);

	parameters[0].behaviour.f = 60.0f * 0.08f;
	AddBehaviourToProjectile(temp, TypeOfFunction::CONTINUOUS, TargetClosestEnemyBehaviour(parameters));


	ProfileReturnConst(temp);

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
	float speedModifier = parameters[0].behaviour.f;

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
	float rotationModifier = parameters[0].behaviour.f;

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
	float intervall = parameters[0].behaviour.f;
	data.f = intervall;

	int dataIndex = parameters[1].behaviour.projectile->AddBehaviourData(data);

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

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::
StunOwnerUnitBehaviour(std::vector<BehaviourParameter> parameters)
{
	std::weak_ptr<GameUnit*> ownerPtr = *parameters[0].behaviour.projectileOwner;
	auto StunOwnerUnit = [ownerPtr](Projectile* p, float dt) -> bool
	{
		if(p->GetCollisionType() == CollisionType::ENEMY)
			if(auto owner = ownerPtr.lock())
			{
				auto unit = *owner.get();
				ConditionEvent cEvent;
				cEvent.type = ConditionEvent::ConditionTypes::CONDITION_TYPE_STUN;
				unit->AddConditionEvent(cEvent);
			}

		return true;
	};

	return StunOwnerUnit;
}

std::function<bool(SE::Gameplay::Projectile* projectile, float dt)> SE::Gameplay::ProjectileFactory::TargetClosestEnemyBehaviour(std::vector<SE::Gameplay::ProjectileFactory::BehaviourParameter> parameters)
{
	Room* currentRoom = *ptrs.currentRoom;
	float rotPerSecond = parameters[0].behaviour.f;

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

