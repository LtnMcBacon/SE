#include "BufferTest.h"
#include <d3d11.h>
#include <DirectXMath.h>

#include "..\Graphics\ConstantBufferHandler.h"
#include "..\Graphics\DeviceManager.h"
#include "..\includes\Window\Window.h"

namespace SE
{
	namespace Test
	{
		BufferTest::BufferTest()
		{

		}

		BufferTest::~BufferTest()
		{

		}

		bool BufferTest::Run(Utilz::IConsoleBackend* console)
		{
			Window::InterfaceWindow* window = new Window::Window();
			window->Initialise();
			Graphics::DeviceManager* deviceManager = new Graphics::DeviceManager();
			deviceManager->Initialize((HWND)window->GethWnd());


		#pragma region Constbuffer
			DirectX::XMVECTOR axis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(axis, DirectX::XM_PI * 0.1);
			DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(1.0f, 5.0f, 10.0f);
			DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
			DirectX::XMMATRIX world = DirectX::XMMatrixTranspose(rotation * scale * translation);

			DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(0, 0, 0, 0);
			DirectX::XMVECTOR lookAtVec = DirectX::XMVectorSet(0, 1, 0, 0);
			DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(cameraPos, lookAtVec, DirectX::XMVectorSet(0, 1, 0, 0));

			DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI *0.45f, 640 / 480, 0.5f, 500);

			Graphics::ConstantBufferHandler* cBufferHandle = new ConstantBufferHandler();
		#pragma endregion
		}

	}
}
