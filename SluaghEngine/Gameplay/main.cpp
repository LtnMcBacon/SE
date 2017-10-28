#include "CoreInit.h"

using namespace SE;
int main()
{
	Gameplay::CoreInit::Init(Core::CreateEngine());
	// Gameplay game;
	// game.Init();
	// game.Run();
	// game.Shutdown();

	Gameplay::CoreInit::Shutdown();

	return 0;
}