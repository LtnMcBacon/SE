#include "ForestTrainer.h"
#include "PlayerUnit.h"
#include "EnemyUnit.h"
#include <Profiler.h>

void SE::Gameplay::ForestTrainer::TrainingExample(ForestStruct& frameData, bool startOfCombat)
{
	StartProfile;
	if (startOfCombat)
		playerHealthPreviousFrame = frameData.thePlayer->GetHealth();
	AddPreCalculatedFeatures(frameData);
	AddPlayerRelatedFeatures(frameData.thePlayer);
	AddClosestEnemyRelatedFeatures(frameData.closestUnitToPlayer, frameData.thePlayer);
	AddClosestEnemyToMouseRelatedFeatures(frameData.closestUnitToMouse, frameData.thePlayer);

	forest.AddTrainingExample(frameFeatures, frameData.attack, frameData.move);

	playerHealthPreviousFrame = frameData.thePlayer->GetHealth();

	ProfileReturnVoid;
}

void SE::Gameplay::ForestTrainer::DataForSluaghCalculations(ForestStruct& frameData, bool startOfCombat)
{
	StartProfile;
	if (startOfCombat)
		playerHealthPreviousFrame = frameData.thePlayer->GetHealth();

	featureCounter = 0;

	AddPreCalculatedFeatures(frameData);
	AddPlayerRelatedFeatures(frameData.closestUnitToMouse);
	AddClosestEnemyRelatedFeatures(frameData.thePlayer, frameData.closestUnitToPlayer);
	AddClosestEnemyToMouseRelatedFeatures(frameData.thePlayer, frameData.closestUnitToPlayer);

	playerHealthPreviousFrame = frameData.thePlayer->GetHealth();


	ProfileReturnVoid;
}

SE::Gameplay::PlayerAttackActions SE::Gameplay::ForestTrainer::GetSlaughAttack()
{
	StartProfile;
	PlayerAttackActions attack;

	forest.GetAnswer(frameFeatures, attack);

	ProfileReturnConst(attack);
}

SE::Gameplay::PlayerMovementActions SE::Gameplay::ForestTrainer::GetSlaughMovement()
{
	StartProfile;
	PlayerMovementActions move;

	forest.GetAnswer(frameFeatures, move);

	ProfileReturnConst(move);

}

void SE::Gameplay::ForestTrainer::AddPreCalculatedFeatures(ForestStruct& frameData)
{
	StartProfile;
	frameFeatures[featureCounter++]->ChangeAnswer(frameData.distanceToClosestWallX);
	frameFeatures[featureCounter++]->ChangeAnswer(frameData.distanceToClosestWallY);
	frameFeatures[featureCounter++]->ChangeAnswer(frameData.closestEnemyToPlayerBlockedByWall);
	frameFeatures[featureCounter++]->ChangeAnswer(frameData.closestEnemyToMouseBlockedByWall);

	int close = 0;
	int middleGround = 0;
	int noThreat = 0;

	for(auto distance : frameData.distanceToAllEnemies)
	{
		if (distance <= closeDistance)
			close++;
		else if (distance <= middleGroundDistance)
			middleGround++;
		else
			noThreat++;
	}

	frameFeatures[featureCounter++]->ChangeAnswer(close, enemyDistanceSplits, close / enemyDistanceSplits);
	frameFeatures[featureCounter++]->ChangeAnswer(middleGround, enemyDistanceSplits, middleGround / enemyDistanceSplits);
	frameFeatures[featureCounter++]->ChangeAnswer(noThreat, enemyDistanceSplits, noThreat / enemyDistanceSplits);


	ProfileReturnVoid;
}

void SE::Gameplay::ForestTrainer::AddPlayerRelatedFeatures(GameUnit* player)
{
	StartProfile;
	frameFeatures[featureCounter++]->ChangeAnswer(player->GetHealth());
	frameFeatures[featureCounter++]->ChangeAnswer(playerHealthPreviousFrame - player->GetHealth());
	/*Weapons and armor go here*/



	ProfileReturnVoid;
}

void SE::Gameplay::ForestTrainer::AddClosestEnemyRelatedFeatures(GameUnit* enemy, GameUnit* player)
{
	StartProfile;
	float xDiff = player->GetXPosition() - enemy->GetXPosition();
	float yDiff = player->GetYPosition() - enemy->GetYPosition();
	frameFeatures[featureCounter++]->ChangeAnswer(xDiff);
	frameFeatures[featureCounter++]->ChangeAnswer(yDiff);
	frameFeatures[featureCounter++]->ChangeAnswer(sqrtf(xDiff*xDiff + yDiff*yDiff));
	ProfileReturnVoid;
}

void SE::Gameplay::ForestTrainer::AddClosestEnemyToMouseRelatedFeatures(GameUnit* enemy, GameUnit* player)
{
	StartProfile;
	float xDiff = player->GetXPosition() - enemy->GetXPosition();
	float yDiff = player->GetYPosition() - enemy->GetYPosition();
	frameFeatures[featureCounter++]->ChangeAnswer(xDiff);
	frameFeatures[featureCounter++]->ChangeAnswer(yDiff);
	frameFeatures[featureCounter++]->ChangeAnswer(sqrtf(xDiff*xDiff + yDiff*yDiff));
	ProfileReturnVoid;
}

SE::Gameplay::ForestTrainer::ForestTrainer()
{
	StartProfile;
	playerHealthPreviousFrame = 0.f;
	featureCounter = 0;
	forest.SetNumberOfPossibleAnswersForDecisionTrees({
		int(PlayerAttackActions::PLAYER_ATTACK_ENUM_SIZE),
		int(PlayerMovementActions::PLAYER_MOVEMENT_ENUM_SIZE)
	});


	frameFeatures.push_back(new FloatFeature(0.f));
	frameFeatures.push_back(new FloatFeature(0.f));
	frameFeatures.push_back(new BoolFeature(false));
	frameFeatures.push_back(new BoolFeature(false));
	frameFeatures.push_back(new IntFeature(0, enemyDistanceSplits, 0));
	frameFeatures.push_back(new IntFeature(0, enemyDistanceSplits, 0));
	frameFeatures.push_back(new IntFeature(0, enemyDistanceSplits, 0));
	frameFeatures.push_back(new FloatFeature(0.f));
	frameFeatures.push_back(new FloatFeature(0.f));
	frameFeatures.push_back(new FloatFeature(0.f));
	frameFeatures.push_back(new FloatFeature(0.f));
	frameFeatures.push_back(new FloatFeature(0.f));
	frameFeatures.push_back(new FloatFeature(0.f));
	frameFeatures.push_back(new FloatFeature(0.f));
	frameFeatures.push_back(new FloatFeature(0.f));
	StopProfile;
}

SE::Gameplay::ForestTrainer::~ForestTrainer()
{
	for (auto feature : frameFeatures)
		delete feature;
}
