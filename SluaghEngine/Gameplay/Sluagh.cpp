#include "..\includes\Gameplay\Sluagh.h"
#include "ProjectileData.h"
#include <Profiler.h>
#include "CoreInit.h"
#include "SluaghRoom.h"
#include "Items.h"

#include <fstream>

SE::Gameplay::Sluagh::Sluagh(PlayerUnit * thePlayer, SluaghRoom* room)
	:thePlayer(thePlayer)
{
	char map[25][25];
	room->GetMap(map);

	this->room = room;
	theSluagh = nullptr;
	distanceToPlayer = 0.f;
}

SE::Gameplay::Sluagh::~Sluagh()
{
	delete theSluagh;
}

void SE::Gameplay::Sluagh::Update(float dt, std::vector<ProjectileData>& projectilesOut)
{
	StartProfile;
	PlayerUnit::MovementInput moveInput(false, false, false, false, false, 0.f, 0.f);
	PlayerUnit::ActionInput actionInput;
	/*Utility function calls here*/
	DecideActions(dt, moveInput, actionInput);

	/*Mouse pos = player pos => always follow the player*/
	moveInput.mousePosX = thePlayer->GetXPosition();
	moveInput.mousePosY = thePlayer->GetYPosition();


	/*Resolve player update*/
	theSluagh->Update(dt, moveInput, projectilesOut, actionInput);

	previousMovement = moveInput;
	previousAction = actionInput;

	for(auto &projectile : projectilesOut)
	{
		projectile.target = ValidTarget::PLAYER;
	}

	ProfileReturnVoid;
}

bool SE::Gameplay::Sluagh::CollisionAgainstProjectile(float projectileX, float projectileY, float projectileRadius)
{
	StartProfile;
	float xDistance = projectileX - theSluagh->GetXPosition();
	float yDistance = projectileX - theSluagh->GetYPosition();
	static const float sluaghRadius = sqrtf(0.25f*0.25f*2);
	ProfileReturn((sqrtf(xDistance*xDistance + yDistance*yDistance) < projectileRadius+sluaghRadius));
}

void SE::Gameplay::Sluagh::ToggleRendering(bool render)
{
	CoreInit::managers.eventManager->ToggleVisible(theSluagh->GetEntity(), render);
	CoreInit::managers.eventManager->ToggleVisible(theSluagh->GetCurrentItem(), render);
}

void SE::Gameplay::Sluagh::InitializeSluagh()
{
	StartProfile;
	char* appdataBuffer;
	size_t bcount;
	if (_dupenv_s(&appdataBuffer, &bcount, "APPDATA"))
		throw std::exception("Failed to retrieve path to appdata.");
	std::string appdata(appdataBuffer);
	free(appdataBuffer);
	appdata += "\\Sluagh\\";
	std::ifstream in(appdata + "sluaghFile.sluagh", std::ios::in|std::ios::binary);
	

	/*Create copy of the player*/
	char roomMap[25][25]; room->GetMap(roomMap);
	if(in.is_open())
	{
		/*Create sluagh from data*/
		theSluagh = new PlayerUnit(in, 15, 15, roomMap);
		theSluagh->ToggleAsSluagh(true);
		in.close();

	}
	else
	{
		/*Copy the current player*/
		theSluagh = new PlayerUnit(&thePlayer->GetAllSkills()[0], nullptr,nullptr, 15, 15, roomMap);
		theSluagh->ToggleAsSluagh(true);
		auto playerItems = thePlayer->GetAllItems();
		for(int i = 0; i < 5; i++)
		{
			theSluagh->AddItem(Item::Copy(playerItems[i]), i);
		}
		

	}
	float rotX, rotY;
	thePlayer->GetRotation(rotX, rotY);
	theSluagh->UpdatePlayerRotation(rotX, rotY);
	theSluagh->SetZPosition(0.9f);
	theSluagh->PositionEntity(15.5f, 15.5f);
}

void SE::Gameplay::Sluagh::DecideActions(float dt, PlayerUnit::MovementInput &movement, PlayerUnit::ActionInput &action)
{
	StartProfile;
	ClearMap();

	CalculateSkillUtilities(dt);
	CalculateWeaponChangeUtilities(dt);
	CalculateUseItemUtilities(dt);
	CalculateMovementUtilities(dt);
	DecideActionInput(dt, action);
	DecideMovementInput(dt, movement, action);
	

	StopProfile;
}

