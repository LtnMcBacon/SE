#include "InitGraphicsTest.h"
#include <Graphics\Renderer.h>
#include <Window\InterfaceWindow.h>

#ifdef _DEBUG
#pragma comment(lib, "GraphicsD.lib")
#else
#pragma comment(lib, "Graphics.lib")
#endif

SE::Test::InitGraphicsTest::InitGraphicsTest::InitGraphicsTest()
{
}


SE::Test::InitGraphicsTest::InitGraphicsTest::~InitGraphicsTest()
{
}

bool SE::Test::InitGraphicsTest::Run(Utilz::IConsoleBackend * console)
{
	Graphics::IRenderer* r = new Graphics::Renderer();
	r->Initialize(nullptr);
	r->Shutdown();
	return true;
}
