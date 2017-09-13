
#include "DeviceManager.h"


#pragma comment(lib, "d3d11.lib")

using namespace std;
using namespace DirectX;
using namespace SE::Graphics;

DeviceManager::DeviceManager() {

}

DeviceManager::~DeviceManager() {


}

HRESULT DeviceManager::Initialize(HWND windowHandle) {

	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			"CRITICAL ERROR: Device resources could not be initialized\nClosing application...",
			"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	hr = CreateSwapChain(windowHandle);

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			"CRITICAL ERROR: Swap chain could not be initialized\nClosing application...",
			"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	hr = CreateBackBufferRTV();

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			"CRITICAL ERROR: Back buffer render target view could not be created\nClosing application...",
			"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	hr = CreateDepthStencil();

	if (FAILED(hr)) {

		MessageBox(
			NULL,
			"CRITICAL ERROR: Depth stencil view could not be created\nClosing application...",
			"ERROR",
			MB_OK);

		PostQuitMessage(0);
	}

	SetViewport();

	return 0;
}

void DeviceManager::Shutdown() {

	gDeviceContext.Reset();
	gSwapChain.Reset();

	gBackBuffer.Reset();
	gBackbufferRTV.Reset();

	gDepthStencil.Reset();
	gDepthStencilView.Reset();

#ifdef _DEBUG
	reportLiveObjects(gDevice.Get());
#endif
	gDevice.Reset();
}

HRESULT DeviceManager::CreateDeviceResources() {

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

		cout << "Device Creation Error: Device, DeviceContext and Swap Chain could not be created" << endl;
		return S_FALSE;
	}

	return hr;
}

HRESULT DeviceManager::CreateSwapChain(HWND windowHandle) {

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

		return S_FALSE;
	}

	IDXGIAdapter* dxgiAdapter = 0;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)& dxgiAdapter);
	if (FAILED(hr)) {

		return S_FALSE;
	}

	IDXGIFactory* dxgiFactory = 0;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)& dxgiFactory);
	if (FAILED(hr)) {

		dxgiFactory->CreateSwapChain(gDevice.Get(), &swChDesc, &gSwapChain);
	}

	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();

	return hr;

}

HRESULT DeviceManager::CreateBackBufferRTV() {

	HRESULT hr = S_OK;

	hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&gBackBuffer);

	setDebugName(gBackBuffer.Get(), "STANDARD_BACK_BUFFER_TEXTURE2D");

	if (FAILED(hr)) {

		cout << "Buffer Error: Back buffer could not be retrieved" << endl;
		return S_FALSE;
	}

	hr = gDevice->CreateRenderTargetView(gBackBuffer.Get(), nullptr, &gBackbufferRTV);

	if (FAILED(hr)) {

		cout << "Render Target View Error: Render target view could not be created" << endl;
		return S_FALSE;
	}

	setDebugName(gBackbufferRTV.Get(), "STANDARD_BACK_BUFFER_RTV");

	return hr;
}

HRESULT DeviceManager::CreateDepthStencil() {

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

		cout << "Depth Stencil Error: Depth stencil texture couldn't be created" << endl;
		return S_FALSE;
	}

	setDebugName(gDepthStencil.Get(), "STANDARD_DEPTH_STENCIL_TEXTURE2D");

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	gDevice->CreateDepthStencilView(

		gDepthStencil.Get(),
		&depthStencilViewDesc,
		&gDepthStencilView

	);

	if (FAILED(hr)) {

		cout << "Depth Stencil RTV Error: Depth stencil RTV could not be created" << endl;
		return S_FALSE;
	}

	setDebugName(gDepthStencilView.Get(), "STANDARD_DEPTH_STENCIL_RTV");

	return hr;
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

	gSwapChain->Present(1, 0);
}