float SE::Gameplay::Sluagh::UtilityForUsingACertainSkill(float dt, Skill & skillToCheck)
{
	StartProfile;
	if (skillToCheck.currentCooldown > 0.f)
		ProfileReturnConst(0.f);

	if (skillToCheck.atkType == DamageSources::DAMAGE_SOURCE_MELEE && distanceToPlayer < 2.f)
		ProfileReturnConst(0.f);

	float utilityValue = skillToCheck.skillDamage*2.5f/skillToCheck.cooldown;
	
	ProfileReturnConst(utilityValue);
}

float SE::Gameplay::Sluagh::UtilityForChangingWeapon(float dt, int & weaponToSwapFor)
{
	StartProfile;

	float maxUtility = 0.f;
	auto playerWeapon = thePlayer->GetCurrentItem();

	auto type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(playerWeapon, "Type", -1));
	Item::Weapon::Type playerWeaponType = Item::Weapon::Type::NONE;
	if (type != int32_t(-1))
		playerWeaponType = Item::Weapon::Type(type);
	
	auto sluaghWeapons = theSluagh->GetAllItems();

	for(int i = 0; i < 5; i++)
	{
		if((type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(sluaghWeapons[i], "Type", -1))) != -1)
		{
			float swapUtility = 0.f;
			auto sluaghWeaponType = Item::Weapon::Type(type);
			switch(sluaghWeaponType)
			{
			case Item::Weapon::Type::SWORD:
				if (distanceToPlayer > 2.5f)
				{
					swapUtility = 0.f;
					break;
				}
				else
					swapUtility += 1.f;
				if (playerWeaponType == Item::Weapon::Type::CROSSBOW || playerWeaponType == Item::Weapon::Type::WAND)
				{
					/*Disadvantage for the Sluagh*/
					swapUtility -= 0.5f;
				}
				swapUtility += 2.0f*std::get<int32_t>(CoreInit::managers.dataManager->GetValue(sluaghWeapons[i], "Damage", 0))/5.f;

				break;
				
			case Item::Weapon::Type::CROSSBOW: /*Fall through, only checking ranged*/
			case Item::Weapon::Type::WAND:
				if (distanceToPlayer < 2.5f)
				{
					swapUtility -= 1.f;
					break;
				}
				if (playerWeaponType == Item::Weapon::Type::SWORD)
				{
					/*Disadvantage for the Sluagh*/
					swapUtility += 0.5f;
				}
				swapUtility += 1.0f*std::get<int32_t>(CoreInit::managers.dataManager->GetValue(sluaghWeapons[i], "Damage", 0)) / 5.f;

				break;

			default: ;
			}

			if(maxUtility < swapUtility)
			{
				maxUtility = swapUtility;
				weaponToSwapFor = i;
			}
			
		}
	}


	ProfileReturnConst(maxUtility);
}

float SE::Gameplay::Sluagh::UtilityForUsingItem(float dt, int & item)
{
	StartProfile;
	
	auto sluaghWeapons = theSluagh->GetAllItems();
	float maxUtility = 0.f;
	int32_t type;
	for (int i = 0; i < 5; i++)
	{
		ItemType itemType;
		if ((type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(sluaghWeapons[i], "Type", -1))) != -1)
		{
			float swapUtility = 0.f;
			auto itemType = ItemType(type);
			switch (itemType)
			{
			case ItemType::CONSUMABLE:
				if(theSluagh->GetHealth() < theSluagh->GetMaxHealth()/2.f)
					swapUtility += 2.0f*std::get<int32_t>(CoreInit::managers.dataManager->GetValue(sluaghWeapons[i], "Health", 0)) / 5.f;
				break;

			default:
				break;
			}

			if (maxUtility < swapUtility)
			{
				maxUtility = swapUtility;
				item = i;
			}

		}
	}


	ProfileReturnConst(0.f);
}

