#include "PlayerUnit.h"
#include <Profiler.h>
#include "Flowfield.h"
#include "ProjectileData.h"
#include "CoreInit.h"

void SE::Gameplay::PlayerUnit::ResolveEvents()
{
	StartProfile;

	// only basic at the moment
	
	for (int i = 0; i < DamageEventVector.size(); i++)
	{
		this->health -= DamageEventVector[i].amount;
	}
	
	//for(auto condition : ConditionEventVector)
	//{
	//	if (condition.isBane == false)
	//	{
	//		switch (condition.boon)
	//		{
	//		case Boons::CONDITIONAL_BOONS_NONE:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_DAMAGE:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_KNOCKBACK:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_STUN:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_ROOT:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_PROTECTION:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_PHYSICAL_RESISTANCE:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_MAGICAL_RESISTANCE:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_FIRE_RESISTANCE:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_WATER_RESISTANCE:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_NATURE_RESISTANCE:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_CASTSPEED:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_SWIFTNESS:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_SLOW:
	//			break;
	//		case Boons::CONDITIONAL_BOONS_INVULNERABILITY:
	//			break;
	//		}
	//	}
	//}

	for(auto healing : HealingEventVector)
	{
		health += healing.amount;
	}
	
	this->health = 100.f;
	ProfileReturnVoid;

}


bool SE::Gameplay::PlayerUnit::CorrectCollision(float dt, float &xMov, float &yMov)
{
	StartProfile;
	bool collision = false;
	float moveTot = abs(xMov) + abs(yMov);
	float xMovementTot = xMov;
	float yMovementTot = yMov;
	if (moveTot != 0.0f)
	{
		xMovementTot /= moveTot;
		yMovementTot /= moveTot;
	}
	xMovementTot *= dt;
	yMovementTot *= dt;


	/*float sampleX = 0.f;
	float sampleY = 0.f;*/

	float localExtent = extents + 0.15;

	pos myPos;
	/*Checking collision in left x-axis*/
	myPos.x = xPos + xMovementTot - localExtent;
	myPos.y = yPos - localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		xMov = 0;
		collision = true;
	}
	myPos.y = yPos + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		xMov = 0;
		collision = true;
	}

	/*Checking collision in right x-axis*/
	myPos.x = xPos + xMovementTot + localExtent;
	myPos.y = yPos - localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		xMov = 0;
		collision = true;
	}
	myPos.y = yPos + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		xMov = 0;
		collision = true;
	}

	/*Checking collision in down y-axis*/
	myPos.x = xPos - localExtent;
	myPos.y = yPos + yMovementTot - localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		yMov = 0;
		collision = true;
	}
	myPos.x = xPos + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		yMov = 0;
		collision = true;
	}

	/*Checking collision in up y-axis*/
	myPos.x = xPos - localExtent;
	myPos.y = yPos + yMovementTot + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		yMov = 0;
		collision = true;
	}
	myPos.x = xPos + localExtent;

	if (map[int(myPos.x)][int(myPos.y)])
	{
		yMov = 0;
		collision = true;
	}

	ProfileReturnConst(collision);
}

void SE::Gameplay::PlayerUnit::UpdatePlayerRotation(float camAngleX, float camAngleY)
{
	this->rotMov[0] = cosf(camAngleX);
	this->rotMov[1] = sinf(camAngleY);
}

