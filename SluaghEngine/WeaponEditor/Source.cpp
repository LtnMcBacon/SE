

#include <Core\IEngine.h>
#include <Imgui\imgui.h>
#include <fstream>
#include <filesystem>
namespace fs = std::experimental::filesystem;

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#else
#pragma comment(lib, "core.lib")
#endif
#ifdef _DEBUG
#pragma comment(lib, "ImGuiDX11SDLD.lib")
#else
#pragma comment(lib, "ImGuiDX11SDL.lib")
#endif
using namespace SE;


struct Weapon
{
	std::string name = "Name";
	int str = 0;
	int intl = 0;
	Utilz::GUID mesh;
	Utilz::GUID mat;
	Utilz::GUID shader;
	Utilz::GUID particleSystem;
};

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	auto engine = Core::CreateEngine();
	auto r = engine->Init();
	if (r < 0)
	{
		delete engine;
		return false;
	}

	auto managers = engine->GetManagers();
	auto subSystem = engine->GetSubsystems();
	ImGui::SetCurrentContext((ImGuiContext*)subSystem.devConsole->GetContext());


	std::vector<Utilz::GUID> guidsMesh;
	std::vector<std::string> namesMesh;
	subSystem.resourceHandler->GetAllGUIDsWithExtension("mesh", guidsMesh, namesMesh);

	std::vector<Utilz::GUID> guidsMat;
	std::vector<std::string> namesMat;
	subSystem.resourceHandler->GetAllGUIDsWithExtension("mat", guidsMat, namesMat);

	std::vector<Utilz::GUID> guidsSha;
	std::vector<std::string> namesSha;
	subSystem.resourceHandler->GetAllGUIDsWithExtension("hlsl", guidsSha, namesSha);
	
	std::vector<Utilz::GUID> guidsPar;
	std::vector<std::string> namesPar;
	subSystem.resourceHandler->GetAllGUIDsWithExtension("txt", guidsPar, namesPar);







	auto& weaponEnt = managers.entityManager->Create();
	Core::Entity particleEnt;
	auto& camera = managers.entityManager->Create();

	managers.cameraManager->Create(camera);
	managers.cameraManager->SetActive(camera);
	managers.transformManager->SetRotation(camera, 0.0f, 0.0f, 0.0f);
	managers.transformManager->SetPosition(camera, { 0.0f, 1.0f, -5.0f });


	managers.transformManager->Create(weaponEnt);
	managers.transformManager->SetPosition(weaponEnt, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	managers.transformManager->SetRotation(weaponEnt, 0.0f, 3.14f, 0.0f);


	Weapon weaponInfo;

	weaponInfo.shader = "default.mat";
	weaponInfo.mat = "SimpleLightPS.hlsl";

	managers.materialManager->Create(weaponEnt, { weaponInfo.shader,weaponInfo.mat });

	weaponInfo.mesh = "default.mesh";
	managers.renderableManager->CreateRenderableObject(weaponEnt, { weaponInfo.mesh });
	managers.renderableManager->ToggleRenderableObject(weaponEnt, true);

	auto& l = managers.entityManager->Create();
	Core::ILightManager::CreateInfo d;
	d.radius = 100.0f;
	d.pos = { 0.0f, 5.0f, -5.0f };
	d.color = { 1, 1,1 };
	managers.lightManager->Create(l, d);
	managers.lightManager->ToggleLight(l, true);

	subSystem.window->MapActionButton(0, Window::KeyEscape);

	bool running = true;

	char buffer[256] = { "Name\0" };
	char buffer2[256] = { "Name\0" };
	int currentMesh = 0;
	int currentMat = 0;
	int currentSha = 0;
	int currentPar = 0;
	while (running)
	{
		if (subSystem.window->ButtonPressed(0))
			running = false;

		engine->BeginFrame();
		ImGui::Begin("Save/Load");
		if (ImGui::InputText("Filename", buffer2, 256))
		{
			weaponInfo.name = buffer2;
		}

		if (ImGui::Button("Save"))
		{
			fs::create_directory("Weapons");
			std::string fn = "Weapons/" + std::string(buffer2);
			std::ofstream out(fn, std::ios::binary | std::ios::trunc);
			if (out.is_open())
			{
				out.write((char*)&weaponInfo, sizeof(Weapon));
				out.close();
			}

		}
		if (ImGui::Button("Load"))
		{
			std::string fn = "Weapons/" + std::string(buffer2);
			std::ifstream out(fn, std::ios::binary);
			if (out.is_open())
			{
				out.read((char*)&weaponInfo, sizeof(Weapon));
				out.close();
				managers.materialManager->Create(weaponEnt, { weaponInfo.shader,weaponInfo.mat });
				managers.renderableManager->CreateRenderableObject(weaponEnt, { weaponInfo.mesh });

				if (!(weaponInfo.particleSystem == 0))
				{
					managers.particleSystemManager->CreateSystem(particleEnt, { weaponInfo.particleSystem });
				}
			}
		}
		ImGui::End();

		ImGui::Begin("Stats");

		
		if (ImGui::InputText("Name", buffer, 256))
		{
			weaponInfo.name = buffer;
		}
		ImGui::InputInt("Strength", &weaponInfo.str);
		ImGui::InputInt("Intelligence", &weaponInfo.intl);

		ImGui::End();

		ImGui::Begin("Render Info");
		ImGui::ListBox("Meshes", &currentMesh, [](void*data, int n, const char** out) {
			auto& v = *(std::vector<std::string>*)data;
			if (n < v.size())
			{
				*out = v[n].c_str();
				return true;
			}

			return false;
		}, &namesMesh, namesMesh.size(), 5);

		if (ImGui::Button("ChangeMesh"))
		{
			weaponInfo.mesh = guidsMesh[currentMesh];
			managers.renderableManager->CreateRenderableObject(weaponEnt, { guidsMesh[currentMesh] });
		}
		ImGui::ListBox("Materials", &currentMat, [](void*data, int n, const char** out) {
			auto& v = *(std::vector<std::string>*)data;
			if (n < v.size())
			{
				*out = v[n].c_str();
				return true;
			}

			return false;
		}, &namesMat, namesMat.size(), 5);

		if (ImGui::Button("ChangeMat"))
		{
			weaponInfo.mat = guidsMat[currentMat];
			managers.materialManager->Create(weaponEnt, { weaponInfo.shader,weaponInfo.mat });
		}

		ImGui::ListBox("Shaders", &currentSha, [](void*data, int n, const char** out) {
			auto& v = *(std::vector<std::string>*)data;
			if (n < v.size())
			{
				*out = v[n].c_str();
				return true;
			}

			return false;
		}, &namesSha, namesSha.size(), 5);

		if (ImGui::Button("ChangeShader"))
		{
			weaponInfo.shader = guidsSha[currentSha];
			managers.materialManager->Create(weaponEnt, { weaponInfo.shader,weaponInfo.mat });
		}

		ImGui::ListBox("Particle Systems", &currentSha, [](void*data, int n, const char** out) {
			auto& v = *(std::vector<std::string>*)data;
			if (n < v.size())
			{
				*out = v[n].c_str();
				return true;
			}

			return false;
		}, &namesPar, namesPar.size(), 5);

		if (ImGui::Button("Change ParticleSystem"))
		{
			weaponInfo.particleSystem = guidsPar[currentPar];
			managers.particleSystemManager->CreateSystem(particleEnt, { weaponInfo.particleSystem });
		}
		ImGui::End();

		engine->EndFrame();
	}


	engine->Release(); delete engine;
}