float SE::Gameplay::Sluagh::UtilityForMoveInDirection(float dt, MovementDirection dir)
{
	StartProfile;
	
	float utilityValue = 0.f;

	float xPos = theSluagh->GetXPosition();
	float yPos = theSluagh->GetYPosition();
		

	auto type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(thePlayer->GetCurrentItem(), "Type", -1));
	Item::Weapon::Type playerWeaponType = Item::Weapon::Type::NONE;
	if (type != int32_t(-1))
		playerWeaponType = Item::Weapon::Type(type);

	type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(theSluagh->GetCurrentItem(), "Type", -1));
	Item::Weapon::Type sluaghWeaponType = Item::Weapon::Type::NONE;
	if (type != int32_t(-1))
		sluaghWeaponType = Item::Weapon::Type(type);

	PlayerUnit::MovementInput moveEvaluate(false, false, false, false, false, thePlayer->GetXPosition(), thePlayer->GetYPosition());

	switch(dir)
	{
	case MovementDirection::UP: moveEvaluate.upButton = true; break;
	case MovementDirection::DOWN: moveEvaluate.downButton = true; break;
	case MovementDirection::LEFT: moveEvaluate.leftButton = true; break;
	case MovementDirection::RIGHT: moveEvaluate.rightButton = true; break;
	default: ;
	}
	bool lineOfSightBeforeMove = room->CheckLineOfSightBetweenPoints(theSluagh->GetXPosition(), 
		theSluagh->GetYPosition(),
		thePlayer->GetXPosition(), 
		thePlayer->GetYPosition());
	theSluagh->UpdateMovement(dt, moveEvaluate);
	bool lineOfSightAfterMove = room->CheckLineOfSightBetweenPoints(theSluagh->GetXPosition(),
		theSluagh->GetYPosition(),
		thePlayer->GetXPosition(),
		thePlayer->GetYPosition());


	/*Check the weapon combinations, adapt the behaviour after that*/
	if(sluaghWeaponType == Item::Weapon::Type::SWORD && playerWeaponType == Item::Weapon::Type::SWORD) /*Both wields melee weapon -> Sluagh moves towards the player*/
	{
		float xDifference = theSluagh->GetXPosition() - thePlayer->GetXPosition();
		float yDifference = theSluagh->GetYPosition() - thePlayer->GetYPosition();
		float distAfterMove = sqrtf(xDifference*xDifference + yDifference*yDifference);

		if (distanceToPlayer > distAfterMove)
			utilityValue += abs(distanceToPlayer - distAfterMove)*20.f;

		if(lineOfSightAfterMove)
			utilityValue += 1.f;

		if (distAfterMove > 2.0f && !lineOfSightAfterMove)
			utilityValue = 0.f;
		
	}
	else if((sluaghWeaponType == Item::Weapon::Type::CROSSBOW || sluaghWeaponType == Item::Weapon::Type::WAND) && playerWeaponType == Item::Weapon::Type::SWORD) /*Player Melee, Sluagh Ranged*/
	{
		float xDifference = theSluagh->GetXPosition() - thePlayer->GetXPosition();
		float yDifference = theSluagh->GetYPosition() - thePlayer->GetYPosition();
		float distAfterMove = sqrtf(xDifference*xDifference + yDifference*yDifference);
		
		if (distanceToPlayer < distAfterMove && lineOfSightAfterMove)
			utilityValue += abs(distanceToPlayer - distAfterMove)*20.f;
		else if (distAfterMove < 2.0f)
			utilityValue -= 0.5f;

		if (lineOfSightBeforeMove && !lineOfSightAfterMove) /*Moved behing object... not good*/
			utilityValue -= 0.5f;
		else if (!lineOfSightBeforeMove && lineOfSightAfterMove) /*Moved so we could see, pretty good*/
			utilityValue += 0.5f;
		else if (!lineOfSightAfterMove)
			utilityValue -= 0.5f;
		else
			utilityValue += 0.75f;
	}
	else if(sluaghWeaponType == Item::Weapon::Type::SWORD && (playerWeaponType == Item::Weapon::Type::CROSSBOW || playerWeaponType == Item::Weapon::Type::WAND)) /*Player Ranged, Sluagh Melee*/
	{
		float xDifference = theSluagh->GetXPosition() - thePlayer->GetXPosition();
		float yDifference = theSluagh->GetYPosition() - thePlayer->GetYPosition();
		float distAfterMove = sqrtf(xDifference*xDifference + yDifference*yDifference);

		if (distanceToPlayer > distAfterMove)
			utilityValue += abs(distanceToPlayer - distAfterMove)*20.f;

		if (lineOfSightAfterMove)
			utilityValue += 1.f;

		if (distAfterMove > 2.0f)
			utilityValue = 0.f;
	}
	else if(playerWeaponType == Item::Weapon::Type::NONE) /*CONFUSED SCREAMING*/
	{
		float xDifference = theSluagh->GetXPosition() - thePlayer->GetXPosition();
		float yDifference = theSluagh->GetYPosition() - thePlayer->GetYPosition();
		float distAfterMove = sqrtf(xDifference*xDifference + yDifference*yDifference);

		if (distanceToPlayer > distAfterMove)
			utilityValue += abs(distanceToPlayer - distAfterMove)*20.f;

		if (lineOfSightAfterMove)
			utilityValue += 1.f;

		if (distAfterMove > 2.0f)
			utilityValue = 0.f;
	}
	else /*Player Ranged, Sluagh Ranged*/
	{
		float xDifference = theSluagh->GetXPosition() - thePlayer->GetXPosition();
		float yDifference = theSluagh->GetYPosition() - thePlayer->GetYPosition();
		float distAfterMove = sqrtf(xDifference*xDifference + yDifference*yDifference);


		if (distanceToPlayer < distAfterMove && lineOfSightAfterMove)
			utilityValue += abs(distanceToPlayer - distAfterMove)*20.f;
		else if (distAfterMove < 2.0f)
			utilityValue -= 0.5f;

		if (lineOfSightBeforeMove && !lineOfSightAfterMove) /*Moved behing object... not good*/
			utilityValue -= 0.5f;
		else if (!lineOfSightBeforeMove && lineOfSightAfterMove) /*Moved so we could see, pretty good*/
			utilityValue += 0.5f;
		else if (!lineOfSightAfterMove)
			utilityValue -= 0.5f;
		else
			utilityValue += 0.75f;

		if(abs(xPos-thePlayer->GetXPosition()) < abs(yPos-thePlayer->GetYPosition()))
		{
			if (abs(yDifference) < abs(yPos - thePlayer->GetYPosition()))
				utilityValue += 0.75f;
		}
		else
		{
			if (abs(xDifference) < abs(xPos - thePlayer->GetXPosition()))
				utilityValue += 0.75f;			
		}
	}



	theSluagh->PositionEntity(xPos, yPos);

	ProfileReturnConst(utilityValue);
}

