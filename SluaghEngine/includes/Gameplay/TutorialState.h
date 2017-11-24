#ifndef SE_GAMEPLAY_TUTORIAL_STATE_H_
#define SE_GAMEPLAY_TUTORIAL_STATE_H_
#include "IGameState.h"

namespace SE::Gameplay
{
	class Room;
	class PlayerUnit;
	class TutorialState : public IGameState
	{
	public:
		TutorialState();
		~TutorialState();

		State Update(void*&)override;

	private:

		Room* room;
		PlayerUnit* player;
	
		struct SkillIndicator
		{
			Core::Entity frame;
			Core::Entity Image;
			float currentCooldown = 0;
			float maxCooldown;
		}skillIndicators[2];
	};
}


#endif