#include "EnemyUnit.h"
#include <Profiler.h>
#include "Flowfield.h"
#include "CoreInit.h"
#include <Gameplay/EnemyBlackboard.h>
#include <Gameplay/BehaviouralTree.h>

void SE::Gameplay::EnemyUnit::ResolveEvents(float dt)
{
	StartProfile;
	// only basic at the moment
	myBlackboard->activeBane = Banes::CONDITIONAL_BANES_NONE;
	myBlackboard->activeCondition = Boons::CONDITIONAL_BOONS_NONE;
	myBlackboard->conditionAmmount = 0.f;
	if (!myBlackboard->invurnerable)
	{
		auto ph = health;
		for (int i = 0; i < DamageEventVector.size(); i++)
		{
			float damageAmount = DamageEventVector[i].amount;
			switch(DamageEventVector[i].type)
			{
			case DamageType::PHYSICAL: damageAmount*=(1 - newStat.physicalResistance); break;
			case DamageType::FIRE: damageAmount*=(1 - newStat.fireResistance); break;
			case DamageType::WATER: damageAmount*=(1 - newStat.waterResistance); break;
			case DamageType::NATURE: damageAmount*=(1 - newStat.natureResistance); break;
			case DamageType::RANGED: damageAmount*=(1 - newStat.physicalResistance); break;
			case DamageType::MAGIC: damageAmount*=(1 - newStat.magicResistance); break;
			default: ;
			}
			this->health -= damageAmount;
		}


		if (ph > health)
		{
			
			auto ent = CoreInit::managers.entityManager->Create();
			CoreInit::managers.transformManager->Create(ent, CoreInit::managers.transformManager->GetPosition(unitEntity));

			// Blood spatter
			auto bs = CoreInit::managers.entityManager->Create();
			auto p = CoreInit::managers.transformManager->GetPosition(unitEntity);
			p.y = 0;
			CoreInit::managers.transformManager->Create(bs, p, { DirectX::XM_PIDIV2, 0,0 }, { 0.4f,0.4f, 0.05f });

			Core::DecalCreateInfo ci;
			ci.textureName = "bloodSpatt.png";
			CoreInit::managers.decalManager->Create(bs, ci);
			CoreInit::managers.eventManager->SetLifetime(bs, 20);
			CoreInit::managers.eventManager->ToggleVisible(bs, true);

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
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_DAMAGE;
					newStat.meleeMultiplier += baseStat.meleeMultiplier * ConditionEventVector[i].effectValue;
					break;
				case Boons::CONDITIONAL_BOONS_STUN:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_STUN;
					break;
				case Boons::CONDITIONAL_BOONS_ROOT:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_ROOT;
					break;
				case Boons::CONDITIONAL_BOONS_PROTECTION:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_PROTECTION;
					this->newStat.physicalResistance += this->baseStat.physicalResistance * ConditionEventVector[i].effectValue;
					break;
				case Boons::CONDITIONAL_BOONS_PHYSICAL_RESISTANCE:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_PHYSICAL_RESISTANCE;
					this->newStat.physicalResistance += this->baseStat.physicalResistance * ConditionEventVector[i].effectValue;
					break;
				case Boons::CONDITIONAL_BOONS_MAGICAL_RESISTANCE:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_MAGICAL_RESISTANCE;
					this->newStat.magicResistance += this->baseStat.magicResistance * ConditionEventVector[i].effectValue;
					break;
				case Boons::CONDITIONAL_BOONS_FIRE_RESISTANCE:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_FIRE_RESISTANCE;
					this->newStat.fireResistance += this->baseStat.fireResistance * ConditionEventVector[i].effectValue;
					break;
				case Boons::CONDITIONAL_BOONS_WATER_RESISTANCE:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_WATER_RESISTANCE;
					this->newStat.waterResistance += this->baseStat.waterResistance * ConditionEventVector[i].effectValue;
					break;
				case Boons::CONDITIONAL_BOONS_NATURE_RESISTANCE:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_NATURE_RESISTANCE;
					this->newStat.natureResistance += this->baseStat.natureResistance * ConditionEventVector[i].effectValue;
					break;
				case Boons::CONDITIONAL_BOONS_CASTSPEED:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_CASTSPEED;
					break;
				case Boons::CONDITIONAL_BOONS_SWIFTNESS:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_SWIFTNESS;
					this->newStat.movementSpeed += this->baseStat.movementSpeed * ConditionEventVector[i].effectValue;
					break;
				case Boons::CONDITIONAL_BOONS_SLOW:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_SLOW;
					if (myBlackboard->conditionAmmount < ConditionEventVector[i].effectValue)
						myBlackboard->conditionAmmount = ConditionEventVector[i].effectValue;
					break;
				case Boons::CONDITIONAL_BOONS_INVULNERABILITY:
					myBlackboard->activeCondition |= Boons::CONDITIONAL_BOONS_INVULNERABILITY;
					break;
				}
			}
			else
			{
				switch (ConditionEventVector[i].bane)
				{
				case Banes::CONDITIONAL_BANES_NONE:
				myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_NONE;
				break;
				case Banes::CONDITIONAL_BANES_DAMAGE:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_DAMAGE;
					this->newStat.meleeMultiplier -= baseStat.meleeMultiplier * ConditionEventVector[i].effectValue;
					break;
				case Banes::CONDITIONAL_BANES_STUN:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_STUN;
					break;
				case Banes::CONDITIONAL_BANES_ROOT:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_ROOT;
					break;
				case Banes::CONDITIONAL_BANES_BLOODLETTING:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_BLOODLETTING;
					break;
				case Banes::CONDITIONAL_BANES_UNCOVER:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_UNCOVER;
					break;
				case Banes::CONDITIONAL_BANES_PHYSICAL_WEAKNESS:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_PHYSICAL_WEAKNESS;
					this->newStat.physicalResistance -= this->baseStat.physicalResistance * ConditionEventVector[i].effectValue;
					break;
				case Banes::CONDITIONAL_BANES_MAGICAL_WEAKNESS:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_MAGICAL_WEAKNESS;
					this->newStat.magicResistance -= this->baseStat.magicResistance * ConditionEventVector[i].effectValue;
					break;
				case Banes::CONDITIONAL_BANES_FIRE_WEAKNESS:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_FIRE_WEAKNESS;
					this->newStat.fireResistance -= this->baseStat.fireResistance * ConditionEventVector[i].effectValue;
					break;
				case Banes::CONDITIONAL_BANES_WATER_WEAKNESS:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_WATER_WEAKNESS;
					this->newStat.waterResistance -= this->baseStat.waterResistance * ConditionEventVector[i].effectValue;
					break;
				case Banes::CONDITIONAL_BANES_NATURE_WEAKNESS:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_NATURE_WEAKNESS;
					this->newStat.natureResistance += this->baseStat.natureResistance * ConditionEventVector[i].effectValue;
					break;
				case Banes::CONDITIONAL_BANES_SLOW:
					myBlackboard->activeBane |= Banes::CONDITIONAL_BANES_SLOW;
					this->newStat.movementSpeed += this->baseStat.movementSpeed * ConditionEventVector[i].effectValue;
					break;
				}
			}
			if (ConditionEventVector[i].duration > 0.f)
			{
				NextFrameCondition.push_back(ConditionEventVector[i]);
			}
		}
	}

	for (int i = 0; i < HealingEventVector.size(); i++)
	{
		this->health += HealingEventVector[i].amount;
	}

	if (this->health > this->maxHealth)
		this->health = this->maxHealth;

	DamageEventVector.clear();
	ConditionEventVector.clear();
	HealingEventVector.clear();


	ProfileReturnVoid;

}

