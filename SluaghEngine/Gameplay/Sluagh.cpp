#include "..\includes\Gameplay\Sluagh.h"
#include "ProjectileData.h"
#include <Profiler.h>
#include "CoreInit.h"
#include "SluaghRoom.h"
#include "Items.h"

SE::Gameplay::Sluagh::Sluagh(PlayerUnit * thePlayer, SluaghRoom* room)
	:thePlayer(thePlayer)
{
	char map[25][25];
	room->GetMap(map);
	theSluagh = new PlayerUnit(&thePlayer->GetAllSkills()[0], nullptr, 15, 15, map);
	theSluagh->ToggleAsSluagh(true);

	float rotX, rotY;
	thePlayer->GetRotation(rotX, rotY);
	theSluagh->UpdatePlayerRotation(rotX, rotY);
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
}

void SE::Gameplay::Sluagh::DecideActions(float dt, PlayerUnit::MovementInput &movement, PlayerUnit::ActionInput &action)
{
	StartProfile;


	float xDist = thePlayer->GetXPosition() - theSluagh->GetXPosition();
	float yDist = thePlayer->GetYPosition() - theSluagh->GetYPosition();

	distanceToPlayer = sqrtf(xDist*xDist + yDist*yDist);

	auto skills = theSluagh->GetAllSkills();
	utilityMap[UtilityMapEnum::SKILL1] = UtilityForUsingACertainSkill(dt, skills[0]);
	utilityMap[UtilityMapEnum::SKILL2] = UtilityForUsingACertainSkill(dt, skills[1]);

	int weaponSlot = -1;
	float weaponUtility = UtilityForChangingWeapon(dt, weaponSlot);

	switch(weaponSlot)
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

	utilityMap[UtilityMapEnum::MOVE_UP] = UtilityForMoveInDirection(dt, MovementDirection::UP);
	utilityMap[UtilityMapEnum::MOVE_DOWN] = UtilityForMoveInDirection(dt, MovementDirection::DOWN);
	utilityMap[UtilityMapEnum::MOVE_LEFT] = UtilityForMoveInDirection(dt, MovementDirection::LEFT);
	utilityMap[UtilityMapEnum::MOVE_RIGHT] = UtilityForMoveInDirection(dt, MovementDirection::RIGHT);

	float maxUtility = -1.f;
	UtilityMapEnum choice;

	int currentEquipment = 0;
	auto currentEquipmentEntity = theSluagh->GetCurrentItem();
	auto allEquipments = theSluagh->GetAllItems();

	for(int i = 0; i < 5; i++)
	{
		if (currentEquipmentEntity == allEquipments[i])
		{
			currentEquipment = i;
			break;
		}
	}
	

	for(auto utility : utilityMap)
	{
		if(maxUtility < utility.second && int(utility.first) < 7)
		{
			choice = utility.first;
			maxUtility = utility.second;
		}
	}

	if (maxUtility <= 0.f) /*No choice*/
		ProfileReturnVoid;

	switch(choice)
	{
	case UtilityMapEnum::SKILL1: action.skill1Button = true; break;
	case UtilityMapEnum::SKILL2: action.skill2Button = true; break;
	case UtilityMapEnum::WEAPON_SLOT_1: 
		if(currentEquipment == 0)
		{
			action.actionButton = true;
		}
		else
		{
		}
		break;
	case UtilityMapEnum::WEAPON_SLOT_2: break;
	case UtilityMapEnum::WEAPON_SLOT_3: break;
	case UtilityMapEnum::WEAPON_SLOT_4: break;
	case UtilityMapEnum::WEAPON_SLOT_5: break;
	
	
	default: ;
	}

	if (.50f < utilityMap[UtilityMapEnum::MOVE_UP])
		movement.upButton = true;
	if (0.50f < utilityMap[UtilityMapEnum::MOVE_LEFT])
		movement.leftButton = true;
	if (0.50f < utilityMap[UtilityMapEnum::MOVE_DOWN])
		movement.downButton = true;
	if (0.50f < utilityMap[UtilityMapEnum::MOVE_RIGHT])
		movement.rightButton = true;
		


	StopProfile;
}

