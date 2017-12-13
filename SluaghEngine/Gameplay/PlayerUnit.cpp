#include "PlayerUnit.h"
#include <Profiler.h>
#include "Flowfield.h"
#include "ProjectileData.h"
#include "CoreInit.h"
#include <KeyBindings.h>
#include <Items.h>
#include <Gameplay\PerkFactory.h>
#include <Gameplay\perkConditionEnum.h>


void SE::Gameplay::PlayerUnit::InitializeAnimationInfo()
{
	StartProfile;

	/*Initialize On Death animation*/
	animationPlayInfos[PLAYER_ON_DEATH_ANIMATION].push_back("DeathAnim_MCModell.anim"); //playInfo;

	/*Initialize On Hit animation*/
	animationPlayInfos[PLAYER_ON_HIT_ANIMATION].push_back("TopHitAnim_MCModell.anim"); //playInfo;

	/*Initialize Attack animation*/
	animationPlayInfos[PLAYER_ATTACK_ANIMATION].push_back("TopSwordAttackAnim_MCModell.anim"); //= playInfo;


	/*Initialize Idle Animation*/
	animationPlayInfos[PLAYER_IDLE_ANIMATION].push_back("TopIdleAnim_MCModell.anim");
	animationPlayInfos[PLAYER_IDLE_ANIMATION].push_back("BottomIdleAnim_MCModell.anim");// = playInfo;

	/*Initialize Run animation*/
	animationPlayInfos[PLAYER_RUN_ANIMATION].push_back("TopRunAnim_MCModell.anim");
	animationPlayInfos[PLAYER_RUN_ANIMATION].push_back("BottomRunAnim_MCModell.anim"); //= playInfo;
	
	this->deathAnimation = "DeathAnim_MCModell.anim";

	StopProfile;
	
}


bool SE::Gameplay::PlayerUnit::AnimationUpdate(AvailableAnimations animationToRun, Core::AnimationFlags animationFlags, float playSpeed)
{
	StartProfile;
	ProfileReturn(CoreInit::managers.animationManager->Start(unitEntity, &animationPlayInfos[animationToRun][0], animationPlayInfos[animationToRun].size(), playSpeed, animationFlags));
	
}
std::vector<SE::Gameplay::DamageEvent>& SE::Gameplay::PlayerUnit::GetDamageEvents()
{
	return DamageEventVector;
}
std::vector<SE::Gameplay::HealingEvent>& SE::Gameplay::PlayerUnit::GetHealingEvents()
{
	return HealingEventVector;
}
std::vector<SE::Gameplay::ConditionEvent>& SE::Gameplay::PlayerUnit::GetConditionEvents()
{
	return ConditionEventVector;
}
#undef max