void SE::Gameplay::EnemyUnit::DecideAction(float dt)
{
	StartProfile;
	/*
	* Code body
	*/
	entityAction = EnemyActions::ENEMY_ACTION_MOVE;
	
	if (true/*force[0] == 0.0f && force[1] == 0.0f*/)
	{
		myBlackboard->extents = 0.25;
		myBlackboard->ownerPointer = this;
		myBlackboard->checkedThisFrame = false;
		
		myBehaviouralTree->Tick();
	}
	
	ProfileReturnVoid;
}

void SE::Gameplay::EnemyUnit::PerformAction(float dt)
{
	StartProfile;
	/*
	* Code body
	*/
	ProfileReturnVoid;
}

void SE::Gameplay::EnemyUnit::Update(float dt)
{
	StartProfile;
	if (health > 0.f)
	{
		/*
		* Code body
		*/

		ClearNewStats();
		calcBaseStrChanges();
		calcBaseAgiChanges();
		calcBaseWhiChanges();
		ResolveEvents(dt);
		DecideAction(dt);
		PerformAction(dt);
		ClearHealingEvents();
		ClearDamageEvents();
		ClearConditionEvents();
	}
	ProfileReturnVoid;
}

SE::Gameplay::EnemyUnit::EnemyUnit(EnemyType myType, const FlowField* roomFlowField, float xPos, float yPos, float maxHealth) :
	GameUnit(xPos, yPos, maxHealth),
	extraSampleCoords{xPos, yPos},
	previousMovement{0,0},
	sample(0),
	myType(myType)
{
	this->maxHealth = maxHealth;
	extents = 0.25f; /*Should not be hardcoded! Obviously*/
	radius = sqrt(extents*extents + extents*extents);

}

SE::Gameplay::EnemyUnit::~EnemyUnit()
{
	if (auto weapon = std::get_if<Core::Entity>(&CoreInit::managers.dataManager->GetValue(unitEntity, "Weapon", false)))
	{
		CoreInit::managers.entityManager->DestroyNow(*weapon);
	}
	CoreInit::managers.entityManager->Destroy(this->unitEntity);
	/*
	* Code body
	*/
	delete myBehaviouralTree;
	delete myBlackboard;
}

