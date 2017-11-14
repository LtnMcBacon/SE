#include "PlayerUnit.h"
#include <Profiler.h>
#include "Flowfield.h"
#include "ProjectileData.h"
#include "CoreInit.h"

void SE::Gameplay::PlayerUnit::InitializeAnimationInfo()
{
	StartProfile;

	/*Initialize On Death animation*/
	animationPlayInfos[PLAYER_ON_DEATH_ANIMATION].push_back("DeathAnim_MCModell.anim"); //playInfo;

	/*Initialize On Hit animation*/
	animationPlayInfos[PLAYER_ON_HIT_ANIMATION].push_back("TopHitAnim_MCModell.anim"); //playInfo;

	/*Initialize Attack animation*/
	animationPlayInfos[PLAYER_ATTACK_ANIMATION].push_back("TopAttackAnim_MCModell.anim"); //= playInfo;


	/*Initialize Idle Animation*/
	animationPlayInfos[PLAYER_IDLE_ANIMATION].push_back("TopIdleAnim_MCModell.anim");
	animationPlayInfos[PLAYER_IDLE_ANIMATION].push_back("BottomIdleAnim_MCModell.anim");// = playInfo;

	/*Initialize Run animation*/
	animationPlayInfos[PLAYER_RUN_ANIMATION].push_back("TopRunAnim_MCModell.anim");
	animationPlayInfos[PLAYER_RUN_ANIMATION].push_back("BottomRunAnim_MCModell.anim"); //= playInfo;
		

	StopProfile;
	
}


void SE::Gameplay::PlayerUnit::AnimationUpdate(AvailableAnimations animationToRun, Core::AnimationFlags animationFlags)
{
	StartProfile;
	CoreInit::managers.animationManager->Start(unitEntity, &animationPlayInfos[animationToRun][0], animationPlayInfos[animationToRun].size(), 1.f, animationFlags);
	StopProfile;
}

