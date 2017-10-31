#include <Game.h>
#include "CoreInit.h"

void SE::Gameplay::Game::Initiate(Core::IEngine* engine)
{
	CoreInit::Init(engine);
	this->engine = engine;

	//should be read from file later on but hardcoded now

	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::EXIT_GAME), Window::KeyEscape);

	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::UP), Window::KeyW);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::DOWN), Window::KeyS);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::LEFT), Window::KeyA);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::RIGHT), Window::KeyD);

	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::SKILL1), Window::Key1);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::SKILL2), Window::Key2);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::ACTION), Window::MouseLeft);

	//state = new PlayState(CoreInit::subSystems.window, engine);
}

void SE::Gameplay::Game::Run()
{
	void* data = nullptr;
	SE::Gameplay::IGameState::State currentState = SE::Gameplay::IGameState::State::PLAY_STATE;
	SE::Gameplay::IGameState::State newState;

	while (!CoreInit::subSystems.window->ButtonPressed(uint32_t(GameInput::EXIT_GAME)))
	{
		 newState = state->Update(data);

		 if (newState != currentState)
		 {
			switch (newState)
			{
				case SE::Gameplay::IGameState::State::PLAY_STATE:
				{
					delete state;
					state = new SE::Gameplay::PlayState(input, engine);
				}
			 }
		 }

		 CoreInit::engine->BeginFrame();
		 CoreInit::engine->EndFrame();
	}
}

void SE::Gameplay::Game::Shutdown()
{
	delete state;
}
