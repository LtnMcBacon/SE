#include "AudioTest.h"
#include <Core\IEngine.h>
#include <Utilz\Memory.h>
#include <Utilz\Timer.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#pragma comment(lib, "windowD.lib")
#else
#pragma comment(lib, "Core.lib")
#pragma comment(lib, "window.lib")
#endif
#undef PlaySound

namespace SE
{
	namespace Test
	{
		AudioTest::AudioTest() 
		{

		}
		AudioTest::~AudioTest()
		{

		}

		bool AudioTest::Run(SE::DevConsole::IConsole* console)
		{		
			auto e = Core::CreateEngine();
			auto re = e->Init();
			if (re)
			{
				console->Print("Could not init Core, Error: %d. \n", re);
				return false;
			}
			auto managers = e->GetManagers();
			auto subSystems = e->GetSubsystems();

			auto soundEnt = managers.entityManager->Create();
			managers.audioManager->Create(soundEnt, { "BLoop.wav", Audio::SoundIndexName::BakgroundLoopSound });

			{	
#pragma region GUI

				auto entText = managers.entityManager->Create();

			//	// Text creation
				Graphics::TextGUI guiText;
				guiText.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiText.effect = Graphics::Effect::NoEffect;
				guiText.text = L"Is this pizza heaven????";
				guiText.hashString = std::hash<std::wstring>()(guiText.text);
				guiText.layerDepth = 0;
				guiText.anchor = DirectX::XMFLOAT2(0.5, 0.5);
				guiText.screenAnchor = DirectX::XMFLOAT2(0.5, 0.5);
				guiText.posX = 0;
				guiText.posY = 0;
				guiText.width = 300;
				guiText.height = 100;
				guiText.rotation = 0;
				guiText.scale = DirectX::XMFLOAT2(2.0, 1.0);
				
				managers.textManager->Create(entText, { Utilz::GUID(), guiText });
				managers.textManager->ToggleRenderableText(entText, true);

				// GUI texture creation
				auto entTexture = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture;
				guiTexture.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture.effect = Graphics::Effect::BothFlipEffect;
				guiTexture.layerDepth = 0.1;
				guiTexture.posX = 0;
				guiTexture.posY = 0;
				guiTexture.rotation = 0;
				guiTexture.width = -1;
				guiTexture.height = -1;
				guiTexture.screenAnchor = { 0.25f ,0.25f };
				guiTexture.anchor = { 0.5f, 0.5f };
				guiTexture.scale = { 1.0f, 1.0f };
				//guiTexture.rect = nullptr;	//not needed default nullptr

				managers.guiManager->Create( entTexture, { Utilz::GUID("GUITest.sei"), guiTexture });
				managers.guiManager->ToggleRenderableTexture(entTexture, true);

				// GUI texture creation2
				auto entTexture2 = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture2;
				guiTexture2.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture2.effect = Graphics::Effect::HoriFlipEffect;
				guiTexture2.layerDepth = 0.1;
				guiTexture2.posX = 0;
				guiTexture2.posY = 0;
				guiTexture2.rotation = 0;
				guiTexture2.width = -1;
				guiTexture2.height = -1;
				guiTexture2.screenAnchor = { 0.75f ,0.25f };
				guiTexture2.anchor = { 0.5f, 0.5f };
				//guiTexture2.rect = nullptr;	//not needed default nullptr


				managers.guiManager->Create( entTexture2, {Utilz::GUID("GUITest.sei"), guiTexture2 });
				managers.guiManager->ToggleRenderableTexture(entTexture2, true);

				// GUI texture creation3
				auto entTexture3 = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture3;
				guiTexture3.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture3.effect = Graphics::Effect::VertiFlipEffect;
				guiTexture3.layerDepth = 0.1;
				guiTexture3.posX = 0;
				guiTexture3.posY = 0;
				guiTexture3.rotation = 0;
				guiTexture3.width = -1;
				guiTexture3.height = -1;
				guiTexture3.screenAnchor = { 0.25f ,0.75f };
				guiTexture3.anchor = { 0.5f, 0.5f };
		
				managers.guiManager->Create( entTexture3,{ Utilz::GUID("GUITest.sei"), guiTexture3 });
				managers.guiManager->ToggleRenderableTexture(entTexture3, true);

				// GUI texture creation4
				auto entTexture4 = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture4;
				guiTexture4.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture4.effect = Graphics::Effect::NoEffect;
				guiTexture4.layerDepth = 0.1;
				guiTexture4.posX = 0;
				guiTexture4.posY = 0;
				guiTexture4.rotation = 0;
				guiTexture4.width = -1;
				guiTexture4.height = -1;
				guiTexture4.screenAnchor = { 0.75f ,0.75f };
				guiTexture4.anchor = { 0.5f, 0.5f };
				//guiTexture4.rect = nullptr;	//not needed default nullptr


				managers.guiManager->Create( entTexture4,{ Utilz::GUID("GUITest.sei"), guiTexture4 });
				managers.guiManager->ToggleRenderableTexture(entTexture4, true);

				// GUI texture creation5
				auto entTexture5 = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture5;
				guiTexture5.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture5.effect = Graphics::Effect::NoEffect;
				guiTexture5.layerDepth = 0.1;
				guiTexture5.screenAnchor = { 0.5f ,0.5f };
				guiTexture5.posX = 0;
				guiTexture5.posY = 0;
				guiTexture5.rotation = 0;
				guiTexture5.width = -1;
				guiTexture5.height = -1;
				guiTexture5.scale = { 0.25f, 0.25f };
				guiTexture5.anchor = { 0.5f, 0.5f };
				guiTexture5.absolute = false;
				//guiTexture5.rect = nullptr;	//not needed default nullptr


				managers.guiManager->Create( entTexture5,{ Utilz::GUID("TransparentTest.sei"), guiTexture5 });
				managers.guiManager->ToggleRenderableTexture(entTexture5, true);

#pragma endregion GUI


				managers.audioManager->PlaySound(soundEnt, "BLoop.wav");
				subSystems.window->MapActionButton(0, Window::KeyEscape);
				subSystems.window->MapActionButton(1, Window::KeyW);
				subSystems.window->MapActionButton(2, Window::KeyS);
				subSystems.window->MapActionButton(3, Window::KeyR);
				subSystems.window->MapActionButton(4, Window::KeyV);
				subSystems.window->MapActionButton(5, Window::KeyM);
				subSystems.window->MapActionButton(11, Window::KeyU);
				subSystems.window->MapActionButton(12, Window::KeyJ);
				subSystems.window->MapActionButton(13, Window::KeyO);
				subSystems.window->MapActionButton(14, Window::KeyL);

				console->Print("Start main loop!!\n");
				Utilz::Timer time;
				time.Tick();
				float rotation = 0.0;
				while (subSystems.window->ButtonPressed(0) != true)
				{
					rotation += 0.0002 * time.GetDelta();
					managers.guiManager->SetTextureRotation(entTexture5, rotation);
				//	managers.guiManager->SetTextureRotation(entTexture4, rotation);
					
					if (subSystems.window->ButtonPressed(1) == true)
					{

						managers.audioManager->Create(soundEnt, { "Canary.wav", Audio::SoundIndexName::BakgroundSound });

						managers.audioManager->PlaySound(soundEnt, "Canary.wav");
						

					}
					if (subSystems.window->ButtonPressed(2) == true)
					{

						managers.audioManager->StopSound(soundEnt, "Canary.wav");
						managers.audioManager->RemoveSound(soundEnt, "Canary.wav");

					}
					if (subSystems.window->ButtonPressed(4) == true)
					{
						console->Print("VRam: %d \n", toMB(subSystems.renderer->GetVRam()));
					}
					if (subSystems.window->ButtonPressed(5) == true)
					{
						size_t physMem = toMB(Utilz::Memory::GetPhysicalProcessMemory());
						size_t virtMem = toMB(Utilz::Memory::GetVirtualProcessMemory());
						console->Print("PhysicalProcessMemory: %d \nVirtualProcessMemory: %d \n", physMem, virtMem);
					}
					if (subSystems.window->ButtonPressed(11) == true)
					{
						subSystems.optionsHandler->SetOptionUnsignedInt("Window", "height", 720);
						subSystems.optionsHandler->SetOptionUnsignedInt("Window", "width", 1280);
						subSystems.optionsHandler->Trigger();
					}
					if (subSystems.window->ButtonPressed(12) == true)
					{
						subSystems.optionsHandler->SetOptionUnsignedInt("Window", "height", 900);
						subSystems.optionsHandler->SetOptionUnsignedInt("Window", "width", 1440);
						subSystems.optionsHandler->Trigger();
					}
					if (subSystems.window->ButtonPressed(13) == true)
					{
						managers.guiManager->SetTexture(entTexture3, Utilz::GUID("GUITest.sei"));
					}
					if (subSystems.window->ButtonPressed(14) == true)
					{
						managers.guiManager->SetTexture(entTexture3, Utilz::GUID("TransparentTest.sei"));
					}
					e->BeginFrame();
					e->EndFrame();
					time.Tick();
				}
				e->Release();
				delete e;
				return true;
			}
			return false;
		}
	}
}