float SE::Gameplay::Sluagh::UtilityForUsingACertainSkill(float dt, Skill & skillToCheck)
{
	StartProfile;
	if (skillToCheck.currentCooldown > 0.f)
		ProfileReturnConst(0.f);

	if (skillToCheck.atkType == DamageSources::DAMAGE_SOURCE_MELEE && distanceToPlayer < 2.f)
		ProfileReturnConst(0.f);

	float utilityValue = skillToCheck.skillDamage*5.f/skillToCheck.cooldown;
	
	ProfileReturnConst(utilityValue);
}

float SE::Gameplay::Sluagh::UtilityForChangingWeapon(float dt, int & weaponToSwapFor)
{
	StartProfile;

	float maxUtility = 0.f;
	auto playerWeapon = thePlayer->GetCurrentItem();

	auto type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(playerWeapon, "Type", -1));
	WeaponType playerWeaponType = WeaponType::NONE;
	if (type != int32_t(-1))
		playerWeaponType = WeaponType(type);
	
	auto sluaghWeapons = theSluagh->GetAllItems();

	for(int i = 0; i < 5; i++)
	{
		if((type = std::get<int32_t>(CoreInit::managers.dataManager->GetValue(sluaghWeapons[i], "Type", -1))) != -1)
		{
			float swapUtility = 0.f;
			auto sluaghWeaponType = WeaponType(type);
			switch(sluaghWeaponType)
			{
			case WeaponType::SWORD:
				if (distanceToPlayer > 2.5f)
				{
					swapUtility = 0.f;
					break;
				}
				else
					swapUtility += 1.f;
				if (playerWeaponType == WeaponType::CROSSBOW || playerWeaponType == WeaponType::WAND)
				{
					/*Disadvantage for the Sluagh*/
					swapUtility -= 0.5f;
				}
				swapUtility += 2.0f*std::get<int32_t>(CoreInit::managers.dataManager->GetValue(sluaghWeapons[i], "Damage", 0))/5.f;

				break;
				
			case WeaponType::CROSSBOW: /*Fall through, only checking ranged*/
			case WeaponType::WAND:
				if (distanceToPlayer < 2.5f)
				{
					swapUtility -= 1.f;
					break;
				}
				if (playerWeaponType == WeaponType::SWORD)
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

	

	ProfileReturnConst(0.f);
}

float SE::Gameplay::Sluagh::UtilityForMoveInDirection(float dt, MovementDirection dir)
{
	StartProfile;
	
	float utilityValue = 0.f;

	float xPos = theSluagh->GetXPosition();
	float yPos = theSluagh->GetYPosition();
		
	auto SluaghCurrentWeapon = theSluagh->GetCurrentItem();
	auto PlayerCurrentWeapon = thePlayer->GetCurrentItem();

	PlayerUnit::MovementInput moveEvaluate(false, false, false, false, false, thePlayer->GetXPosition(), thePlayer->GetYPosition());

	switch(dir)
	{
	case MovementDirection::UP: moveEvaluate.upButton = true; break;
	case MovementDirection::DOWN: moveEvaluate.downButton = true; break;
	case MovementDirection::LEFT: moveEvaluate.leftButton = true; break;
	case MovementDirection::RIGHT: moveEvaluate.rightButton = true; break;
	default: ;
	}

	theSluagh->UpdateMovement(dt, moveEvaluate);



	/*Check the weapon combinations, adapt the behaviour after that*/
	if(true) /*Both wields melee weapon -> Sluagh moves towards the player*/
	{
		float xDifference = theSluagh->GetXPosition() - thePlayer->GetXPosition();
		float yDifference = theSluagh->GetYPosition() - thePlayer->GetYPosition();
		float distAfterMove = sqrtf(xDifference*xDifference + yDifference*yDifference);

		if (distanceToPlayer > distAfterMove)
			utilityValue += abs(distanceToPlayer - distAfterMove)*20.f;


		if (distAfterMove < 2.0f)
			utilityValue = 0.f;
		
	}
	else if(true) /*Player Melee, Sluagh Ranged*/
	{
		
	}
	else if(true) /*Player Ranged, Sluagh Melee*/
	{
		
	}
	else /*Player Ranged, Sluagh Ranged*/
	{
		
	}



	theSluagh->PositionEntity(xPos, yPos);

	ProfileReturnConst(utilityValue);
}
