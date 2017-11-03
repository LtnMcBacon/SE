#include "DeviceManager.h"
#include <Profiler.h>

#pragma comment(lib, "d3d11.lib")

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

using namespace std;
using namespace DirectX;
using namespace SE::Graphics;

DeviceManager::DeviceManager(): gFeatureLevel()
{
	gDevice = nullptr;
	gDeviceContext = nullptr;
	gSecDeviceContext = nullptr;
	gSwapChain = nullptr;
	gBackBuffer = nullptr;
	gBackbufferRTV = nullptr;
	gBBSRV = nullptr;
	gDepthStencil = nullptr;
	gDepthStencilView = nullptr;
	gDepthStencilSRV = nullptr;
	pDSState = nullptr;
	blendSolidState = nullptr;
	blendTransState = nullptr;
	rasterSolidState = nullptr;
	rasterWireState = nullptr;
}

DeviceManager::~DeviceManager() {


}

HRESULT DeviceManager::Init(HWND windowHandle) {

	StartProfile;

	HRESULT hr = CreateDeviceResources();

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
	

	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;

	hr = gDevice->CreateRasterizerState(&rasterizerState, &rasterSolidState);
	if (FAILED(hr))
		throw std::exception("Failed to create rasterizer state");
	gDeviceContext->RSSetState(rasterSolidState);

	rasterizerState.FillMode = D3D11_FILL_WIREFRAME;

	hr = gDevice->CreateRasterizerState(&rasterizerState, &rasterWireState);
	if (FAILED(hr))
		throw std::exception("Failed to create rasterizer state");

	
	ProfileReturnConst(hr);

}

void DeviceManager::Shutdown() {

	StartProfile;

	gBBSRV->Release();
	gBackBuffer->Release();
	gBackbufferRTV->Release();
	pDSState->Release();
	gDepthStencilView->Release();
	gDepthStencilSRV->Release();
	blendSolidState->Release();
	blendTransState->Release();
	rasterSolidState->Release();
	rasterWireState->Release();
	gSwapChain->Release();
	gSecDeviceContext->Release();
	gDeviceContext->Release();
#ifdef _DEBUG
//	reportLiveObjects(gDevice);
#endif
	gDevice->Release();

	StopProfile;
}

HRESULT DeviceManager::CreateDeviceResources() {

	StartProfile;

	D3D_FEATURE_LEVEL levels[] = {

		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1

	};

	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)

	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif
	HRESULT hr = D3D11CreateDevice(

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
		ProfileReturnConst(hr);
	}

	hr = gDevice->CreateDeferredContext(0, &gSecDeviceContext);

	if (FAILED(hr)) {
		ProfileReturnConst(hr);
	}
	ProfileReturnConst(hr);
}

HRESULT DeviceManager::CreateSwapChain(HWND windowHandle) {

	StartProfile;

	DXGI_SWAP_CHAIN_DESC swChDesc;
	ZeroMemory(&swChDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swChDesc.Windowed = TRUE;
	swChDesc.BufferCount = 2;
	swChDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swChDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swChDesc.SampleDesc.Count = 1;
	swChDesc.SampleDesc.Quality = 0;
	swChDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swChDesc.OutputWindow = windowHandle;

	IDXGIDevice* dxgiDevice = 0;
	HRESULT hr = gDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)& dxgiDevice);
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

	hr = dxgiFactory->CreateSwapChain(gDevice, &swChDesc, &gSwapChain);
	if (FAILED(hr)) {
		ProfileReturnConst(hr);
	}
	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();

	ProfileReturnConst(hr);

}

HRESULT DeviceManager::CreateBackBufferRTV() {

	StartProfile;

	HRESULT hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&gBackBuffer);

	setDebugName(gBackBuffer, "STANDARD_BACK_BUFFER_TEXTURE2D");

	if (FAILED(hr)) {
		ProfileReturnConst(hr);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	gDevice->CreateShaderResourceView(gBackBuffer, &srvd, &gBBSRV);



	hr = gDevice->CreateRenderTargetView(gBackBuffer, nullptr, &gBackbufferRTV);

	if (FAILED(hr)) {
		ProfileReturnConst(hr);
	}

	setDebugName(gBackbufferRTV, "STANDARD_BACK_BUFFER_RTV");

	ProfileReturnConst(hr);
}