void SE::Gameplay::PlayerUnit::ResolveEvents(float dt)
{
	StartProfile;

	// only basic at the moment
	auto ph = this->health;
	for (int i = 0; i < DamageEventVector.size(); i++)
	{
		this->health -= DamageEventVector[i].amount;
	}

	if (ph > this->health && !isSluagh)
	{
		if (!CoreInit::managers.entityManager->Alive(itemSelectedEntity))
		{
			itemSelectedEntity = CoreInit::managers.entityManager->Create();
			Core::IGUIManager::CreateInfo ise;
			ise.texture = "damageFrame.png";
			ise.textureInfo.width = CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Window", "width", 1280);
			ise.textureInfo.height = CoreInit::subSystems.optionsHandler->GetOptionUnsignedInt("Window", "height", 720);;
			ise.textureInfo.layerDepth = 1.0;
			ise.textureInfo.anchor = { 0.0f, 0.0f };
			ise.textureInfo.screenAnchor = { 0, 0 };
			ise.textureInfo.posX = 0;
			ise.textureInfo.posY = 0;
			float alpha = (1.0f - (health / (float)newStat.health) + 1.0f) / 2.0f;
			ise.textureInfo.colour = { 1.0f,1.0f,1.0f, alpha };
			CoreInit::managers.guiManager->Create(itemSelectedEntity, ise);
			CoreInit::managers.guiManager->ToggleRenderableTexture(itemSelectedEntity, true);
			CoreInit::managers.eventManager->SetLifetime(itemSelectedEntity, 0.15f);
		}
	
	}

	for (int i = 0; i < ConditionEventVector.size(); i++)
	{
		ConditionEventVector[i].duration -= dt;

		if (ConditionEventVector[i].isBane == false)
		{
			switch (ConditionEventVector[i].boon)
			{
			case Boons::CONDITIONAL_BOONS_NONE:
				break;
			case Boons::CONDITIONAL_BOONS_DAMAGE:
				newStat.meleeMultiplier += baseStat.meleeMultiplier * ConditionEventVector[i].effectValue;
				break;
			case Boons::CONDITIONAL_BOONS_STUN:
				break;
			case Boons::CONDITIONAL_BOONS_ROOT:
				break;
			case Boons::CONDITIONAL_BOONS_PROTECTION:
				this->newStat.physicalResistance += this->baseStat.physicalResistance * ConditionEventVector[i].effectValue;
				break;
			case Boons::CONDITIONAL_BOONS_PHYSICAL_RESISTANCE:
				this->newStat.physicalResistance += this->baseStat.physicalResistance * ConditionEventVector[i].effectValue;
				break;
			case Boons::CONDITIONAL_BOONS_MAGICAL_RESISTANCE:
				this->newStat.magicResistance += this->baseStat.magicResistance * ConditionEventVector[i].effectValue;
				break;
			case Boons::CONDITIONAL_BOONS_FIRE_RESISTANCE:
				this->newStat.fireResistance += this->baseStat.fireResistance * ConditionEventVector[i].effectValue;
				break;
			case Boons::CONDITIONAL_BOONS_WATER_RESISTANCE:
				this->newStat.waterResistance += this->baseStat.waterResistance * ConditionEventVector[i].effectValue;
				break;
			case Boons::CONDITIONAL_BOONS_NATURE_RESISTANCE:
				this->newStat.natureResistance += this->baseStat.natureResistance * ConditionEventVector[i].effectValue;
				break;
			case Boons::CONDITIONAL_BOONS_CASTSPEED:
				break;
			case Boons::CONDITIONAL_BOONS_SWIFTNESS:
				this->newStat.movementSpeed += this->baseStat.movementSpeed * ConditionEventVector[i].effectValue;
				break;
			case Boons::CONDITIONAL_BOONS_SLOW:
				break;
			case Boons::CONDITIONAL_BOONS_INVULNERABILITY:
				break;
			}
		}
		else
		{
			switch (ConditionEventVector[i].bane)
			{
			case Banes::CONDITIONAL_BANES_NONE:
				break;
			case Banes::CONDITIONAL_BANES_DAMAGE:
				this->newStat.meleeMultiplier -= this->baseStat.meleeMultiplier * ConditionEventVector[i].effectValue;
				break;
			case Banes::CONDITIONAL_BANES_STUN:
				this->isStunned = true;
				break;
			case Banes::CONDITIONAL_BANES_ROOT:
				this->newStat.movementSpeed = 0;
				break;
			case Banes::CONDITIONAL_BANES_BLOODLETTING:
				this->health -= this->baseStat.damage * ConditionEventVector[i].effectValue;
				break;
			case Banes::CONDITIONAL_BANES_UNCOVER:
				break;
			case Banes::CONDITIONAL_BANES_PHYSICAL_WEAKNESS:
				this->newStat.physicalResistance -= this->baseStat.physicalResistance * ConditionEventVector[i].effectValue;
				break;
			case Banes::CONDITIONAL_BANES_MAGICAL_WEAKNESS:
				this->newStat.magicResistance -= this->baseStat.magicResistance * ConditionEventVector[i].effectValue;
				break;
			case Banes::CONDITIONAL_BANES_FIRE_WEAKNESS:
				this->newStat.fireResistance -= this->baseStat.fireResistance * ConditionEventVector[i].effectValue;
				break;
			case Banes::CONDITIONAL_BANES_WATER_WEAKNESS:
				this->newStat.waterResistance -= this->baseStat.waterResistance * ConditionEventVector[i].effectValue;
				break;
			case Banes::CONDITIONAL_BANES_NATURE_WEAKNESS:
				this->newStat.natureResistance -= this->baseStat.natureResistance * ConditionEventVector[i].effectValue;
				break;
			case Banes::CONDITIONAL_BANES_SLOW:
				this->newStat.movementSpeed -= this->baseStat.movementSpeed * ConditionEventVector[i].effectValue;
				this->newStat.attackSpeed -= this->baseStat.attackSpeed * ConditionEventVector[i].effectValue;
				break;
			}
		}
		if (ConditionEventVector[i].duration > 0.f)
		{
			NextFrameCondition.push_back(ConditionEventVector[i]);
		}
	}

	for(auto healing : HealingEventVector)
	{
		health += healing.amount;
	}

	if (this->health > this->GetMaxHealth())
		this->health = this->GetMaxHealth();
	else if (this->health < 0)
		this->health = 0;
	
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

void SE::Gameplay::PlayerUnit::SetCurrentWeaponStats()
{
	auto cwe = items[currentItem];
	auto dm = CoreInit::managers.dataManager;
	weaponStats.str = std::get<int32_t>(dm->GetValue(cwe, "Str", 0));
	weaponStats.agi = std::get<int32_t>(dm->GetValue(cwe, "Agi", 0));
	weaponStats.whi = std::get<int32_t>(dm->GetValue(cwe, "Wis", 0));

	weaponStats.damage = std::get<int32_t>(dm->GetValue(cwe, "Damage", 0));
	weaponStats.health = std::get<int32_t>(dm->GetValue(cwe, "Health", 0));

	weaponStats.damageType = DamageType(std::get<int32_t>(dm->GetValue(cwe, "DamageType", 0)));
	auto type = Item::Weapon::Type(std::get<int32_t>(dm->GetValue(cwe, "Type", 0)));
	if (type == Item::Weapon::Type::SWORD)
		weaponStats.weapon = DamageSources::DAMAGE_SOURCE_MELEE;
	else
		weaponStats.weapon = DamageSources::DAMAGE_SOURCE_RANGED;
	auto an = std::get<uint32_t>(CoreInit::managers.dataManager->GetValue(cwe, "AttAnim", false));
	animationPlayInfos[PLAYER_ATTACK_ANIMATION][0] = Utilz::GUID(an);
}

void SE::Gameplay::PlayerUnit::SetGodMode(bool on)
{
	godMode = on;
}

void SE::Gameplay::PlayerUnit::SetSpeed(float speed)
{
	this->baseStat.movementSpeed = speed;
}

void SE::Gameplay::PlayerUnit::Suicide()
{
	this->health = -100.0f;
}

void SE::Gameplay::PlayerUnit::UpdatePlayerRotation(float camAngleX, float camAngleY)
{
	StartProfile;
	this->rotMov[0] = cosf(camAngleX);
	this->rotMov[1] = sinf(camAngleY);
	StopProfile;
}

void SE::Gameplay::PlayerUnit::UpdateMovement(float dt, const MovementInput & inputs)
{
	StartProfile;

	dt *= newStat.movementSpeed;
	float xMovement = 0.f;
	float yMovement = 0.f;

	if (!this->isStunned)
	{
		// Handle input and apply movement
		if (inputs.upButton)
			yMovement += newStat.movementSpeed;
		if (inputs.downButton)
			yMovement -= newStat.movementSpeed;
		if (inputs.leftButton)
			xMovement -= newStat.movementSpeed;
		if (inputs.rightButton)
			xMovement += newStat.movementSpeed;
	}
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
		AnimationUpdate(PLAYER_RUN_ANIMATION, Core::AnimationFlags::BLENDTO | Core::AnimationFlags::LOOP);
		
	}
	else
	{
		AnimationUpdate(PLAYER_IDLE_ANIMATION, Core::AnimationFlags::BLENDTO | Core::AnimationFlags::LOOP);
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

	if (!this->isStunned)
	{
		CoreInit::managers.transformManager->SetRotation(this->unitEntity, tempRot.x, tempRot.y, tempRot.z);
	}
	//-----------------------

	/*Move the entity in the normalized direction*/
	MoveEntity(xMovement * dt, yMovement * dt);
	StopProfile;
}

void SE::Gameplay::PlayerUnit::UpdateActions(float dt, std::vector<ProjectileData>& newProjectiles, const ActionInput& input)
{
	StartProfile;

	// Can only switch weapon if the player is not attacking
	if(playerAttackCooldown <= 0.0f){

		auto w = CoreInit::subSystems.window;
		bool ci = false;
		auto newItem = 0;
		if (!isSluagh)
	{
		if (CoreInit::subSystems.window->ButtonUp(GameInput::SHOWINFO))
			showingItem = currentItem;
	}
		if (input.one)
		{
			newItem = 0;
			ci = true;
		}
		else if (input.two)
		{
			newItem = 1;
			ci = true;
		}
		else if (input.three)
		{
			newItem = 2;
			ci = true;
		}
		else if (input.four)
	{
		newItem = 3;
		ci = true;
	}
		else if (input.five)
	{
		newItem = 4;
		ci = true;
	}
		if (ci && input.showInfo)
		{
			if (auto item = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[newItem], "Item", -1)); item != -1)
		{

				showingItem = newItem;
				CoreInit::managers.eventManager->TriggerEvent("StopRenderItemInfo", true);	
		}
		}
		if (ci)
	{
		if (!input.showInfo)
		{

			auto item = ItemType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[newItem], "Item", -1)));
			if (item == ItemType::WEAPON)
			{
				auto pit = ItemType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[currentItem], "Item", -1)));
				if(pit == ItemType::WEAPON)
					Item::Unequip(items[currentItem], unitEntity);

				currentItem = newItem;
				Item::Equip(items[currentItem], unitEntity);

				SetCurrentWeaponStats();
			}
			else if (item == ItemType::CONSUMABLE)
			{
				auto charges = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[newItem], "Charges", 0));
				if (charges > 0)
				{
					CoreInit::managers.audioManager->StopSound(this->unitEntity.id, currentSound);
					uint8_t soundToPlay;

					currentSound = playerHealingSounds[CoreInit::subSystems.window->GetRand() % nrHealingSounds];

					CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, currentSound);
					health += std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[newItem], "Health", 0));
					charges--;
					if (charges == 0)
					{
						CoreInit::managers.entityManager->DestroyNow(items[newItem]);
					}
					else
						CoreInit::managers.dataManager->SetValue(items[newItem], "Charges", charges);
				}
				
			}

		}
	}

	}

	this->newStat.str += weaponStats.str;
	this->newStat.agi += weaponStats.agi;
	this->newStat.whi += weaponStats.whi;

	this->newStat.damage += weaponStats.damage;
	this->newStat.health += weaponStats.health;

	this->newStat.damageType = weaponStats.damageType;
	this->newStat.weapon = weaponStats.weapon;

	this->calcNewStrChanges();
	this->calcNewAgiChanges();
	this->calcNewWhiChanges();

	int nrOfSKills = skills.size();

	if (nrOfSKills > 0 && skills[0].currentCooldown <= 0.0f && input.skill1Button)
	{
		ProjectileData temp;

		temp.startRotation = CoreInit::managers.transformManager->GetRotation(unitEntity).y;
		temp.startPosX = this->xPos;
		temp.startPosY = this->yPos;

		if (skills[0].atkType == DamageSources::DAMAGE_SOURCE_SELFCAST)
			temp.target = ValidTarget::PLAYER;
		else
			temp.target = ValidTarget::ENEMIES;

		temp.eventDamage = DamageEvent(skills[0].atkType, skills[0].damageType, skills[0].skillDamage);
		//temp.healingEvent = skills[0]->GetHealingEvent();
		//temp.conditionEvent = skills[0]->GetConditionEvent();
		if (skills[0].boon != Boons(1 << 0))
		{
			ConditionEvent::ConditionType condition;
			condition.unionType = 0;
			condition.condition.boon = skills[0].boon;
			ConditionEventVector.push_back(ConditionEvent(condition, skills[0].boonEffectValue, skills[0].boonDuration));
		}
		if (skills[0].bane != Banes(1 << 0))
		{
			ConditionEvent::ConditionType condition;
			condition.unionType = 1;
			condition.condition.bane = skills[0].bane;
			ConditionEventVector.push_back(ConditionEvent(condition, skills[0].boonEffectValue, skills[0].baneDuration));
		}
		if (!isSluagh)
		{
			CoreInit::managers.audioManager->StopSound(this->unitEntity.id, currentSound);
			if (/*SkillType == Damage*/true)
				currentSound = playerAggroSounds[CoreInit::subSystems.window->GetRand() % nrAggroSounds];
			else
				currentSound = playerHealingSounds[CoreInit::subSystems.window->GetRand() % nrHealingSounds];

			CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, currentSound);
		}
		temp.ownerUnit = mySelf;
		temp.fileNameGuid = skills[0].projectileFileGUID;

		newProjectiles.push_back(temp);
		skills[0].currentCooldown = skills[0].cooldown;
	}
	else if (nrOfSKills > 0 && input.skill1Button)
	{
		if (!isSluagh)
		{
			CoreInit::managers.audioManager->StopSound(this->unitEntity.id, currentSound);
			if (/*SkillType == Damage*/true)
				currentSound = playerAggroColdSounds[CoreInit::subSystems.window->GetRand() % nrAggroColdSounds];
			else
				currentSound = playerHealingColdSounds[CoreInit::subSystems.window->GetRand() % nrHealingColdSounds];

			CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, currentSound);
		}
	}

	if (nrOfSKills > 1 && skills[1].currentCooldown <= 0.0f && input.skill2Button)
	{
		ProjectileData temp;

		temp.startRotation = CoreInit::managers.transformManager->GetRotation(unitEntity).y;
		temp.startPosX = this->xPos;
		temp.startPosY = this->yPos;
		temp.target = ValidTarget::ENEMIES;
		temp.eventDamage = DamageEvent(skills[1].atkType, skills[1].damageType, skills[1].skillDamage);
		//temp.healingEvent = skills[1]->GetHealingEvent();
		//temp.conditionEvent = skills[1]->GetConditionEvent();
		if (skills[1].boon != Boons(1 << 0))
		{
			ConditionEvent::ConditionType condition;
			condition.unionType = 0;
			condition.condition.boon = skills[1].boon;
			ConditionEventVector.push_back(ConditionEvent(condition, skills[1].boonEffectValue, skills[1].boonDuration));
		}
		if (skills[1].bane != Banes(1 << 0))
		{
			ConditionEvent::ConditionType condition;
			condition.unionType = 1;
			condition.condition.bane = skills[1].bane;
			ConditionEventVector.push_back(ConditionEvent(condition, skills[1].boonEffectValue, skills[1].baneDuration));
		}
		if (!isSluagh)
		{
			CoreInit::managers.audioManager->StopSound(this->unitEntity.id, currentSound);
			uint8_t soundToPlay;
			if (/*SkillType == Damage*/true)
				currentSound = playerAggroSounds[CoreInit::subSystems.window->GetRand() % nrAggroSounds];
			else
				currentSound = playerHealingSounds[CoreInit::subSystems.window->GetRand() % nrHealingSounds];

			CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, currentSound);
		}
		temp.ownerUnit = mySelf;
		temp.fileNameGuid = skills[1].projectileFileGUID;

		newProjectiles.push_back(temp);
		skills[1].currentCooldown = skills[1].cooldown;
	}
	else if (nrOfSKills > 1 && input.skill2Button)
	{
		if (!isSluagh)
		{
			CoreInit::managers.audioManager->StopSound(this->unitEntity.id, currentSound);
			if (/*SkillType == Damage*/true)
				currentSound = playerAggroColdSounds[CoreInit::subSystems.window->GetRand() % nrAggroColdSounds];
			else
				currentSound = playerHealingColdSounds[CoreInit::subSystems.window->GetRand() % nrHealingColdSounds];

			CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, currentSound);
		}
	}

	if (nrOfSKills > 0 && skills[0].currentCooldown > 0.0f)
	{
		skills[0].currentCooldown -= dt;
	}
	if (nrOfSKills > 1 && skills[1].currentCooldown > 0.0f)
	{
		skills[1].currentCooldown -= dt;
	}
	
	if (input.actionButton && playerAttackCooldown <= 0.0f)
	{
		if (auto equipped = std::get<bool>(CoreInit::managers.dataManager->GetValue(items[currentItem], "Equipped", false)); equipped)
		{
				// Only allow attacking if attack animation is not already playing and attacking is false
				if (AnimationUpdate(PLAYER_ATTACK_ANIMATION, Core::AnimationFlags::BLENDTOANDBACK, 1.0f/ this->newStat.attackSpeed))
				{

					ProjectileData temp;

					temp.startRotation = CoreInit::managers.transformManager->GetRotation(unitEntity).y;
					temp.startPosX = this->xPos;
					temp.startPosY = this->yPos;
					temp.target = ValidTarget::ENEMIES;

					temp.eventDamage = DamageEvent(weaponStats.weapon, weaponStats.damageType, newStat.damage);
					temp.ownerUnit = mySelf;

					Utilz::GUID p = Utilz::GUID(std::get<uint32_t>(CoreInit::managers.dataManager->GetValue(items[currentItem], "AttProj", false)));
		
					temp.fileNameGuid = Utilz::GUID(p);
					newProjectiles.push_back(temp);

					this->playerAttackCooldown = 1.5f;
				}
			
		}
		
	}
	
	std::string cooldownDebug = std::to_string(this->playerAttackCooldown);
	CoreInit::subSystems.devConsole->Print(cooldownDebug.c_str());

	if (this->playerAttackCooldown > 0.0f)
	{
		this->playerAttackCooldown -= dt;
	}
	else if (this->playerAttackCooldown <= 0.0f){
		
		this->playerAttackCooldown = 0.0f;
	}

	handlePerks(dt, this, newProjectiles);

	ResolveEvents(dt);
	//ClearConditionEvents();
	//ClearDamageEvents();
	//ClearHealingEvents();
	StopProfile;

}

