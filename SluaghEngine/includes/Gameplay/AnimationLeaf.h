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
			std::vector<Utilz::GUID> animationGUID;
			Core::AnimationFlags animationFlags;
			float animationDuration = 0.f;
		public:
			AnimationLeaf(EnemyBlackboard* enemyBlackboard, GameBlackboard* gameBlackboard, const std::vector<Utilz::GUID> &animations, float animationDuration, const Core::AnimationFlags &animationFlags);
			~AnimationLeaf();

			Status Update() override;

			inline IBehaviour* CopyBehaviour(GameBlackboard* gameBlackboard, EnemyBlackboard* enemyBlackboard) const override
			{
				return new AnimationLeaf(enemyBlackboard, gameBlackboard, animationGUID, animationDuration, animationFlags);
			};
		};
	}
}







#endif