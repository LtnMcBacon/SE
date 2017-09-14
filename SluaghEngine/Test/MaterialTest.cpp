#include "MaterialTest.h"
#include <Graphics\DeviceManager.h>
#include <Graphics\MaterialHandler.h>
#include <Window\Window.h>

#ifdef _DEBUG
#pragma comment(lib, "GraphicsD.lib")
#pragma comment(lib, "WindowD.lib")
#else
#pragma comment(lib, "Graphics.lib")
#pragma comment(lib, "Window.lib")
#endif

using namespace SE;

SE::Test::MaterialTest::MaterialTest::MaterialTest() {


}

SE::Test::MaterialTest::MaterialTest::~MaterialTest() {


}

bool SE::Test::MaterialTest::Run(Utilz::IConsoleBackend * console) 
{

	Window::InterfaceWindow* window = new Window::Window;
	auto r = window->Initialise();
	if (r)
	{
		console->Print("Could not init window, Error: %d.\n", r);
		return false;
	}
	Graphics::DeviceManager* device = new Graphics::DeviceManager();
	HRESULT hr = device->Init((HWND)window->GethWnd());
	if (FAILED(hr))
	{
		console->Print("Could not init device, Error: %d.\n", hr);
		return false;
	}
	auto mat = new Graphics::MaterialHandler(device->GetDevice(), device->GetDeviceContext());
	hr = mat->Init();
	if (r)
	{
		console->Print("Could not init material handler, Error: %d.\n", hr);
		return false;
	}
	mat->SetMaterial();

	mat->Shutdown();
	device->Shutdown();
	window->Shutdown();
	
	return true;
}