void SE::Gameplay::PlayerUnit::UpdateMovement(float dt, const MovementInput & inputs)
{
	StartProfile;
	if(stunDuration > 0)
	{
		stunDuration -= dt;
		if (stunDuration < 0)
			stunDuration = 0.f;
		ProfileReturnVoid;
	}
	dt *= newStat.movementSpeed;
	float xMovement = 0.f;
	float yMovement = 0.f;

	// Handle input and apply movement
	if (inputs.upButton)
		yMovement += 1.0f;
	if (inputs.downButton)
		yMovement -= 1.0f;
	if (inputs.leftButton)
		xMovement -= 1.0f;
	if (inputs.rightButton)
		xMovement += 1.0f;

	float tempX = xMovement;
	float tempY = yMovement;

	xMovement = tempX*rotMov[0] + tempY*rotMov[1];
	yMovement = -tempX*rotMov[1] + tempY*rotMov[0];

	if (inputs.mouseRightDown)
	{
		xMovement = inputs.mousePosX - xPos;
		yMovement = inputs.mousePosY - yPos;
	}
	// Check for collision and update the movement based on it
	CorrectCollision(dt, xMovement, yMovement);

	/*Normalize the movement vector*/
	float moveTot = abs(xMovement) + abs(yMovement);
	if (moveTot != 0.0f)
	{
		xMovement /= moveTot;
		yMovement /= moveTot;
		if (!CoreInit::managers.animationManager->IsAnimationPlaying(unitEntity))
		{
			Core::IAnimationManager::AnimationPlayInfo playInfo;
			playInfo.animations[0] = "TopRunAnim_MCModell.anim";
			playInfo.animationSpeed[0] = 20.0f;
			playInfo.timePos[0] = 0.0f;
			playInfo.looping[0] = true;

			playInfo.animations[1] = "BottomRunAnim_MCModell.anim";
			playInfo.animationSpeed[1] = 20.0f;
			playInfo.timePos[1] = 0.0f;
			playInfo.looping[1] = true;

			playInfo.nrOfLayers = 2;

			CoreInit::managers.animationManager->Start(unitEntity, playInfo);
		}
	}
	else
	{
		CoreInit::managers.animationManager->SetKeyFrame(unitEntity, 0);
	}

	//------------------------

	DirectX::XMFLOAT3 tempRot = CoreInit::managers.transformManager->GetRotation(this->unitEntity);

	DirectX::XMVECTOR defaultVector = { 0.0f, 0.0f, 1.0f, 0.0f };
	DirectX::XMVECTOR mouseVector = {inputs.mousePosX - xPos, 0.0f, inputs.mousePosY - yPos, 0.0f};

	int side;

	if (inputs.mousePosX < xPos)
		side = -1;
	else
		side = 1;

	tempRot.y = side * DirectX::XMVectorGetY(DirectX::XMVector3AngleBetweenVectors(defaultVector, mouseVector));

	CoreInit::managers.transformManager->SetRotation(this->unitEntity, tempRot.x, tempRot.y, tempRot.z);

	//-----------------------

	/*Move the entity in the normalized direction*/
	MoveEntity(xMovement * dt, yMovement * dt);
	StopProfile;
}

