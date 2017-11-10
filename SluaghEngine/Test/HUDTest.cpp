#include "HUDTest.h"
#include <Gameplay\HUD_Parsing.h>
#include <Core\IEngine.h>
#include <Gameplay\Game.h>
SE::Test::HUDTest::HUDTest()
{
}
SE::Test::HUDTest::~HUDTest()
{
}
bool SE::Test::HUDTest::Run(SE::DevConsole::IConsole * console)
{

	auto Engine = Core::CreateEngine();
	Engine->Init();
	Gameplay::Game game;
	game.Initiate(Engine);

	Gameplay::HUDParser myParser;
	myParser.ParseFiles("MainMenu.HuD");





	game.Shutdown();
	Engine->Release();
	delete Engine;
	return true;
}