void SE::Gameplay::PlayerUnit::UpdateMap(char** mapForRoom)
{
	StartProfile;
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			map[i][j] = mapForRoom[i][j];
		}
	}
	StopProfile;
}

void SE::Gameplay::PlayerUnit::Update(float dt, const MovementInput & mInputs, std::vector<ProjectileData>& newProjectiles, const ActionInput & aInput)
{
	StartProfile;
	if (health > 0.f)
	{
		ClearNewStats();
		isStunned = false;
		UpdateActions(dt, newProjectiles, aInput);
		UpdateMovement(dt, mInputs);

		ClearConditionEvents();
		ClearDamageEvents();
		ClearHealingEvents();
	}
	if (godMode)
		health = GetMaxHealth();
	StopProfile;
}

void SE::Gameplay::PlayerUnit::handlePerks(float deltaTime,PlayerUnit* player , std::vector<ProjectileData>& newProjectiles)
{
	int nrOf = newProjectiles.size();
	bool cond = false;
	for (auto& perk: perks)
	{
		cond = perk.checkConditions(newProjectiles,this);
		for (auto& func: perk.perkFunctions)
		{
			func(this,newProjectiles,deltaTime,cond);
		}
	}


}

void SE::Gameplay::PlayerUnit::AddItem(Core::Entity item, uint8_t slot)
{
	StartProfile;
	_ASSERT(slot < MAX_ITEMS);
	
	auto itype = (ItemType)(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(item, "Item", -1)));

	auto isitem = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[slot], "Item", -1));
	if (isitem != -1)
	{
		auto p = CoreInit::managers.transformManager->GetPosition(unitEntity);
		p.y = 0;
		if (currentItem == slot)
		{
			auto pit = ItemType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[currentItem], "Item", -1)));
			if (pit == ItemType::WEAPON)
				Item::Unequip(items[currentItem], unitEntity);

		}

		Item::Drop(items[slot], p);

	}
		
	if (!isSluagh)
	{
		CoreInit::managers.guiManager->SetTexturePos(item, 45 + slot * 60, -55);
		Item::Pickup(item);
	}
	else
	{
		Item::GodPickup(item);
	}

	
	if (itype == ItemType::WEAPON)
	{
		
		if (auto isitem = ItemType( std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[currentItem], "Item", -1))); isitem == ItemType::WEAPON)
		{
			Item::Unequip(items[currentItem], unitEntity);
		}

		currentItem = slot;
		items[slot] = item;
		Item::Equip(items[currentItem], unitEntity);
		SetCurrentWeaponStats();
	}
	items[slot] = item;
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
void SE::Gameplay::PlayerUnit::changeElementType(DamageType dmgT)
{
	newStat.damageType = dmgT;
}