void SE::Gameplay::Sluagh::ClearMap()
{
	utilityMap.clear();
}

void SE::Gameplay::Sluagh::CalculateSkillUtilities(float dt)
{
		float xDist = thePlayer->GetXPosition() - theSluagh->GetXPosition();
	float yDist = thePlayer->GetYPosition() - theSluagh->GetYPosition();
	distanceToPlayer = sqrtf(xDist*xDist + yDist*yDist);

	auto skills = theSluagh->GetAllSkills();
	utilityMap[UtilityMapEnum::SKILL1] = UtilityForUsingACertainSkill(dt, skills[0]);
	utilityMap[UtilityMapEnum::SKILL2] = UtilityForUsingACertainSkill(dt, skills[1]);

}

void SE::Gameplay::Sluagh::CalculateWeaponChangeUtilities(float dt)
{
	int weaponSlot = -1;
	float weaponUtility = UtilityForChangingWeapon(dt, weaponSlot);

	switch (weaponSlot)
	{
	case 0:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_1] = weaponUtility;
		break;
	case 1:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_2] = weaponUtility;
		break;
	case 2:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_3] = weaponUtility;
		break;
	case 3:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_4] = weaponUtility;
		break;
	case 4:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_5] = weaponUtility;
		break;
	default:

		break;
	}

}