void SE::Gameplay::PlayerUnit::ResolveEvents()
{
	StartProfile;

	// only basic at the moment
	
	for (int i = 0; i < DamageEventVector.size(); i++)
	{
		this->health -= DamageEventVector[i].amount;
	}
	
	for(auto condition : ConditionEventVector)
	{
		if(condition.type == ConditionEvent::ConditionTypes::CONDITION_TYPE_STUN)
		{
			stunDuration += condition.duration;
		}
	}

	for(auto healing : HealingEventVector)
	{
		health += healing.amount;
	}
	
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

	CoreInit::managers.transformManager->SetRotation(this->unitEntity, tempRot.x, tempRot.y, tempRot.z);

	//-----------------------

	/*Move the entity in the normalized direction*/
	MoveEntity(xMovement * dt, yMovement * dt);
	StopProfile;
}
#include <KeyBindings.h>
#include <Items.h>
void SE::Gameplay::PlayerUnit::UpdateActions(float dt, std::vector<ProjectileData>& newProjectiles, const ActionInput& input)
{
	StartProfile;

	auto w = CoreInit::subSystems.window;
	bool ci = false;
	auto pi = currentItem;
	if (w->ButtonPressed(GameInput::ONE))
	{
		currentItem = 0;
		ci = true;
	}
	else if (w->ButtonPressed(GameInput::TWO))
	{
		currentItem = 1;;
		ci = true;
	}
	else if (w->ButtonPressed(GameInput::THREE))
	{
		currentItem = 2;;
		ci = true;
	}
	else if (w->ButtonPressed(GameInput::FOUR))
	{
		currentItem = 3;;
		ci = true;
	}
	else if (w->ButtonPressed(GameInput::FIVE))
	{
		currentItem = 4;;
		ci = true;
	}

	if (ci)
	{
		if(auto item = std::get<bool>(CoreInit::managers.dataManager->GetValue(items[currentItem], "Item", false)))
		{
			auto type = ItemType(std::get<int32_t>(CoreInit::managers.dataManager->GetValue(items[currentItem], "Type", false)));
			CoreInit::managers.guiManager->SetTexturePos(itemSelectedEntity, 40 + currentItem * 55, -55);
		}
		else
		{
			currentItem = pi;		
		}
	}





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

		CoreInit::managers.audioManager->StopSound(this->unitEntity.id, currentSound);
		if (/*SkillType == Damage*/true)
			currentSound = playerAggroSounds[CoreInit::subSystems.window->GetRand() % nrAggroSounds];
		else
			currentSound = playerHealingSounds[CoreInit::subSystems.window->GetRand() % nrHealingSounds];
		
		CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, currentSound);
		temp.ownerUnit = mySelf;
		temp.fileNameGuid = skills[0].projectileFileGUID;

		newProjectiles.push_back(temp);
		skills[0].currentCooldown = skills[0].coolDown;
	}
	else if (nrOfSKills > 0 && input.skill1Button)
	{
		CoreInit::managers.audioManager->StopSound(this->unitEntity.id, currentSound);
		if (/*SkillType == Damage*/true)
			currentSound = playerAggroColdSounds[CoreInit::subSystems.window->GetRand() % nrAggroColdSounds];
		else
			currentSound = playerHealingColdSounds[CoreInit::subSystems.window->GetRand() % nrHealingColdSounds];

		CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, currentSound);
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

		CoreInit::managers.audioManager->StopSound(this->unitEntity.id, currentSound);
		uint8_t soundToPlay;
		if (/*SkillType == Damage*/true)
			currentSound = playerAggroSounds[CoreInit::subSystems.window->GetRand() % nrAggroSounds];
		else
			currentSound = playerHealingSounds[CoreInit::subSystems.window->GetRand() % nrHealingSounds];

		CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, currentSound);
		temp.ownerUnit = mySelf;
		temp.fileNameGuid = skills[1].projectileFileGUID;

		newProjectiles.push_back(temp);
		skills[1].currentCooldown = skills[1].coolDown;
	}
	else if (nrOfSKills > 1 && input.skill2Button)
	{
		CoreInit::managers.audioManager->StopSound(this->unitEntity.id, currentSound);
		if (/*SkillType == Damage*/true)
			currentSound = playerAggroColdSounds[CoreInit::subSystems.window->GetRand() % nrAggroColdSounds];
		else
			currentSound = playerHealingColdSounds[CoreInit::subSystems.window->GetRand() % nrHealingColdSounds];

		CoreInit::managers.audioManager->PlaySound(this->unitEntity.id, currentSound);
	}

	if (nrOfSKills > 0 && skills[0].currentCooldown > 0.0f)
	{
		skills[0].currentCooldown -= dt;
	}
	if (nrOfSKills > 1 && skills[1].currentCooldown > 0.0f)
	{
		skills[1].currentCooldown -= dt;
	}

	if (input.actionButton && attackCooldown <= 0.0f)
	{
		ProjectileData temp;

		temp.startRotation = CoreInit::managers.transformManager->GetRotation(unitEntity).y;
		temp.startPosX = this->xPos;
		temp.startPosY = this->yPos;
		temp.target = ValidTarget::ENEMIES;
		temp.eventDamage = DamageEvent(DamageSources::DAMAGE_SOURCE_MELEE, Element::PHYSICAL, 2);
		temp.ownerUnit = mySelf;
		temp.fileNameGuid = "playerMeleeProjectiles.SEP";

		newProjectiles.push_back(temp);

		attackCooldown = 1.0f / attackSpeed;

		//AnimationUpdate(PLAYER_ATTACK_ANIMATION, Core::AnimationFlags::BLENDTOANDBACK);
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
	UpdateMovement(dt, mInputs);
	UpdateActions(dt, newProjectiles, aInput);

	ClearConditionEvents();
	ClearDamageEvents();
	ClearHealingEvents();
	StopProfile;
}
#include <Items.h>
void SE::Gameplay::PlayerUnit::AddWeapon(Core::Entity wep, uint8_t slot)
{
	_ASSERT(slot < MAX_ITEMS);
	auto item = std::get<bool>( CoreInit::managers.dataManager->GetValue(items[slot], "Item", false));
	if (item)
	{
		//CoreInit::managers.entityManager->Destroy(items[currentItem]);
		auto p = CoreInit::managers.transformManager->GetPosition(unitEntity);
		p.y = 0;
		Item::Drop(items[slot], p);
	}
	CoreInit::managers.guiManager->SetTexturePos(wep, 40 + slot * 55, -55);
	CoreInit::managers.guiManager->SetTexturePos(itemSelectedEntity, 40 + slot * 55, -55);
	CoreInit::managers.guiManager->ToggleRenderableTexture(itemSelectedEntity, true);

	Item::Pickup(wep);

	items[slot] = wep;
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
void SE::Gameplay::PlayerUnit::changeElementType(Element element)
{
	newStat.element = element;
}

void SE::Gameplay::PlayerUnit::flushSkills(std::vector<Skill> skills)
{
	skills.clear();
}

void SE::Gameplay::PlayerUnit::PlayerSounds()
{
	playerAggroSounds[0] = Utilz::GUID("DefaultAttackSound.wav");
	playerAggroSounds[1] = Utilz::GUID("DefaultAttackSound.wav");
	playerAggroSounds[2] = Utilz::GUID("DefaultAttackSound.wav");
	playerAggroSounds[3] = Utilz::GUID("DefaultAttackSound.wav");
	playerAggroSounds[4] = Utilz::GUID("DefaultAttackSound.wav");
	playerAggroSounds[5] = Utilz::GUID("DefaultAttackSound.wav");
	playerHealingSounds[0] = Utilz::GUID("DefaultAttackSound.wav");
	playerHealingSounds[1] = Utilz::GUID("DefaultAttackSound.wav");
	playerHealingSounds[2] = Utilz::GUID("DefaultAttackSound.wav");
	playerAggroColdSounds[0] = Utilz::GUID("DefaultAttackSound.wav");
	playerAggroColdSounds[1] = Utilz::GUID("DefaultAttackSound.wav");
	playerAggroColdSounds[2] = Utilz::GUID("DefaultAttackSound.wav");
	playerHealingColdSounds[0] = Utilz::GUID("DefaultAttackSound.wav");


	for (int i = 0; i < nrAggroSounds; ++i)
	{
		CoreInit::managers.audioManager->Create(unitEntity, { playerAggroSounds[i], SE::Audio::StereoPanSound });
	}
	for (int i = 0; i < nrHealingSounds; ++i)
	{
		CoreInit::managers.audioManager->Create(unitEntity, { playerHealingSounds[i], SE::Audio::StereoPanSound });
	}
	for (int i = 0; i < nrAggroColdSounds; ++i)
	{
		CoreInit::managers.audioManager->Create(unitEntity, { playerAggroColdSounds[i], SE::Audio::StereoPanSound });
	}
	for (int i = 0; i < nrHealingColdSounds; ++i)
	{
		CoreInit::managers.audioManager->Create(unitEntity, { playerHealingColdSounds[i], SE::Audio::StereoPanSound });
	}

}

SE::Gameplay::PlayerUnit::PlayerUnit(Skill* skills, void* perks, float xPos, float yPos, char mapForRoom[25][25]) :
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

	Core::IAnimationManager::CreateInfo sai;
	sai.mesh = "MCModell.mesh";
	sai.skeleton = "MCModell.skel";
	sai.animationCount = 4;

	Utilz::GUID anims[] = { "BottomRunAnim_MCModell.anim", "BottomIdleAnim_MCModell.anim", "TopRunAnim_MCModell.anim", "TopIdleAnim_MCModell.anim",
		"DeathAnim_MCModell.anim", "TopAttackAnim_MCModell.anim", "TopHitAnim_MCModell.anim" };
	sai.animations = anims;

	Core::IMaterialManager::CreateInfo info;
	auto shader = Utilz::GUID("SimpleLightPS.hlsl");
	auto material = Utilz::GUID("MCModell.mat");
	info.shader = shader;
	info.materialFile = material;

	CoreInit::managers.materialManager->Create(unitEntity, info);

	CoreInit::managers.animationManager->CreateAnimatedObject(unitEntity, sai);
	
	CoreInit::managers.collisionManager->CreateBoundingHierarchy(unitEntity, "MCModell.mesh");

	CoreInit::managers.animationManager->ToggleVisible(unitEntity, true);

	PlayerSounds();
	InitializeAnimationInfo();



	CoreInit::managers.animationManager->Start(unitEntity, &animationPlayInfos[PLAYER_IDLE_ANIMATION][0], animationPlayInfos[PLAYER_IDLE_ANIMATION].size(), 1.f, Core::AnimationFlags::LOOP | Core::AnimationFlags::IMMEDIATE);
	
	
	
	
	
	
	Core::IEventManager::EventCallbacks startRenderItemInfo;
	startRenderItemInfo.triggerCheck = [](const Core::Entity ent, void* data)
	{
		return CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO);
	};
	
	startRenderItemInfo.triggerCallback = [this](const Core::Entity ent, void *data)
	{
		auto item = std::get<bool>(CoreInit::managers.dataManager->GetValue(items[currentItem], "Item", false));
		if (item)
		{
			CoreInit::managers.eventManager->UnregisterEntitytoEvent(unitEntity, "StartRenderItemInfo");
			Item::ToggleRenderEquiuppedInfo(items[currentItem], unitEntity);
		}
	};


	Core::IEventManager::EventCallbacks stopRenderItemInfo;
	stopRenderItemInfo.triggerCheck = [](const Core::Entity ent, void* data)
	{
		return !CoreInit::subSystems.window->ButtonDown(GameInput::SHOWINFO) || CoreInit::subSystems.window->ButtonPressed(GameInput::PICKUP);
	};

	stopRenderItemInfo.triggerCallback = [this](const Core::Entity ent, void *data)
	{
		CoreInit::managers.entityManager->DestroyNow(ent);
		CoreInit::managers.eventManager->RegisterEntitytoEvent(unitEntity, "StartRenderItemInfo");
	};


	CoreInit::managers.eventManager->RegisterEventCallback("StartRenderItemInfo", startRenderItemInfo);
	CoreInit::managers.eventManager->RegisterEventCallback("StopRenderItemInfo", stopRenderItemInfo);

	CoreInit::managers.eventManager->RegisterEntitytoEvent(unitEntity, "StartRenderItemInfo");


	itemSelectedEntity = CoreInit::managers.entityManager->Create();
	Core::IGUIManager::CreateInfo ise;
	ise.texture = "Fire.png";
	ise.textureInfo.width = 60;
	ise.textureInfo.height = 60;
	ise.textureInfo.layerDepth = 0.001;
	ise.textureInfo.anchor = { 0.5f, 0.5f };
	ise.textureInfo.screenAnchor = { 0, 1 };
	ise.textureInfo.posX = currentItem * 55 + 40;
	ise.textureInfo.posY = -55;
	CoreInit::managers.guiManager->Create(itemSelectedEntity, ise);

	StopProfile;
}

SE::Gameplay::PlayerUnit::~PlayerUnit()
{
	StartProfile;

	this->DestroyEntity();

	ProfileReturnVoid;
}