int SE::Gameplay::PlayerUnit::getSkillVectorSize()
{
	return skills.size();
}

void SE::Gameplay::PlayerUnit::ToggleAsSluagh(bool sluagh)
{
	isSluagh = sluagh;
	Core::IMaterialManager::CreateInfo info;
	if (sluagh)
	{
		auto shader = Utilz::GUID("SimpleLightPS.hlsl");
		auto material = Utilz::GUID("Slaughplane.mat");
		info.shader = shader;
		info.materialFile = material;
	}
	else
	{
		auto shader = Utilz::GUID("SimpleNormMapPS.hlsl");
		auto material = Utilz::GUID("MCModell.mat");
		info.shader = shader;
		info.materialFile = material;
	}

	CoreInit::managers.materialManager->Create(unitEntity, info);
}
void SE::Gameplay::PlayerUnit::PlayerSounds()
{
	playerAggroSounds[0] = Utilz::GUID("Bullar.wav");
	playerAggroSounds[1] = Utilz::GUID("ÄtJärn.wav");
	playerAggroSounds[2] = Utilz::GUID("RedoEller.wav");
	playerAggroSounds[3] = Utilz::GUID("FörHeder.wav");
	playerAggroSounds[4] = Utilz::GUID("NuSkaDu.wav");
	playerAggroSounds[5] = Utilz::GUID("SmakaPå.wav");
	playerHealingSounds[0] = Utilz::GUID("FörGammal.wav");
	playerHealingSounds[1] = Utilz::GUID("KännsBättre.wav");
	playerHealingSounds[2] = Utilz::GUID("SätterPlåster.wav");
	playerAggroColdSounds[0] = Utilz::GUID("VäntaLite.wav");
	playerAggroColdSounds[1] = Utilz::GUID("IngenTrollformel.wav");
	playerAggroColdSounds[2] = Utilz::GUID("Nedkyllning.wav");
	playerHealingColdSounds[0] = Utilz::GUID("IngePlåster.wav");

	CoreInit::managers.audioManager->Create(unitEntity, { Utilz::GUID("deathMC.wav"), SE::Audio::VoiceSound });
	CoreInit::managers.dataManager->SetValue(unitEntity, SE::Utilz::GUID("deathSoundGUID"), static_cast<uint32_t>(Utilz::GUID("deathMC.wav").id));


	for (int i = 0; i < nrAggroSounds; ++i)
	{
		CoreInit::managers.audioManager->Create(unitEntity, { playerAggroSounds[i], SE::Audio::VoiceSound });
	}
	for (int i = 0; i < nrHealingSounds; ++i)
	{
		CoreInit::managers.audioManager->Create(unitEntity, { playerHealingSounds[i], SE::Audio::VoiceSound });
	}
	for (int i = 0; i < nrAggroColdSounds; ++i)
	{
		CoreInit::managers.audioManager->Create(unitEntity, { playerAggroColdSounds[i], SE::Audio::VoiceSound });
	}
	for (int i = 0; i < nrHealingColdSounds; ++i)
	{
		CoreInit::managers.audioManager->Create(unitEntity, { playerHealingColdSounds[i], SE::Audio::VoiceSound });
	}

}

