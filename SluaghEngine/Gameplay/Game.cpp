#include <Game.h>

void SE::Gameplay::Game::Initiate()
{
	//should be read from file later on but hardcoded now
	input = SE::Core::Engine::GetInstance().GetWindow();

	input->MapActionButton(uint32_t(GameInput::EXIT_GAME), Window::KeyEscape);

	input->MapActionButton(uint32_t(GameInput::UP), Window::KeyW);
	input->MapActionButton(uint32_t(GameInput::DOWN), Window::KeyS);
	input->MapActionButton(uint32_t(GameInput::LEFT), Window::KeyA);
	input->MapActionButton(uint32_t(GameInput::RIGHT), Window::KeyD);

	input->MapActionButton(uint32_t(GameInput::SKILL1), Window::Key1);
	input->MapActionButton(uint32_t(GameInput::SKILL2), Window::Key2);
	input->MapActionButton(uint32_t(GameInput::ACTION), Window::MouseLeft);

	state = new PlayState(input);
}

void SE::Gameplay::Game::Run()
{
	auto& engine = Core::Engine::GetInstance();
	void* data = nullptr;
	SE::Gameplay::IGameState::State currentState = SE::Gameplay::IGameState::State::PLAY_STATE;
	SE::Gameplay::IGameState::State newState;

	while (!SE::Core::Engine::GetInstance().GetWindow()->ButtonPressed(uint32_t(GameInput::EXIT_GAME)))
	{
		 newState = state->Update(data);

		 if (newState != currentState)
		 {
			switch (newState)
			{
				case SE::Gameplay::IGameState::State::PLAY_STATE:
				{
					delete state;
					state = new SE::Gameplay::PlayState(input);
				}
			 }
		 }

		 engine.Frame(1 / 60.0f);
	}
}