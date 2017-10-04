#include <Gameplay\ProjectileFactory.h>
#include <Profiler.h>
#include <Core\CollisionManager.h>
#include "Core/Engine.h"

SE::Gameplay::Projectile SE::Gameplay::ProjectileFactory::CreateNewProjectile(ProjectileData data)
{
	Rotation rot;

	SE::Gameplay::Projectile temp(data, rot, 2.0f, 10.0f, SE::Gameplay::ValidTarget::ENEMIES, data.eventDamage, data.eventHealing, data.eventCondition);
	/*projectile.SetXPosition(data.startPosX);
	projectile.SetYPosition(data.startPosY);
	projectile.SetRotation(data.startRotation);
	projectile.SetRotationStyle(rot);
	projectile.SetValidTarget(SE::Gameplay::ValidTarget::ENEMIES);
	projectile.SetDamageEvent(data.eventDamage);
	projectile.SetHealingEvent(data.eventHealing);
	projectile.SetConditionEvent(data.eventCondition);*/

	auto& e = Core::Engine::GetInstance();

	auto& em = e.GetEntityManager();
	auto& rm = e.GetRenderableManager();
	auto& tm = e.GetTransformManager();

	tm.Create(temp.GetEntity());
	tm.SetPosition(temp.GetEntity(), DirectX::XMFLOAT3(data.startPosX, 0.5f, data.startPosY));
	tm.SetScale(temp.GetEntity(), DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f));

	Utilz::GUID testGuid = Utilz::GUID("pCube1_Placeholder_Block.mesh");

	rm.CreateRenderableObject(temp.GetEntity(), Utilz::GUID("pCube1_Placeholder_Block.mesh"));
	rm.ToggleRenderableObject(temp.GetEntity(), true);

	return temp;

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