SE::Gameplay::PlayerUnit::PlayerUnit(Skill* skills, Perk* importPerks, PerkData* slaughPerks,float xPos, float yPos, char mapForRoom[25][25]) :
	GameUnit(xPos, yPos, 100)
{
	StartProfile;
	memcpy(this->map, mapForRoom, 25 * 25 * sizeof(char));
	extents = 0.25f; /*Should not be hardcoded! Obviously*/
	this->health = baseStat.health;

	if (skills != nullptr)
	{
		this->skills.push_back(skills[0]);
		this->skills.push_back(skills[1]);
		
	}

	if (importPerks != nullptr)
	{
		this->perks.push_back(importPerks[0]);
		this->perks.push_back(importPerks[1]);

		perkConditions myCond = static_cast<perkConditions>(this->perks[0].intToEnum);
		perkConditions secondCond = static_cast<perkConditions>(this->perks[1].intToEnum);

		this->perks[0].myCondition = myCond;
		this->perks[1].myCondition = secondCond;
		this->perks[0].slaughPerk = slaughPerks[0];
		this->perks[1].slaughPerk = slaughPerks[1];

	}

	Core::IAnimationManager::CreateInfo sai;
	sai.mesh = "MCModell.mesh";
	sai.skeleton = "MCModell.skel";
	sai.animationCount = 4;

	Utilz::GUID anims[] = { "BottomRunAnim_MCModell.anim", "BottomIdleAnim_MCModell.anim", "TopRunAnim_MCModell.anim", "TopIdleAnim_MCModell.anim",
		"DeathAnim_MCModell.anim", "TopAttackAnim_MCModell.anim", "TopHitAnim_MCModell.anim" };
	sai.animations = anims;

	Core::IMaterialManager::CreateInfo info;
	auto shader = Utilz::GUID("SimpleNormMapPS.hlsl");
	auto material = Utilz::GUID("MCModell.mat");
	info.shader = shader;
	info.materialFile = material;

	CoreInit::managers.materialManager->Create(unitEntity, info);
	CoreInit::managers.animationManager->CreateAnimatedObject(unitEntity, sai);
	CoreInit::managers.animationManager->ToggleShadow(unitEntity, true);
	CoreInit::managers.collisionManager->CreateBoundingHierarchy(unitEntity, "MCModell.mesh");

	CoreInit::managers.animationManager->ToggleVisible(unitEntity, true);

	PlayerSounds();
	InitializeAnimationInfo();

	CoreInit::managers.animationManager->Start(unitEntity, &animationPlayInfos[PLAYER_IDLE_ANIMATION][0], animationPlayInfos[PLAYER_IDLE_ANIMATION].size(), 1.f, Core::AnimationFlags::LOOP | Core::AnimationFlags::IMMEDIATE);
	

	itemSelectedEntity = CoreInit::managers.entityManager->Create();
	CoreInit::managers.entityManager->Destroy(itemSelectedEntity);
	

	StopProfile;
}

