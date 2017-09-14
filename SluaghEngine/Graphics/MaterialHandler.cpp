
#include "MaterialHandler.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3Dcompiler.lib")

using namespace std;
using namespace DirectX;
using namespace SE::Graphics;

MaterialHandler::MaterialHandler(Microsoft::WRL::ComPtr<ID3D11Device> gDevice) {

	this->gDevice = gDevice;
}

MaterialHandler::~MaterialHandler() {


}

HRESULT MaterialHandler::Init() {

	HRESULT hr = S_OK;

	hr = InitializeDefaultShaders(this->gDevice);

	if (FAILED(hr)) {

		throw std::exception("CRITICAL ERROR: Default shaders in MaterialHandler could not be created");
	}

	return hr;
}

HRESULT MaterialHandler::InitializeDefaultShaders(Microsoft::WRL::ComPtr<ID3D11Device> gDevice) {

	HRESULT hr = S_OK;

	// We require three inputs (IN THE FOLLOWING ORDER):
	// POSITION
	// NORMAL
	// TEXCOORD
	D3D11_INPUT_ELEMENT_DESC vertexInputLayout[3];

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Test\\Asset\\SimpleVS.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		hr;
	}

	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &d_vertexShader);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return hr;
	}

	setDebugName(d_vertexShader.Get(), "DEFAULT_VERTEX_SHADER");

	vertexInputLayout[0].SemanticName = "POSITION";
	vertexInputLayout[0].SemanticIndex = 0;
	vertexInputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexInputLayout[0].InputSlot = 0;
	vertexInputLayout[0].AlignedByteOffset = 0;
	vertexInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[0].InstanceDataStepRate = 0;

	vertexInputLayout[1].SemanticName = "NORMAL";
	vertexInputLayout[1].SemanticIndex = 0;
	vertexInputLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexInputLayout[1].InputSlot = 0;
	vertexInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[1].InstanceDataStepRate = 0;

	vertexInputLayout[2].SemanticName = "TEXCOORD";
	vertexInputLayout[2].SemanticIndex = 0;
	vertexInputLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexInputLayout[2].InputSlot = 0;
	vertexInputLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexInputLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexInputLayout[2].InstanceDataStepRate = 0;

	int inputLayoutSize = sizeof(vertexInputLayout) / sizeof(vertexInputLayout[0]);
	gDevice->CreateInputLayout(vertexInputLayout, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &d_layout);

	if (FAILED(hr)) {

		cout << "Input Layout Error: Shader Input Layout could not be created" << endl;

		return hr;
	}

	vsBlob->Release();

	setDebugName(d_layout.Get(), "DEFAULT_INPUT_LAYOUT");


	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Test\\Asset\\SimplePS.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Pixel Shader Error: Pixel Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return hr;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &d_pixelShader);

	if (FAILED(hr)) {

		cout << "Pixel Shader Error: Pixel Shader could not be created" << endl;
		return hr;
	}

	setDebugName(d_pixelShader.Get(), "DEFAULT_PIXEL_SHADER");

	psBlob->Release();

	return hr;
}

void MaterialHandler::Shutdown() {

	d_vertexShader.Reset();
	d_pixelShader.Reset();
	d_layout.Reset();


}