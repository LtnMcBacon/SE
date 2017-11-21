#include <Game.h>
#include "CoreInit.h"
#include <Profiler.h>

void SE::Gameplay::Game::Initiate(Core::IEngine* engine)
{
	StartProfile;
	CoreInit::Init(engine);
	this->engine = engine;

	//should be read from file later on but hardcoded now

	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::EXIT_GAME), Window::KeyEscape);

	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::UP), Window::KeyW);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::DOWN), Window::KeyS);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::LEFT), Window::KeyA);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::RIGHT), Window::KeyD);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::CONSOLE), Window::KeyTab);

	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::SKILL1), Window::KeyQ);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::SKILL2), Window::KeyE);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::ACTION), Window::MouseLeft);

	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::STEP), Window::KeyF1);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::INTERACT), Window::KeyF);
	CoreInit::subSystems.window->MapActionButton(uint32_t(GameInput::SHOWINFO), Window::KeyShiftL);
	CoreInit::subSystems.window->MapActionButton(GameInput::PICKUP, Window::Key1);
	CoreInit::subSystems.window->MapActionButton(GameInput::PICKUP, Window::Key2);
	CoreInit::subSystems.window->MapActionButton(GameInput::PICKUP, Window::Key3);
	CoreInit::subSystems.window->MapActionButton(GameInput::PICKUP, Window::Key4);
	CoreInit::subSystems.window->MapActionButton(GameInput::PICKUP, Window::Key5);
	CoreInit::subSystems.window->MapActionButton(GameInput::ONE, Window::Key1);
	CoreInit::subSystems.window->MapActionButton(GameInput::TWO, Window::Key2);
	CoreInit::subSystems.window->MapActionButton(GameInput::THREE, Window::Key3);
	CoreInit::subSystems.window->MapActionButton(GameInput::FOUR, Window::Key4);
	CoreInit::subSystems.window->MapActionButton(GameInput::FIVE, Window::Key5);



	CoreInit::subSystems.window->BindKeyPressCallback(uint32_t(GameInput::CONSOLE), []()
	{
		CoreInit::subSystems.devConsole->Toggle();
	});
	void* data = nullptr;
	//state = new PlayState(CoreInit::subSystems.window, engine, nullptr);
	//currentState = SE::Gameplay::IGameState::State::PLAY_STATE;
	
	CoreInit::managers.textManager->MakeFont(Utilz::GUID("CloisterBlack.spritefont"));

	state =  new MainMenuState(CoreInit::subSystems.window);
	currentState = SE::Gameplay::IGameState::State::MAIN_MENU_STATE;



	Core::IEventManager::EventCallbacks roomChangeEvent;
	roomChangeEvent.triggerCheck = [](Core::Entity ent, void* data)
	{
		return *(bool*)data;
	};

	roomChangeEvent.triggerCallback = [](Core::Entity ent, void* data)
	{
		auto vis = std::get<bool>( CoreInit::managers.dataManager->GetValue(ent, "Visible", false));
		if (*(bool*)data && !vis)
		{
			
			CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "WeaponPickUp");
			CoreInit::managers.eventManager->RegisterEntitytoEvent(ent, "StartRenderWIC");
			CoreInit::managers.renderableManager->ToggleRenderableObject(ent, true);
		
		}
		else if(!*(bool*)data && vis)
		{
			CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "StartRenderWIC");
			CoreInit::managers.eventManager->UnregisterEntitytoEvent(ent, "WeaponPickUp");
			CoreInit::managers.renderableManager->ToggleRenderableObject(ent, false);
		}
			
	};
	CoreInit::managers.eventManager->RegisterEventCallback("RoomChange", roomChangeEvent);
}

void SE::Gameplay::Game::Run()
{
	StartProfile;
	void* data = nullptr;
	//SE::Gameplay::IGameState::State currentState = SE::Gameplay::IGameState::State::PLAY_STATE;
	SE::Gameplay::IGameState::State newState = state->PLAY_STATE;
	CoreInit::engine->GetSubsystems().window->UpdateTime();
	bool running = true;
	//!CoreInit::subSystems.window->ButtonPressed(uint32_t(GameInput::EXIT_GAME))
	while (running)
	{
		if (CoreInit::subSystems.window->ButtonPressed(uint32_t(GameInput::EXIT_GAME)))
		{
			running = false;
		}
		CoreInit::engine->BeginFrame();
		newState = state->Update(data);

		if (newState != currentState)
		{
			if (currentState == SE::Gameplay::IGameState::State::PLAY_STATE)
			{
				CoreInit::subSystems.window->StopRecording();
				CoreInit::engine->EndFrame();
				return;
			}
			switch (newState)
			{
				case SE::Gameplay::IGameState::State::GAME_OVER_STATE:
				{
					/*if (currentState == SE::Gameplay::IGameState::State::PLAY_STATE || currentState == SE::Gameplay::IGameState::State::CHARACTER_CREATION_STATE)
						CoreInit::subSystems.window->StopRecording();*/
					delete state;
					state = new SE::Gameplay::PlayState(CoreInit::subSystems.window, engine, data);
					break;
				}
				case SE::Gameplay::IGameState::State::MAIN_MENU_STATE:
				{
					/*if (currentState == SE::Gameplay::IGameState::State::PLAY_STATE || currentState == SE::Gameplay::IGameState::State::CHARACTER_CREATION_STATE)
						CoreInit::subSystems.window->StopRecording();*/
					delete state;
					state = new SE::Gameplay::MainMenuState(CoreInit::subSystems.window);
					break;
				}
				case SE::Gameplay::IGameState::State::CHARACTER_CREATION_STATE:
				{
					/*CoreInit::subSystems.window->StartRecording();*/
					delete state;
					state = new SE::Gameplay::CharacterCreationState(CoreInit::subSystems.window);
					break;
				}
				case SE::Gameplay::IGameState::State::PLAY_STATE:
				{
					delete state;
					state = new SE::Gameplay::PlayState(CoreInit::subSystems.window, engine, data);
					CoreInit::subSystems.window->UpdateTime();
					break;
				}
				case SE::Gameplay::IGameState::State::QUIT_GAME:
					
					running = false;
					break;
			 }

			currentState = newState;
		}

		CoreInit::engine->EndFrame();
	}
	StopProfile;
}

void SE::Gameplay::Game::Shutdown()
{
	delete state;
}
