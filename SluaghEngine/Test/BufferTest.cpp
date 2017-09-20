#include "BufferTest.h"
#include <d3d11.h>
#include <DirectXMath.h>

#include <Graphics\GraphicResourceHandler.h>
#include <Graphics\Renderer.h>
#include <Window\WindowSDL.h>

#include "ObjLoaderTest.h"
#include <Core\Engine.h>
#include <Graphics\Renderer.h>
#include <OBJParser\Parsers.h>

#include <Profiler.h>

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
		static Graphics::DeviceManager* deviceManager = nullptr;
		Window::IWindow* window = nullptr;
		static bool result = false;
		bool BufferTest::Run(Utilz::IConsoleBackend* console)
		{
			StartProfile;
			window = new Window::WindowSDL();
			int isOK = window->Initialize();
			if (isOK != 0)
			{
				ProfileReturnConst( isOK);
			}
			deviceManager = new Graphics::DeviceManager();
			HRESULT hr = deviceManager->Init((HWND)window->GetHWND());
			if (hr != S_OK)
			{
				ProfileReturnConst( false);
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

			Graphics::GraphicResourceHandler* gResourceHandler = new Graphics::GraphicResourceHandler(deviceManager->GetDevice(), deviceManager->GetDeviceContext());
			//create Cbuffers
			hr = gResourceHandler->CreateConstantBuffer(sizeof(DirectX::XMMATRIX), tarOff[0], &ID[0]);
			if (hr != S_OK)
			{
				ProfileReturnConst(false);
			}
			hr = gResourceHandler->CreateConstantBuffer(sizeof(DirectX::XMMATRIX), tarOff[1], &ID[1]);
			if (hr != S_OK)
			{
				ProfileReturnConst(false);
			}
			hr = gResourceHandler->CreateConstantBuffer(sizeof(DirectX::XMMATRIX), tarOff[2], &ID[2]);
			if (hr != S_OK)
			{
				ProfileReturnConst(false);
			}

			//set Cbuffers
			gResourceHandler->SetConstantBuffer((void*)&world, ID[0]);
			gResourceHandler->SetConstantBuffer((void*)&view, ID[1]);
			gResourceHandler->SetConstantBuffer((void*)&projection, ID[2]);

			//remove buffers
			gResourceHandler->RemoveConstantBuffer(ID[0]);
			gResourceHandler->RemoveConstantBuffer(ID[1]);
			gResourceHandler->RemoveConstantBuffer(ID[2]);

			gResourceHandler->Shutdown();
			delete gResourceHandler;
		#pragma endregion Constbuffer

		#pragma region objLoad
			auto& e = Core::Engine::GetInstance();
			auto& info = Core::Engine::InitializationInfo();
			auto re = e.Init(info);
			if (re)
			{
				console->Print("Could not init Core, Error: %d.", re);
				ProfileReturnConst(false);
			}

			auto r = e.GetResourceHandler();
			result = false;


			
			r->LoadResource(Utilz::GUID("test.objtest"), ResourceHandler::LoadResourceDelegate::Make<BufferTest, &BufferTest::Load>(this));
		#pragma endregion objLoad
			e.Release();
			deviceManager->Shutdown();
			delete deviceManager;
			window->Shutdown();
			delete window;
			ProfileReturnConst(true);;
		}
		int BufferTest::Load(const Utilz::GUID & guid, void * data, size_t size)
		{
			ArfData::Data arfData;
			ArfData::DataPointers arfp;
			auto r = Arf::ParseObj(data, size, &arfData, &arfp);
			if (r)
				return r;
			Arf::Mesh::Data* parsedData;
			size_t parsedSize;
			r = Arf::Interleave(arfData, arfp, &parsedData, &parsedSize, Arf::Mesh::InterleaveOption::Position);
			if (r)
				return r;

			delete arfp.buffer;

			auto& mD = *(Arf::Mesh::Data*)parsedData;
			auto verts = (Arf::Mesh::Position*)mD.vertices;


			int vertexID[3];
			Graphics::GraphicResourceHandler* gResourceHandler = new Graphics::GraphicResourceHandler(deviceManager->GetDevice(), deviceManager->GetDeviceContext());
			//create vertexbuffer
			HRESULT hr = gResourceHandler->CreateVertexBuffer(verts, mD.NumVertices, sizeof(float) * 3, &vertexID[0]);
			if (hr != S_OK)
			{
				result = false;
			}
			hr = gResourceHandler->CreateVertexBuffer(verts, mD.NumVertices, sizeof(float) * 3, &vertexID[1]);
			if (hr != S_OK)
			{
				result = false;
			}
			hr = gResourceHandler->CreateVertexBuffer(verts, mD.NumVertices, sizeof(float) * 3, &vertexID[2]);
			if (hr != S_OK)
			{
				result = false;
			}

			//set vertexBuffer
			gResourceHandler->SetVertexBuffer(vertexID[0]);
			gResourceHandler->SetVertexBuffer(vertexID[1]);
			gResourceHandler->SetVertexBuffer(vertexID[2]);

			//remove vertexBuffer
			gResourceHandler->RemoveVertexBuffer(vertexID[0]);
			gResourceHandler->RemoveVertexBuffer(vertexID[1]);
			gResourceHandler->RemoveVertexBuffer(vertexID[2]);

			gResourceHandler->Shutdown();
			delete gResourceHandler;

			result = true;

			return 0;
		}
	}
}