void SE::Gameplay::Sluagh::CalculateUseItemUtilities(float dt)
{
	int itemSlot = -1;
	float itemUtility = UtilityForUsingItem(dt, itemSlot);

	switch (itemSlot)
	{
	case 0:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_1] = itemUtility;
		break;
	case 1:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_2] = itemUtility;
		break;
	case 2:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_3] = itemUtility;
		break;
	case 3:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_4] = itemUtility;
		break;
	case 4:
		utilityMap[UtilityMapEnum::WEAPON_SLOT_5] = itemUtility;
		break;
	default:

		break;
	}
}

void SE::Gameplay::Sluagh::CalculateMovementUtilities(float dt)
{
	utilityMap[UtilityMapEnum::MOVE_UP] = UtilityForMoveInDirection(dt, MovementDirection::UP);
	utilityMap[UtilityMapEnum::MOVE_DOWN] = UtilityForMoveInDirection(dt, MovementDirection::DOWN);
	utilityMap[UtilityMapEnum::MOVE_LEFT] = UtilityForMoveInDirection(dt, MovementDirection::LEFT);
	utilityMap[UtilityMapEnum::MOVE_RIGHT] = UtilityForMoveInDirection(dt, MovementDirection::RIGHT);

}

void SE::Gameplay::Sluagh::DecideActionInput(float dt, PlayerUnit::ActionInput& action)
{

	float maxUtility = -1.f;
	UtilityMapEnum choice;

	int currentEquipment = 0;
	auto currentEquipmentEntity = theSluagh->GetCurrentItem();
	auto allEquipments = theSluagh->GetAllItems();

	for (int i = 0; i < 5; i++)
	{
		if (currentEquipmentEntity == allEquipments[i])
		{
			currentEquipment = i;
			break;
		}
	}


	for (auto utility : utilityMap)
	{
		if (maxUtility < utility.second && int(utility.first) < 7)
		{
			choice = utility.first;
			maxUtility = utility.second;
		}
	}

	if (maxUtility <= 0.f) /*No choice*/
		ProfileReturnVoid;

	switch (choice)
	{
	case UtilityMapEnum::SKILL1: action.skill1Button = true; break;
	case UtilityMapEnum::SKILL2: action.skill2Button = true; break;
	case UtilityMapEnum::WEAPON_SLOT_1:
		if (currentEquipment == 0)
		{
			action.actionButton = true;
		}
		else
		{
			action.one = true;
		}
		break;
	case UtilityMapEnum::WEAPON_SLOT_2:
		if (currentEquipment == 1)
		{
			action.actionButton = true;
		}
		else
		{
			action.two = true;
		}
		break;
	case UtilityMapEnum::WEAPON_SLOT_3:
		if (currentEquipment == 2)
		{
			action.actionButton = true;
		}
		else
		{
			action.three = true;
		}
		break;
	case UtilityMapEnum::WEAPON_SLOT_4:
		if (currentEquipment == 3)
		{
			action.actionButton = true;
		}
		else
		{
			action.four = true;
		}
		break;
	case UtilityMapEnum::WEAPON_SLOT_5:
		if (currentEquipment == 4)
		{
			action.actionButton = true;
		}
		else
		{
			action.five = true;
		}
		break;


	default:;
	}
}

void SE::Gameplay::Sluagh::DecideMovementInput(float dt, PlayerUnit::MovementInput& movement,
	PlayerUnit::ActionInput& action)
{
	bool moved = false;
	if (0.50f < utilityMap[UtilityMapEnum::MOVE_UP])
	{
		movement.upButton = true;
		moved = true;
	}
	if (0.50f < utilityMap[UtilityMapEnum::MOVE_LEFT])
	{
		movement.leftButton = true;
		moved = true;
	}
	if (0.50f < utilityMap[UtilityMapEnum::MOVE_DOWN])
	{
		movement.downButton = true;
		moved = true;
	}
	if (0.50f < utilityMap[UtilityMapEnum::MOVE_RIGHT])
	{
		movement.rightButton = true;
		moved = true;
	}

	if(moved)
	{
		commitmentTime += 5.f;
		previousMovement = movement;
	}
	else
	{
		movement = previousMovement;
		commitmentTime -= dt;
		if (commitmentTime < 0.f)
			ProfileReturnVoid;
	}

}
