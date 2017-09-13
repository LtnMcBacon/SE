#include "InputLayoutTest.h"
#include <Graphics\
#include <Graphics\Renderer.h>
#include <Window\Window.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif

using namespace SE;

SE::Test::InputLayoutTest::InputLayoutTest::InputLayoutTest() {


}

SE::Test::InputLayoutTest::InputLayoutTest::~InputLayoutTest() {


}

bool SE::Test::InputLayoutTest::Run(Utilz::IConsoleBackend * console) {

	Graphics::IRenderer* r = new Graphics::Renderer();
	Window::InterfaceWindow* w = new Window::Window();
	bool result = w->Initialise();
	if (!result)
	{
		console->Print("Could not initialize window.\n");
		return false;
	}
	r->Initialize(w->GethWnd());



	r->Shutdown();

	w->Shutdown();
	return true;
}