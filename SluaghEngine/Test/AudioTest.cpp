#include "AudioTest.h"
#include <Core\IEngine.h>
#include <Utilz\Memory.h>
#include <Utilz\Timer.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#pragma comment(lib, "windowD.lib")
#else
#pragma comment(lib, "core->lib")
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
			//if (managers.audioManager->LoadSound(Utilz::GUID("Canary.wav")) == 0)
			//{
			//	console->Print("Sound already loaded??????\n");
			//	e->Release();
			//	return false;
			//}

			//if (audio.LoadSound(Utilz::GUID("BLoop.wav")) == 0)
			//{
			//	console->Print("Sound already loaded??????\n");
			//	e->Release();
			//	return false;
			//}

			int delay = 0;
			//while (audio.CheckIfLoaded(Utilz::GUID("Canary.wav")) == 0 && audio.CheckIfLoaded(Utilz::GUID("BLoop.wav")) == 0 && delay < 10)
			//{
			//	delay++;
			//}

			auto soundEnt = managers.entityManager->Create();
		//	int soundID[10]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
			managers.audioManager->Create(soundEnt, { "BLoop.wav", Audio::SoundIndexName::BakgroundSound });
			/*if (soundID[0] == -1)
			{
				console->Print("Sound is not loaded!!!!!!!!\n");
				e->Release();
				return false;
			}
			else if (soundID[0] == -2)
			{
				console->Print("No device!!!!!!\n");
				e->Release();
				return false;
			}
			else*/
			{	
#pragma region GUI

				auto entText = managers.entityManager->Create();

				// Load textures for GUI
				managers.guiManager->Create2D(Utilz::GUID("GUITest.sei"));
				managers.guiManager->Create2D(Utilz::GUID("TransparentTest.sei"));

				// Text creation
				Graphics::TextGUI guiText;
				guiText.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiText.effect = DirectX::SpriteEffects_None;
				guiText.fontID = 0;
				guiText.text = L"Is this pizza heaven????";
				guiText.hashString = std::hash<std::wstring>()(guiText.text);
				guiText.layerDepth = 0;
				guiText.origin = DirectX::XMFLOAT2(0.0, 0.0);
				guiText.pos = DirectX::XMFLOAT2(0.0, 0.0);
				guiText.rotation = 0;
				guiText.scale = DirectX::XMFLOAT2(1.0, 1.0);
				
				managers.guiManager->CreateRenderableText(entText, guiText);
				managers.guiManager->ToggleRenderableText(entText, true);

				// GUI texture creation
				auto entTexture = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture;
				guiTexture.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture.effect = DirectX::SpriteEffects_FlipBoth;
				guiTexture.textureID = 0;	// Not needed gets set in the bind function
				guiTexture.layerDepth = 0.1;
				guiTexture.pos = DirectX::XMFLOAT2(940.0, 560.0);
				guiTexture.rotation = 0;
				guiTexture.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture.rect = nullptr;	//not needed default nullptr

				
				managers.guiManager->Bind2D(entTexture, Utilz::GUID("GUITest.sei"), guiTexture);
				managers.guiManager->ToggleRenderableTexture(entTexture, true);

				// GUI texture creation2
				auto entTexture2 = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture2;
				guiTexture2.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture2.effect = DirectX::SpriteEffects_FlipHorizontally;
				guiTexture2.textureID = 0;	// Not needed gets set in the bind function
				guiTexture2.layerDepth = 0.1;
				guiTexture2.pos = DirectX::XMFLOAT2(940.0, 200.0);
				guiTexture2.rotation = 0;
				guiTexture2.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture2.rect = nullptr;	//not needed default nullptr


				managers.guiManager->Bind2D(entTexture2, Utilz::GUID("GUITest.sei"), guiTexture2);
				managers.guiManager->ToggleRenderableTexture(entTexture2, true);

				// GUI texture creation3
				auto entTexture3 = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture3;
				guiTexture3.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture3.effect = DirectX::SpriteEffects_FlipVertically;
				guiTexture3.textureID = 0;	// Not needed gets set in the bind function
				guiTexture3.layerDepth = 0.1;
				guiTexture3.pos = DirectX::XMFLOAT2(300.0, 200.0);
				guiTexture3.rotation = 0;
				guiTexture3.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture3.rect = nullptr;	//not needed default nullptr

				managers.guiManager->Bind2D(entTexture3, Utilz::GUID("GUITest.sei"), guiTexture3);
				managers.guiManager->ToggleRenderableTexture(entTexture3, true);

				// GUI texture creation4
				auto entTexture4 = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture4;
				guiTexture4.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture4.effect = DirectX::SpriteEffects_None;
				guiTexture4.textureID = 0;	// Not needed gets set in the bind function
				guiTexture4.layerDepth = 0.1;
				guiTexture4.pos = DirectX::XMFLOAT2(300.0, 560.0);
				guiTexture4.rotation = 0;
				guiTexture4.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture4.rect = nullptr;	//not needed default nullptr


				managers.guiManager->Bind2D(entTexture4, Utilz::GUID("GUITest.sei"), guiTexture4);
				managers.guiManager->ToggleRenderableTexture(entTexture4, true);

				// GUI texture creation5
				auto entTexture5 = managers.entityManager->Create();
				Graphics::GUITextureInfo guiTexture5;
				guiTexture5.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture5.effect = DirectX::SpriteEffects_None;
				guiTexture5.textureID = 0;	// Not needed gets set in the bind function
				guiTexture5.layerDepth = 0;
				guiTexture5.pos = DirectX::XMFLOAT2(800.0, 400.0);
				guiTexture5.rotation = DirectX::XM_PIDIV2;
				guiTexture5.scale = DirectX::XMFLOAT2(0.5, 0.5);
				guiTexture5.rect = nullptr;	//not needed default nullptr


				managers.guiManager->Bind2D(entTexture5, Utilz::GUID("TransparentTest.sei"), guiTexture5);
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

				console->Print("Start main loop!!\n");
				Utilz::Timer time;
				time.Tick();
				float rotation = 0.0;
				while (subSystems.window->ButtonPressed(0) != true)
				{
					rotation += 0.0002 * time.GetDelta();
					managers.guiManager->SetTextureRotation(entTexture5, rotation);
					managers.guiManager->SetTextureRotation(entTexture4, rotation);
					
					if (subSystems.window->ButtonPressed(1) == true)
					{

						managers.audioManager->Create(soundEnt, { "Canary.wav", Audio::SoundIndexName::EffectSound });

						managers.audioManager->PlaySound(soundEnt, "Canary.wav");
						

					}
					if (subSystems.window->ButtonPressed(2) == true)
					{

						managers.audioManager->StopSound(soundEnt, "Canary.wav");
						managers.audioManager->RemoveSound(soundEnt, "Canary.wav");

					}
					if (subSystems.window->ButtonPressed(4) == true)
					{
						console->Print("VRam: %d \n", Utilz::Memory::toMB(subSystems.renderer->GetVRam()));
					}
					if (subSystems.window->ButtonPressed(5) == true)
					{
						size_t physMem = Utilz::Memory::toMB(Utilz::Memory::GetPhysicalProcessMemory());
						size_t virtMem = Utilz::Memory::toMB(Utilz::Memory::GetVirtualProcessMemory());
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
						subSystems.optionsHandler->SetOptionUnsignedInt("Window", "height", 1080);
						subSystems.optionsHandler->SetOptionUnsignedInt("Window", "width", 1920);
						subSystems.optionsHandler->Trigger();
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
