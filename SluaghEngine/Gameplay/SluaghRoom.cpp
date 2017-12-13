#include "SluaghRoom.h"
#include <Profiler.h>
#include "CoreInit.h"

SE::Gameplay::SluaghRoom::SluaghRoom(const Utilz::GUID& fileName, PlayerUnit* thePlayer, ProjectileManager* projectileManagers)
	:Room(fileName), projectileManagers(projectileManagers)
{
	theSluagh = new Sluagh(thePlayer, this);
	for(int i = 0; i < 4; i++)
	{
		DoorArr[i].active = false;
	}

	const int barWidth = 47;
	const int barHeight = 12;

	const int frameWidth = 51;
	const int frameHeight = 14;

	Core::Entity tempBar = CoreInit::managers.entityManager->Create();
	Core::IGUIManager::CreateInfo barInfo;
	barInfo.texture = "Enemy hpBar.jpg";
	barInfo.textureInfo.height = barHeight;
	barInfo.textureInfo.width = barWidth;
	barInfo.textureInfo.anchor = { 0.0f, 0.0f };
	barInfo.textureInfo.layerDepth = 0.01 * hpBars.size() + 0.01;

	CoreInit::managers.guiManager->Create(tempBar, barInfo);

	Core::Entity tempFrame = CoreInit::managers.entityManager->Create();
	Core::IGUIManager::CreateInfo frameInfo;
	frameInfo.texture = "Enemy hpBar Frame.jpg";
	frameInfo.textureInfo.height = frameHeight;
	frameInfo.textureInfo.width = frameWidth;
	frameInfo.textureInfo.anchor = { 0.0f, 0.0f };
	frameInfo.textureInfo.layerDepth = 0.011 * hpBars.size() + 0.011;

	CoreInit::managers.guiManager->Create(tempFrame, frameInfo);

	HpBar tempHpBar;
	tempHpBar.bar = tempBar;
	tempHpBar.frame = tempFrame;
	hpBars.push_back(tempHpBar);
}

SE::Gameplay::SluaghRoom::~SluaghRoom()
{

}

void SE::Gameplay::SluaghRoom::Update(float dt, float playerX, float playerY)
{
	StartProfile;
	if (theSluagh->GetSluagh()->IsAlive() && theSluagh->GetSluagh()->GetHealth() > 0.f)
	{
		UpdateHpBars(playerX, playerY);
		theSluagh->ToggleRendering(true);
		std::vector<ProjectileData> projectiles;
		theSluagh->Update(dt, projectiles);

		projectileManagers->AddProjectiles(projectiles);
	}
	StopProfile;
}

void SE::Gameplay::SluaghRoom::InitSluagh()
{
	StartProfile;
	theSluagh->InitializeSluagh();
	theSluagh->ToggleRendering(false);
	StopProfile;
}

bool SE::Gameplay::SluaghRoom::GetClosestEnemy(float xPos, float yPos, float & xReturn, float & yReturn)
{
	xReturn = theSluagh->GetSluagh()->GetXPosition();
	yReturn = theSluagh->GetSluagh()->GetYPosition();
	return true;
}

bool SE::Gameplay::SluaghRoom::ProjectileAgainstEnemies(Projectile& projectile)
{
	StartProfile;
	
	CollisionData cData; 
	if (theSluagh->CollisionAgainstProjectile(projectile.GetXPosition(), projectile.GetYPosition(), projectile.GetBoundingRect().radius))
	{
		if (projectile.GetValidTarget() == ValidTarget::PLAYER)
			ProfileReturnConst(false);
		auto sluagh = theSluagh->GetSluagh();
		sluagh->AddDamageEvent(projectile.GetProjectileDamageEvent());
		sluagh->AddHealingEvent(projectile.GetProjectileHealingEvent());
		sluagh->AddConditionEvent(projectile.GetProjectileConditionEvent());
		cData.hitUnit = sluagh->GetSharedPtr();
		cData.type = CollisionType::ENEMY;
	}

	if (cData.type != CollisionType::NONE)
	{
		projectile.SetCollisionData(cData);
		ProfileReturnConst(true);
	}

	ProfileReturnConst(false);
}

void SE::Gameplay::SluaghRoom::UpdateHpBars(float playerX, float playerY)
{
	StartProfile;

	const int barWidth = 47;
	const int barHeight = 12;

	const int frameWidth = 51;
	const int frameHeight = 14;

	CoreInit::managers.guiManager->ToggleRenderableTexture(hpBars[0].bar, true);
	CoreInit::managers.guiManager->ToggleRenderableTexture(hpBars[0].frame, true);

	float xPos, yPos;
	auto a = CoreInit::managers.cameraManager->GetActive();

	DirectX::XMFLOAT3 worldPos = CoreInit::managers.transformManager->GetPosition(theSluagh->GetSluagh()->GetEntity());
	DirectX::XMVECTOR worldVec = DirectX::XMVectorSet(worldPos.x, worldPos.y, worldPos.z, 1.0f);
	DirectX::XMMATRIX viewProj = DirectX::XMLoadFloat4x4(&CoreInit::managers.cameraManager->GetViewProjection(CoreInit::managers.cameraManager->GetActive()));
	DirectX::XMVECTOR screenPos = DirectX::XMVector4Transform(worldVec, viewProj);

	float w = DirectX::XMVectorGetW(screenPos);

	xPos = (int)round(((DirectX::XMVectorGetX(screenPos) / w + 1) / 2.0f) * CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Window", "Width", 1280));
	yPos = (int)round(((1 - DirectX::XMVectorGetY(screenPos) / w) / 2.0f) * CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Window", "Height", 720));

	CoreInit::managers.guiManager->SetTexturePos(hpBars[0].bar, xPos - 50, yPos - 50);
	CoreInit::managers.guiManager->SetTexturePos(hpBars[0].frame, xPos - 50, yPos - 50);

	CoreInit::managers.guiManager->SetTextureDimensions(hpBars[0].bar, barWidth * (theSluagh->GetSluagh()->GetHealth() / theSluagh->GetSluagh()->GetMaxHealth()), barHeight);
	

	StopProfile;
}