HRESULT DeviceManager::CreateDepthStencil() {

	StartProfile;

	gBackBuffer->GetDesc(&gBB_Desc);

	//CD3D11_TEXTURE2D_DESC depthStencilDesc(
	//	DXGI_FORMAT_D24_UNORM_S8_UINT,
	//	static_cast<UINT> (gBB_Desc.Width),
	//	static_cast<UINT> (gBB_Desc.Height),
	//	1, // This depth stencil view has only one texture
	//	1, // Use a single mipmap level
	//	D3D11_BIND_DEPTH_STENCIL
	//);

	D3D11_TEXTURE2D_DESC td;
	td.Width = static_cast<UINT>(gBB_Desc.Width);
	td.Height = static_cast<UINT>(gBB_Desc.Height);
	td.ArraySize = 1;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.Format = DXGI_FORMAT_R24G8_TYPELESS;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.MipLevels = 1;
	td.MiscFlags = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.CPUAccessFlags = 0;

	ID3D11Texture2D* depthTexture;
	HRESULT hr = gDevice->CreateTexture2D(&td, nullptr, &depthTexture);
	if (FAILED(hr))
		ProfileReturnConst(hr);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	dsvd.Flags = 0;

	hr = gDevice->CreateDepthStencilView(depthTexture, &dsvd, &gDepthStencilView);
	if (FAILED(hr))
		ProfileReturnConst(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = td.MipLevels;
	srvd.Texture2D.MostDetailedMip = 0;

	hr = gDevice->CreateShaderResourceView(depthTexture, &srvd, &gDepthStencilSRV);
	if (FAILED(hr))
		ProfileReturnConst(hr);

	/*HRESULT hr = gDevice->CreateTexture2D(
		&depthStencilDesc,
		nullptr,
		&gDepthStencil
	);

	if (FAILED(hr)) {

		ProfileReturnConst(hr);
	}*/

//	setDebugName(gDepthStencil, "STANDARD_DEPTH_STENCIL_TEXTURE2D");


	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	gDevice->CreateDepthStencilState(&dsDesc, &pDSState);

	gDeviceContext->OMSetDepthStencilState(pDSState, 1);
	depthTexture->Release();
	//CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	/*gDevice->CreateDepthStencilView(

		gDepthStencil,
		&depthStencilViewDesc,
		&gDepthStencilView

	);

	if (FAILED(hr)) {
		ProfileReturnConst(hr);
	}*/

//	setDebugName(gDepthStencilView, "STANDARD_DEPTH_STENCIL_RTV");

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



void DeviceManager::Present() {

	gSwapChain->Present(1, 0);
}

void DeviceManager::ResizeSwapChain(HWND windowHandle)
{
	gSwapChain->Release();
	gBackBuffer->Release();
	gBBSRV->Release();
	gBackbufferRTV->Release();
	pDSState->Release();
	//gDepthStencil->Release();
	gDepthStencilView->Release();
	gDepthStencilSRV->Release();
	CreateSwapChain(windowHandle);
	CreateBackBufferRTV();
	CreateDepthStencil();
	SetViewport();
}

void DeviceManager::CreateBlendState()
{
	// Transparency off
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

	HRESULT hr = gDevice->CreateBlendState(&blendStateDesc, &blendSolidState);
	if (FAILED(hr))
		throw std::exception("Could not create blend state");

	UINT sampleM = 0xffffffff;
	gDeviceContext->OMSetBlendState(blendSolidState, NULL, sampleM);

	// Transparency on
	D3D11_RENDER_TARGET_BLEND_DESC rendTransBlendState[8];
	for (auto& rtbs : rendTransBlendState)
	{
		rtbs.BlendEnable = true;
		rtbs.BlendOp = D3D11_BLEND_OP_ADD;
		rtbs.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbs.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtbs.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		rtbs.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		rtbs.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtbs.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	}

	D3D11_BLEND_DESC blendTransStateDesc;
	blendTransStateDesc.AlphaToCoverageEnable = true;
	blendTransStateDesc.IndependentBlendEnable = false;
	blendTransStateDesc.RenderTarget[0] = rendTransBlendState[0];
	blendTransStateDesc.RenderTarget[1] = rendTransBlendState[1];
	blendTransStateDesc.RenderTarget[2] = rendTransBlendState[2];
	blendTransStateDesc.RenderTarget[3] = rendTransBlendState[3];
	blendTransStateDesc.RenderTarget[4] = rendTransBlendState[4];
	blendTransStateDesc.RenderTarget[5] = rendTransBlendState[5];
	blendTransStateDesc.RenderTarget[6] = rendTransBlendState[6];
	blendTransStateDesc.RenderTarget[7] = rendTransBlendState[7];

	hr = gDevice->CreateBlendState(&blendTransStateDesc, &blendTransState);
	if (FAILED(hr))
		throw std::exception("Could not create blend state");
}

ID3D11Texture2D* DeviceManager::GetBackBufferTexture()
{
	return gBackBuffer;
}
