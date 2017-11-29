#ifndef SE_GAMEPLAY_WIN_STATE_H_
#define SE_GAMEPLAY_WIN_STATE_H_
#include "IGameState.h"
#include "Utilz/Timer.h"

namespace SE::Gameplay
{
	class WinState : public IGameState
	{
	public:
		WinState();
		~WinState();

		State Update(void*&)override;

	private:
		Core::Entity cam;
		Core::Entity deadSluagh;
		Core::Entity sluaghSoul;
		Core::Entity victoriousScot;
		Core::Entity victoryText;
		Core::Entity quitButton;
		Core::Entity newCharacterButton;
		Utilz::Timer timer;
		State stateToReturn;
		float totTime;
		bool hack;
		bool hack2;
	};
}


#endif