SE::Gameplay::PlayerUnit::PlayerUnit(std::ifstream &input, float xPos, float yPos, char mapForRoom[25][25])
	: GameUnit(xPos, yPos, 100)
{

	StartProfile;
	PerkFaktory perkFactory;
	memcpy(this->map, mapForRoom, 25 * 25 * sizeof(char));
	extents = 0.25f; /*Should not be hardcoded! Obviously*/

	input.read((char*)&baseStat, sizeof(baseStat));
	skills.resize(2);
	/*Memset to avoid undefined behaviour*/
	input.read((char*)&skills[0], sizeof(skills[0]));
	memset(&skills[0].skillName, 0, sizeof(skills[0].skillName));
	skills[0].skillName = std::string("");
	input.read((char*)&skills[1], sizeof(skills[1]));
	memset(&skills[1].skillName, 0, sizeof(skills[1].skillName));
	skills[1].skillName = std::string("");

	char perkname[255];
	int size;
	input.read((char*)&size, sizeof(int));
	input.read(perkname, size);
	std::string perkString(perkname, size);
	perks.push_back(perkFactory.ReadPerksForSlaugh(perkString));
	input.read((char*)&size, sizeof(int));
	input.read(perkname, size);
	perkString = std::string(perkname, size);
	perks.push_back(perkFactory.ReadPerksForSlaugh(perkString));
	
	

	for (int i = 0; i < MAX_ITEMS; i++)
	{
		items[i] = Item::Create(input);

	}

	Core::IAnimationManager::CreateInfo sai;
	sai.mesh = "MCModell.mesh";
	sai.skeleton = "MCModell.skel";
	sai.animationCount = 4;

	Utilz::GUID anims[] = { "BottomRunAnim_MCModell.anim", "BottomIdleAnim_MCModell.anim", "TopRunAnim_MCModell.anim", "TopIdleAnim_MCModell.anim",
		"DeathAnim_MCModell.anim", "TopAttackAnim_MCModell.anim", "TopHitAnim_MCModell.anim" };
	sai.animations = anims;

	Core::IMaterialManager::CreateInfo info;
	auto shader = Utilz::GUID("SimpleNormMapPS.hlsl");
	auto material = Utilz::GUID("MCModell.mat");
	info.shader = shader;
	info.materialFile = material;

	CoreInit::managers.materialManager->Create(unitEntity, info);

	CoreInit::managers.animationManager->CreateAnimatedObject(unitEntity, sai);
	CoreInit::managers.animationManager->ToggleShadow(unitEntity, true);
	CoreInit::managers.collisionManager->CreateBoundingHierarchy(unitEntity, "MCModell.mesh");

	CoreInit::managers.animationManager->ToggleVisible(unitEntity, true);

	PlayerSounds();
	InitializeAnimationInfo();

	CoreInit::managers.animationManager->Start(unitEntity, &animationPlayInfos[PLAYER_IDLE_ANIMATION][0], animationPlayInfos[PLAYER_IDLE_ANIMATION].size(), 1.f, Core::AnimationFlags::LOOP | Core::AnimationFlags::IMMEDIATE);

	itemSelectedEntity = CoreInit::managers.entityManager->Create();
	CoreInit::managers.entityManager->Destroy(itemSelectedEntity);
	

	StopProfile;
}

