#ifndef FOREST_TRAINER_H
#define FOREST_TRAINER_H

#include "PlayerPossibleActions.h"
#include "RandomForest.h"

namespace SE
{
	namespace Gameplay
	{
		class PlayerUnit;
		class EnemyUnit;
		class GameUnit;
		class ForestTrainer
		{
		public:
			struct ForestStruct
			{
				PlayerUnit* thePlayer;
				EnemyUnit* closestUnitToMouse;
				EnemyUnit* closestUnitToPlayer;
				bool closestEnemyToPlayerBlockedByWall;
				bool closestEnemyToMouseBlockedByWall;
				float mouseXPos; /*Sluagh Y Pos when used for the Sluagh*/
				float mouseYPos; /*Sluagh Y Pos when used for the Sluagh*/
				float distanceToClosestWallX;
				float distanceToClosestWallY;
				std::vector<float> distanceToAllEnemies;
				PlayerAttackActions attack;
				PlayerMovementActions move;
			};
			void TrainingExample(ForestStruct &frameData, bool startOfCombat = false);
			void DataForSluaghCalculations(ForestStruct &frameData, bool startOfCombat = false);
			PlayerAttackActions GetSlaughAttack();
			PlayerMovementActions GetSlaughMovement();
		private:
			DecisionTree::RandomForest<PlayerAttackActions, PlayerMovementActions> forest;

			float playerHealthPreviousFrame;

			class BoolFeature : public DecisionTree::IFeature
			{
			public:
				BoolFeature(bool answer)
					: answer(answer)
				{
				};
				int SplitingPoints() override { return 2; };
				int SplitingGroupForSample() override { return answer; };
				float ConvertToFloat() override { return float(answer); };
				void ChangeAnswer(bool newAnswer) { answer = newAnswer; };
				BoolFeature* copy() override {
					return new BoolFeature(answer);
				};

				bool answer;
			};

			class IntFeature : public DecisionTree::IFeature
			{
			public:
				IntFeature(int answer, int numberOfSplittingPoints, int splittingPoint)
					: answer(answer), numberOfSplittingPoints(numberOfSplittingPoints),
					mySplittingPoint(splittingPoint)
				{
				};
				int SplitingPoints() override { return numberOfSplittingPoints; };
				int SplitingGroupForSample() override { return mySplittingPoint; };
				float ConvertToFloat() override { return float(answer); };
				void ChangeAnswer(int newAnswer) { answer = newAnswer; };
				IntFeature* copy() override {
					return new IntFeature(
						answer, numberOfSplittingPoints, mySplittingPoint);
				};

				int answer;
				int numberOfSplittingPoints;
				int mySplittingPoint;
			};

			class FloatFeature : public Gameplay::DecisionTree::IFeature
			{
			public:
				FloatFeature(float answer) : answer(answer)
				{
				};
				int SplitingPoints() override { return -1; };
				int SplitingGroupForSample() override { return int(answer); };
				float ConvertToFloat() override { return answer; };
				void ChangeAnswer(float newAnswer) { answer = newAnswer; };
				FloatFeature* copy() override { return new FloatFeature(answer); }
				float answer;
			};

			const float closeDistance = sqrtf(2.f);
			const float middleGroundDistance = sqrtf(6.0f);
			const int enemyDistanceSplits = 5;
			void AddPreCalculatedFeatures(ForestStruct &frameData);
			void AddPlayerRelatedFeatures(GameUnit* player);
			void AddClosestEnemyRelatedFeatures(GameUnit* enemy, GameUnit* player);
			void AddClosestEnemyToMouseRelatedFeatures(GameUnit* enemy, GameUnit* player);

			std::vector<DecisionTree::IFeature*> frameFeatures;
			int featureCounter = 0;


		public:
			ForestTrainer();
			~ForestTrainer();

		};
	}
}





#endif