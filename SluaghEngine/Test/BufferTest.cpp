#include "BufferTest.h"
#include <d3d11.h>
#include <DirectXMath.h>

#include "..\Graphics\ConstantBufferHandler.h"
#include "..\includes\Graphics\DeviceManager.h"
#include "..\includes\Window\Window.h"
#include "..\Graphics\StaticVertexBufferHandler.h"

#include "ObjLoaderTest.h"
#include <Core\Engine.h>
#include <Graphics\Renderer.h>
#include <OBJParser\Parsers.h>

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
			int isOK = window->Initialise();
			if (isOK != 0)
			{
				return isOK;
			}
			Graphics::DeviceManager* deviceManager = new Graphics::DeviceManager();
			HRESULT hr = deviceManager->Init((HWND)window->GethWnd());
			if (hr != S_OK)
			{
				return false;
			}

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

			Graphics::TargetOffset tarOff[3];
			tarOff[0].shaderTarget[0] = true;
			tarOff[1].shaderTarget[1] = true;
			tarOff[2].shaderTarget[2] = true;
			tarOff[0].offset[0] = 0;
			tarOff[1].offset[1] = 0;
			tarOff[2].offset[2] = 0;
			int ID[3];
			Graphics::ConstantBufferHandler* cBufferHandle = new Graphics::ConstantBufferHandler(deviceManager->GetDevice(), deviceManager->GetDeviceContext());
			//create Cbuffers
			hr = cBufferHandle->AddConstantBuffer(sizeof(DirectX::XMMATRIX), tarOff[0], &ID[0]);
			if (hr != S_OK)
			{
				return false;
			}
			hr = cBufferHandle->AddConstantBuffer(sizeof(DirectX::XMMATRIX), tarOff[1], &ID[1]);
			if (hr != S_OK)
			{
				return false;
			}
			hr = cBufferHandle->AddConstantBuffer(sizeof(DirectX::XMMATRIX), tarOff[2], &ID[2]);
			if (hr != S_OK)
			{
				return false;
			}

			//set Cbuffers
			cBufferHandle->SetConstantBuffer((void*)&world, ID[0]);
			cBufferHandle->SetConstantBuffer((void*)&view, ID[1]);
			cBufferHandle->SetConstantBuffer((void*)&projection, ID[2]);

			//remove buffers
			cBufferHandle->RemoveConstantBuffer(ID[0]);
			cBufferHandle->RemoveConstantBuffer(ID[1]);
			cBufferHandle->RemoveConstantBuffer(ID[2]);

			cBufferHandle->Shutdown();;
		#pragma endregion Constbuffer

		#pragma region objLoad
			auto& e = Core::Engine::GetInstance();
			auto& info = Core::Engine::InitializationInfo();
			auto re = e.Init(info);
			if (re)
			{
				console->Print("Could not init Core, Error: %d.", re);
				return false;
			}

			auto r = e.GetResourceHandler();
			bool result = false;

			r->AddParser(Utilz::GUID("objtest"), [](void* rawData, size_t rawSize, void** parsedData, size_t* parsedSize) -> int
			{
				ArfData::Data arfData;
				ArfData::DataPointers arfp;
				auto r = Arf::ParseObj(rawData, rawSize, &arfData, &arfp);
				if (r)
					return r;
				auto data = (Arf::Mesh::Data**)parsedData;
				r = Arf::Interleave(arfData, arfp, data, parsedSize, Arf::Mesh::InterleaveOption::Position);
				if (r)
					return r;
			});

			
			r->LoadResource(Utilz::GUID("test.objtest"), [&result, &deviceManager](void* data, size_t size)
			{
				auto& mD = *(Arf::Mesh::Data*)data;
				auto verts = (Arf::Mesh::Position*)mD.vertices;
				
				
				int vertexID[3];
				Graphics::StaticVertexBufferHandler* vertexBuffer = new Graphics::StaticVertexBufferHandler(deviceManager->GetDevice(), deviceManager->GetDeviceContext());
				//create vertexbuffer
				HRESULT hr = vertexBuffer->CreateVertexBuffer(verts, size - sizeof(Arf::Mesh::Data), &vertexID[0]);
				if (hr != S_OK)
				{
					result = false;
				}
				hr = vertexBuffer->CreateVertexBuffer(verts, size - sizeof(Arf::Mesh::Data), &vertexID[1]);
				if (hr != S_OK)
				{
					result = false;
				}
				hr = vertexBuffer->CreateVertexBuffer(verts, size - sizeof(Arf::Mesh::Data), &vertexID[2]);
				if (hr != S_OK)
				{
					result = false;
				}

				//set vertexBuffer
				vertexBuffer->SetVertexBuffer(vertexID[0]);
				vertexBuffer->SetVertexBuffer(vertexID[1]);
				vertexBuffer->SetVertexBuffer(vertexID[2]);

				//remove vertexBuffer
				vertexBuffer->RemoveVertexBuffer(vertexID[0]);
				vertexBuffer->RemoveVertexBuffer(vertexID[1]);
				vertexBuffer->RemoveVertexBuffer(vertexID[2]);

				vertexBuffer->Shutdown();

				result = true;
			});
		#pragma endregion objLoad
			e.Release();
			deviceManager->Shutdown();
			delete deviceManager;
			window->Shutdown();
			delete window;
			return true;
		}
	}
}