void SE::Gameplay::PlayerUnit::SavePlayerToFile(std::ofstream &toSave)
{
	StartProfile;
	/*Save Stats*/
	toSave.write((char*)&baseStat, sizeof(baseStat));

	/*Save Skills*/
	for (int i = 0; i < 2; i++)
		toSave.write((char*)&skills[i], sizeof(skills[i]));

	/*Save Perks*/
	int size = perks[0].slaughPerk.name.size();
	perks[0].slaughPerk.name.resize(size);
	toSave.write((char*)&size, sizeof(int));
	toSave.write(perks[0].slaughPerk.name.c_str(), perks[0].slaughPerk.name.size());
	size = perks[1].slaughPerk.name.size();
	perks[1].slaughPerk.name.resize(size);
	toSave.write((char*)&size, sizeof(int));
	toSave.write(perks[1].slaughPerk.name.c_str(), perks[1].slaughPerk.name.size());

	/*Save Weapons*/
	for (int i = 0; i < MAX_ITEMS; i++)
		Item::WriteToFile(items[i], toSave);
	StopProfile;
}

SE::Gameplay::PlayerUnit::~PlayerUnit()
{
	StartProfile;

	this->DestroyEntity();
	for (int i = 0; i < 5; i++)
		if (auto item = (std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[i], "Item", -1))); item != -1)
		{
			CoreInit::managers.entityManager->DestroyNow(items[i]);
		}



	ProfileReturnVoid;
}

