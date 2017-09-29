#include "DeviceManager.h"
#include <Utilz\Console.h>
#include <Profiler.h>

#pragma comment(lib, "d3d11.lib")

using namespace std;
using namespace DirectX;
using namespace SE::Graphics;
using namespace SE::Utilz;

DeviceManager::DeviceManager() {

}

DeviceManager::~DeviceManager() {


}

HRESULT DeviceManager::Init(HWND windowHandle) {

	StartProfile;

	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (FAILED(hr)) {
		StopProfile;
		throw std::exception("CRITICAL ERROR: Device resources could not be initialized");
	}

	hr = CreateSwapChain(windowHandle);

	if (FAILED(hr)) {
		StopProfile;
		throw std::exception("CRITICAL ERROR: Swap chain could not be initialized");
	}

	hr = CreateBackBufferRTV();

	if (FAILED(hr)) {
		StopProfile;
		throw std::exception("CRITICAL ERROR: Back buffer render target view could not be created");
	}

	hr = CreateDepthStencil();

	if (FAILED(hr)) {
		StopProfile;
		throw std::exception("CRITICAL ERROR: Depth stencil view could not be created");
	}

	SetViewport();
	CreateBlendState();

	ProfileReturnConst(hr);
}

void DeviceManager::Shutdown() {

	StartProfile;

	gSwapChain->Release();

	gBackBuffer->Release();
	gBackbufferRTV->Release();

	gDepthStencil->Release();
	gDepthStencilView->Release();

	gDeviceContext->Release();
	blendState->Release();

#ifdef _DEBUG

	/*reportLiveObjects(gDevice);*/

#endif
	gDevice->Release();;

	StopProfile;
}

HRESULT DeviceManager::CreateDeviceResources() {

	StartProfile;

	HRESULT hr = S_OK;

	D3D_FEATURE_LEVEL levels[] = {

		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1

	};

	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)

	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif
	hr = D3D11CreateDevice(

		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		deviceFlags,
		levels,
		ARRAYSIZE(levels),
		D3D11_SDK_VERSION,
		&gDevice,
		&gFeatureLevel,
		&gDeviceContext

	);


	if (FAILED(hr)) {

		Console::Print("Device Creation Error: Device, DeviceContext and Swap Chain could not be created");
		ProfileReturnConst(hr);
	}

	ProfileReturnConst(hr);
}

HRESULT DeviceManager::CreateSwapChain(HWND windowHandle) {

	StartProfile;

	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC swChDesc;
	ZeroMemory(&swChDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swChDesc.Windowed = TRUE;
	swChDesc.BufferCount = 2;
	swChDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swChDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swChDesc.SampleDesc.Count = 1;
	swChDesc.SampleDesc.Quality = 0;
	swChDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swChDesc.OutputWindow = windowHandle;

	IDXGIDevice* dxgiDevice = 0;
	hr = gDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)& dxgiDevice);
	if (FAILED(hr)) {

		ProfileReturnConst(hr);
	}

	IDXGIAdapter* dxgiAdapter = 0;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)& dxgiAdapter);
	if (FAILED(hr)) {

		ProfileReturnConst(hr);
	}

	IDXGIFactory* dxgiFactory = 0;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)& dxgiFactory);
	if (FAILED(hr)) {
		ProfileReturnConst(hr);
	}

	dxgiFactory->CreateSwapChain(gDevice, &swChDesc, &gSwapChain);

	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();

	ProfileReturnConst(hr);

}

HRESULT DeviceManager::CreateBackBufferRTV() {

	StartProfile;

	HRESULT hr = S_OK;
	hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&gBackBuffer);

	setDebugName(gBackBuffer, "STANDARD_BACK_BUFFER_TEXTURE2D");

	if (FAILED(hr)) {

		Console::Print("Buffer Error: Back buffer could not be retrieved");
		ProfileReturnConst(hr);
	}

	hr = gDevice->CreateRenderTargetView(gBackBuffer, nullptr, &gBackbufferRTV);

	if (FAILED(hr)) {

		Console::Print("Render Target View Error: Render target view could not be created");
		ProfileReturnConst(hr);
	}

	setDebugName(gBackbufferRTV, "STANDARD_BACK_BUFFER_RTV");

	ProfileReturnConst(hr);
}

