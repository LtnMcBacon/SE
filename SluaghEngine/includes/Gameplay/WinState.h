#ifndef SE_GAMEPLAY_WIN_STATE_H_
#define SE_GAMEPLAY_WIN_STATE_H_
#include "IGameState.h"

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
		Core::Entity victoriousScot;
		Core::Entity victoryText;
		Core::Entity quitButton;
		Core::Entity newCharacterButton;
	};
}


#endif