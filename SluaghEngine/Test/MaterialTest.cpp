#include "MaterialTest.h"
#include <Graphics\DeviceManager.h>
#include <Graphics\GraphicResourceHandler.h>
#include <Window\WindowSDL.h>
#include <Profiler.h>

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
	StartProfile;

	Window::IWindow* window = new Window::WindowSDL;
	auto r = window->Initialize();
	if (r)
	{
		console->Print("Could not init window, Error: %d.\n", r);
		ProfileReturnConst(false);
	}
	Graphics::DeviceManager* device = new Graphics::DeviceManager();
	HRESULT hr = device->Init((HWND)window->GetHWND());
	if (FAILED(hr))
	{
		console->Print("Could not init device, Error: %d.\n", hr);
		ProfileReturnConst(false);
	}
	auto mat = new Graphics::GraphicResourceHandler(device->GetDevice(), device->GetDeviceContext());

	int shaderID[2];

	hr = mat->CreateVertexShader(device->GetDevice(), &shaderID[0]);
	if (FAILED(hr))
	{
		console->Print("Could not init material handler, Error: %d.\n", hr);
		return false;
	}

	hr = mat->CreatePixelShader(device->GetDevice(), &shaderID[1]);
	if (FAILED(hr))
	{
		console->Print("Could not init material handler, Error: %d.\n", hr);
		ProfileReturnConst(false);
	}

	mat->SetMaterial(shaderID[0], shaderID[1]);

	mat->Shutdown();
	device->Shutdown();
	window->Shutdown();
	
	delete window;
	delete device;
	delete mat;
	ProfileReturnConst(true);

	
}