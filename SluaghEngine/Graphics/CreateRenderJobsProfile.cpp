#include <Window\IWindow.h>
#include <ResourceHandler\IResourceHandler.h>
#include <Profiler.h>
#include <VertexStructs.h>
#include "Renderer.h"


#pragma comment(lib, "Window.lib")
#pragma comment(lib, "ResourceHandler.lib")
using namespace SE;


int main(int argc, char* argv[])
{
	StartProfile;

	try
	{
		auto rh = ResourceHandler::CreateResourceHandler();

		auto r = new Graphics::Renderer;
		auto w = Window::CreateNewWindow();

		rh->Initialize();
		w->Initialize(Window::InitializationInfo());
		r->Initialize(w->GetHWND());

			Graphics::RenderObjectInfo info;

			rh->LoadResource("SimpleVS.hlsl", [r, &info](auto guid, auto data, auto size) {
				info.vertexShader = r->CreateVertexShader(data, size);
				if (info.vertexShader == -1)
					throw "error";

				return 1;
			});

			rh->LoadResource("SimplePS.hlsl", [r, &info](auto guid, auto data, auto size) {
				info.pixelShader = r->CreatePixelShader(data, size);
				if (info.vertexShader == -1)
					throw "error";

				return 1;
			});

			rh->LoadResource("Cube.mesh", [r, &info](auto guid, auto data, auto size) {
				auto meshHeader = (Graphics::Mesh_Header*)data;
				Vertex* v = (Vertex*)(meshHeader + 1);
				info.bufferHandle = r->CreateVertexBuffer(v, meshHeader->nrOfVertices, sizeof(Vertex));
				if (info.bufferHandle == -1)
					throw "error";
				return 1;
			
			});

			int rj[10000];
			for(int i =0;i<10000;i++)
				rj[i] = r->EnableRendering(info);

		using namespace std::chrono_literals;
		DirectX::XMFLOAT4X4 id;
		DirectX::XMMATRIX mat = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4(&id, mat);
		for (int i = 0; i < 10000; i++)
			for (int j = 0; j < 10000; j++)
				r->UpdateTransform(rj[j], (float*)&id);

		std::this_thread::sleep_for(1s);

		r->Shutdown();
		w->Shutdown();
		rh->Shutdown();

		delete r;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
	StopProfile;
	return 0;
}