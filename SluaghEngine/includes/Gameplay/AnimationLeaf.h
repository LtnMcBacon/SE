#ifndef SE_GAMEPLAY_ANIMATION_LEAF_H
#define SE_GAMEPLAY_ANIMATION_LEAF_H
#include "IBehaviour.h"
#include "Core/IAnimationManager.h"

namespace SE
{
	namespace Gameplay
	{
		class AnimationLeaf : public IBehaviour
		{
		private:
			AnimationLeaf() = delete;
			Core::IAnimationManager::AnimationPlayInfo playInfo;
			AnimationLeaf(EnemyBlackboard* enemyBlackbodar, GameBlackboard* gameBlackboard, const Core::IAnimationManager::AnimationPlayInfo &playInfo);
		public:
			AnimationLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, int numberOfLayers,
			              std::string animationNames[], float animationSpeeds[], float startKeyFrame[], bool looping[]);
			~AnimationLeaf();


			Status Update() override;



			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new AnimationLeaf(enemyBlackboard, gameBlackboard, playInfo);
			};
		};
	}
}







#endif