HRESULT DeviceManager::CreateDepthStencil() {

	StartProfile;

	HRESULT hr = S_OK;

	gBackBuffer->GetDesc(&gBB_Desc);

	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT> (gBB_Desc.Width),
		static_cast<UINT> (gBB_Desc.Height),
		1, // This depth stencil view has only one texture
		1, // Use a single mipmap level
		D3D11_BIND_DEPTH_STENCIL
	);

	hr = gDevice->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&gDepthStencil
	);

	if (FAILED(hr)) {

		Console::Print("Depth Stencil Error: Depth stencil texture couldn't be created");
		ProfileReturnConst(hr);
	}

	setDebugName(gDepthStencil, "STANDARD_DEPTH_STENCIL_TEXTURE2D");

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	gDevice->CreateDepthStencilView(

		gDepthStencil,
		&depthStencilViewDesc,
		&gDepthStencilView

	);

	if (FAILED(hr)) {

		Console::Print("Depth Stencil RTV Error: Depth stencil RTV could not be created");
		ProfileReturnConst(hr);
	}

	setDebugName(gDepthStencilView, "STANDARD_DEPTH_STENCIL_RTV");

	ProfileReturnConst(hr);
}

void DeviceManager::SetViewport() {

	ZeroMemory(&gViewportDefault, sizeof(D3D11_VIEWPORT));
	gViewportDefault.TopLeftX = 0.0f;
	gViewportDefault.TopLeftY = 0.0f;
	gViewportDefault.MinDepth = 0.0f;
	gViewportDefault.MaxDepth = 1.0f;
	gViewportDefault.Width = (FLOAT)gBB_Desc.Width;
	gViewportDefault.Height = (FLOAT)gBB_Desc.Height;

	gDeviceContext->RSSetViewports(1, &gViewportDefault);
}

float DeviceManager::GetAspectRatio() {

	return static_cast<float>(gBB_Desc.Width) / static_cast<float>(gBB_Desc.Height);
}

void DeviceManager::Present() {

	auto hr = gSwapChain->Present(1, 0);
}

void DeviceManager::ResizeSwapChain(HWND windowHandle)
{
	gSwapChain->Release();
	gBackBuffer->Release();
	gBackbufferRTV->Release();
	gDepthStencil->Release();
	gDepthStencilView->Release();
	CreateSwapChain(windowHandle);
	CreateBackBufferRTV();
	CreateDepthStencil();
	SetViewport();
}

void DeviceManager::CreateBlendState()
{
	D3D11_RENDER_TARGET_BLEND_DESC rendTarBlendState[8];
	for (auto& rtbs : rendTarBlendState)
	{
	rtbs.BlendEnable = false;
	rtbs.BlendOp = D3D11_BLEND_OP_ADD;
	rtbs.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbs.DestBlend = D3D11_BLEND_ZERO;
	rtbs.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbs.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	rtbs.SrcBlend = D3D11_BLEND_ONE;
	rtbs.SrcBlendAlpha = D3D11_BLEND_ONE;
	}

	// just default values right now
	D3D11_BLEND_DESC blendStateDesc;
	blendStateDesc.AlphaToCoverageEnable = false;
	blendStateDesc.IndependentBlendEnable = false;
	blendStateDesc.RenderTarget[0] = rendTarBlendState[0];
	blendStateDesc.RenderTarget[1] = rendTarBlendState[1];
	blendStateDesc.RenderTarget[2] = rendTarBlendState[2];
	blendStateDesc.RenderTarget[3] = rendTarBlendState[3];
	blendStateDesc.RenderTarget[4] = rendTarBlendState[4];
	blendStateDesc.RenderTarget[5] = rendTarBlendState[5];
	blendStateDesc.RenderTarget[6] = rendTarBlendState[6];
	blendStateDesc.RenderTarget[7] = rendTarBlendState[7];

	HRESULT hr = gDevice->CreateBlendState(&blendStateDesc, &blendState);

	float blendF[4] = { 0.0f,0.0f,0.0f,0.0f };
	UINT sampleM = 0xffffffff;
	gDeviceContext->OMSetBlendState(blendState, blendF, sampleM);
}
