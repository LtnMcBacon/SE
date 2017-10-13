#include <Gameplay\ProjectileFactory.h>
#include <Profiler.h>
#include <Core\CollisionManager.h>
#include "Core/Engine.h"

SE::Gameplay::Projectile SE::Gameplay::ProjectileFactory::CreateNewProjectile(ProjectileData data)
{
	StartProfile;
	Rotation rot;
	rot.force = 0.04f;

	SE::Gameplay::Projectile temp(data, rot, 6.0f, 3.0f, SE::Gameplay::ValidTarget::ENEMIES, data.eventDamage, data.eventHealing, data.eventCondition);

	auto& e = Core::Engine::GetInstance();
	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();

	tm.SetPosition(temp.GetEntity(), DirectX::XMFLOAT3(data.startPosX, 0.5f, data.startPosY));
	tm.SetRotation(temp.GetEntity(), 0.0f, data.startRotation, 0.0f);
	tm.SetScale(temp.GetEntity(), DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f));

	rm.CreateRenderableObject(temp.GetEntity(), Utilz::GUID("Placeholder_Block.mesh"));
	rm.ToggleRenderableObject(temp.GetEntity(), true);

	AddBounce(temp, TypeOfFunction::ON_COLLISION);
	//AddRotationInvertion(temp, TypeOfFunction::CONTINUOUS, 0.25f);

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
		p->UpdateMovement(dt);
		p->UpdateMovement(dt);
		return false;
	};

	AddBehaviourToProjectile(projectile, type, bounce);
}

void SE::Gameplay::ProjectileFactory::AddRotationInvertion(Projectile & projectile, TypeOfFunction type, float intervall)
{
	auto inverter = [intervall](Projectile* p, float dt) -> bool
	{
		static float timer = intervall;

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