void SE::Gameplay::PlayerUnit::UpdateActions(float dt, std::vector<ProjectileData>& newProjectiles, const ActionInput& input)
{
	StartProfile;

	int nrOfSKills = skills.size();
	
	if (nrOfSKills > 0 && skills[0].currentCooldown <= 0.0f && input.skill1Button)
	{
		ProjectileData temp;

		temp.startRotation = CoreInit::managers.transformManager->GetRotation(unitEntity).y;
		temp.startPosX = this->xPos;
		temp.startPosY = this->yPos;
		temp.target = ValidTarget::ENEMIES;
		temp.eventDamage = DamageEvent(skills[0].atkType, skills[0].element, skills[0].skillDamage);
		//temp.healingEvent = skills[0]->GetHealingEvent();
		//temp.conditionEvent = skills[0]->GetConditionEvent();
		temp.ownerUnit = mySelf;
		temp.fileNameGuid = skills[0].projectileFileGUID;

		newProjectiles.push_back(temp);
		skills[0].currentCooldown = skills[0].coolDown;
	}

	if (nrOfSKills > 1 && skills[1].currentCooldown <= 0.0f && input.skill2Button)
	{
		ProjectileData temp;

		temp.startRotation = CoreInit::managers.transformManager->GetRotation(unitEntity).y;
		temp.startPosX = this->xPos;
		temp.startPosY = this->yPos;
		temp.target = ValidTarget::ENEMIES;
		temp.eventDamage = DamageEvent(skills[1].atkType, skills[1].element, skills[1].skillDamage);
		//temp.healingEvent = skills[1]->GetHealingEvent();
		//temp.conditionEvent = skills[1]->GetConditionEvent();
		temp.ownerUnit = mySelf;
		temp.fileNameGuid = skills[1].projectileFileGUID;

		newProjectiles.push_back(temp);
		skills[1].currentCooldown = skills[1].coolDown;
	}

	if (nrOfSKills > 0 && skills[0].currentCooldown > 0.0f)
	{
		skills[0].currentCooldown -= dt;
	}
	if (nrOfSKills > 1 && skills[1].currentCooldown > 0.0f)
	{
		skills[1].currentCooldown -= dt;
	}

	/*if (input.skill1Button) 
	{
		ProjectileData temp;

		temp.startRotation = CoreInit::managers.transformManager->GetRotation(unitEntity).y;
		temp.startPosX = this->xPos;
		temp.startPosY = this->yPos;
		temp.target = ValidTarget::ENEMIES;
		temp.eventDamage = DamageEvent(Gameplay::DamageSources::DAMAGE_SOURCE_RANGED, Gameplay::DamageTypes::DAMAGE_TYPE_PHYSICAL, 2);
		temp.ownerUnit = mySelf;
		temp.fileNameGuid = "turretProjectile.SEP";

		newProjectiles.push_back(temp);
	}*/

	if (input.actionButton && attackCooldown <= 0.0f)
	{
		ProjectileData temp;

		temp.startRotation = CoreInit::managers.transformManager->GetRotation(unitEntity).y;
		temp.startPosX = this->xPos;
		temp.startPosY = this->yPos;
		temp.target = ValidTarget::ENEMIES;
		temp.eventDamage = DamageEvent(DamageSources::DAMAGE_SOURCE_MELEE, DamageTypes::DAMAGE_TYPE_PHYSICAL, 2);
		temp.ownerUnit = mySelf;
		temp.fileNameGuid = "playerMeleeProjectiles.SEP";

		newProjectiles.push_back(temp);

		attackCooldown = 1.0f / attackSpeed;
	}

	if (attackCooldown > 0.f)
	{
		attackCooldown -= dt;
	}
	if (attackCooldown < 0.f)
		attackCooldown = 0.f;
	ResolveEvents();
	ClearConditionEvents();
	ClearDamageEvents();
	ClearHealingEvents();
	StopProfile;

}

void SE::Gameplay::PlayerUnit::AddForce(float force[2])
{
	StartProfile;
	forcesToApply[0] += force[0];
	forcesToApply[1] += force[1];
	StopProfile;
}

void SE::Gameplay::PlayerUnit::UpdateMap(const char** mapForRoom)
{
	StartProfile;
	//map = mapForRoom;
	StopProfile;
}

void SE::Gameplay::PlayerUnit::calcStrChanges()
{
	StartProfile;
	if (baseStat.str > 5)
	{
		int increment = baseStat.str - 5;
		newStat.health = baseStat.health * (1.f + (0.05f * increment));
		newStat.damage = baseStat.damage * (1.f + (0.05f * increment));
	}
	else if (baseStat.str < 5)
	{
		newStat.health = baseStat.health * (1.f - (0.1f * baseStat.str));
		newStat.damage = baseStat.damage * (1.f - (0.1f * baseStat.str));

		if (baseStat.str <= 3)
		{
			newStat.armorCap = 2;
		}
		else if (baseStat.str == 1)
		{
			newStat.armorCap = 1;
		}
	}
	else
	{
		newStat.health = baseStat.health;
		newStat.damage = baseStat.damage;
	}
	StopProfile;
}
void SE::Gameplay::PlayerUnit::calcAgiChanges()
{
	StartProfile;
	if (baseStat.agi > 5)
	{
		int increment = baseStat.agi - 5;
		newStat.rangedDamage = baseStat.rangedDamage * (1.f + (0.05f * increment));
		newStat.movementSpeed = baseStat.movementSpeed * (1.f + (0.05f * increment));
	}
	else if (baseStat.agi < 5)
	{
		newStat.rangedDamage = baseStat.rangedDamage * (1.f - (0.05f * baseStat.agi));
		newStat.movementSpeed = baseStat.movementSpeed * (1.f - (0.05f * baseStat.agi));
	}
	else
	{
		newStat.rangedDamage = baseStat.rangedDamage;
		newStat.movementSpeed = baseStat.movementSpeed;
	}
	StopProfile;
}
void SE::Gameplay::PlayerUnit::calcWhiChanges()
{
	StartProfile;
	if (baseStat.whi > 5)
	{
		int increment = baseStat.whi - 5;
		newStat.magicDamage = baseStat.magicDamage * (1.f + (0.05f * increment));
		newStat.magicResistance = baseStat.magicResistance * (1.f + (0.025f * increment));
		newStat.natureResistance = baseStat.natureResistance * (1.f + (0.025f * increment));
		newStat.fireResistance = baseStat.fireResistance * (1.f + (0.025f * increment));
		newStat.waterResistance = baseStat.waterResistance * (1.f + (0.025f * increment));
	}
	else if (baseStat.whi < 5)
	{
		newStat.magicDamage = baseStat.magicDamage * (1.f - (0.05f * baseStat.whi));
		newStat.magicResistance = baseStat.magicResistance * (1.f - (0.05f * baseStat.whi));
		newStat.natureResistance = baseStat.natureResistance * (1.f - (0.05f * baseStat.whi));
		newStat.fireResistance = baseStat.fireResistance * (1.f - (0.05f * baseStat.whi));
		newStat.waterResistance = baseStat.waterResistance * (1.f - (0.05f * baseStat.whi));
	}
	else
	{
		newStat.magicDamage = baseStat.magicDamage;
		newStat.magicResistance = baseStat.magicResistance;
		newStat.natureResistance = baseStat.natureResistance;
		newStat.fireResistance = baseStat.fireResistance;
		newStat.waterResistance = baseStat.waterResistance;
	}
	StopProfile;
}

void SE::Gameplay::PlayerUnit::changeArmorType(ArmourType armour)
{
	newStat.armour = armour;
}
void SE::Gameplay::PlayerUnit::changeWeaponType(DamageSources weapon)
{
	newStat.weapon = weapon;
}
void SE::Gameplay::PlayerUnit::changeElementType(DamageTypes element)
{
	newStat.element = element;
}

void SE::Gameplay::PlayerUnit::flushSkills(std::vector<Skill> skills)
{
	skills.clear();
}

SE::Gameplay::PlayerUnit::PlayerUnit(Skill* skills, void* perks, float xPos, float yPos, char mapForRoom[25][25]) :
	GameUnit(xPos, yPos, 100)
{
	memcpy(this->map, mapForRoom, 25 * 25 * sizeof(char));
	extents = 0.25f; /*Should not be hardcoded! Obviously*/

	if (skills != nullptr)
	{
		this->skills.push_back(skills[0]);
		this->skills.push_back(skills[1]);
	}

	Core::IAnimationManager::CreateInfo sai;
	sai.mesh = "MCModell.mesh";
	sai.skeleton = "MCModell.skel";
	sai.animationCount = 4;

	Utilz::GUID anims[] = { "TopIdleAnim_MCModell.anim", "TopRunAnim_MCModell.anim" , "BottomRunAnim_MCModell.anim", "BottomIdleAnim_MCModell.anim" };
	sai.animations = anims;

	Core::IMaterialManager::CreateInfo info;
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	auto material = Utilz::GUID("MCModell.mat");
	info.shader = shader;
	info.materialFile = material;

	CoreInit::managers.materialManager->Create(unitEntity, info);

	CoreInit::managers.animationManager->CreateAnimatedObject(unitEntity, sai);
	//CoreInit::managers.renderableManager->CreateRenderableObject(unitEntity, { "MCModell.mesh" });
	CoreInit::managers.collisionManager->CreateBoundingHierarchy(unitEntity, "Run.mesh");

	CoreInit::managers.animationManager->ToggleVisible(unitEntity, true);
	//CoreInit::managers.renderableManager->ToggleRenderableObject(unitEntity, true);
}

SE::Gameplay::PlayerUnit::~PlayerUnit()
{
	StartProfile;

	this->DestroyEntity();

	ProfileReturnVoid;
}

