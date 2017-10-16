#include "AudioTest.h"
#include <Core\Engine.h>
#include <portaudio\portaudio.h>
#include <Utilz\Memory.h>

#ifdef _DEBUG
#pragma comment(lib, "coreD.lib")
#pragma comment(lib, "windowD.lib")
#else
#pragma comment(lib, "core.lib")
#pragma comment(lib, "window.lib")
#endif

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

		bool AudioTest::Run(SE::Utilz::IConsoleBackend* console)
		{		
			auto& e = Core::Engine::GetInstance();
			auto& info = Core::Engine::InitializationInfo();
			auto re = e.Init(info);
			if (re)
			{
				console->Print("Could not init Core, Error: %d. \n", re);
				return false;
			}
			auto& audio = e.GetAudioManager();

			if (audio.LoadSound(Utilz::GUID("Canary.wav")) == 0)
			{
				console->Print("Sound already loaded??????\n");
				e.Release();
				return false;
			}

			int delay = 0;
			while (audio.CheckIfLoaded(Utilz::GUID("Canary.wav")) == 0 && delay < 10)
			{
				delay++;
			}

			auto soundEnt = e.GetEntityManager().Create();
			int soundID[10]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
			soundID[0] = audio.CreateStream(soundEnt, Utilz::GUID("Canary.wav"), Audio::SoundIndexName::BakgroundSound);
			if (soundID[0] == -1)
			{
				console->Print("Sound is not loaded!!!!!!!!\n");
				e.Release();
				return false;
			}
			else if (soundID[0] == -2)
			{
				console->Print("No device!!!!!!\n");
				e.Release();
				return false;
			}
			else
			{	
#pragma region GUI

				auto entText = e.GetEntityManager().Create();
				auto& guiManager = e.GetGUIManager();

				// Load textures for GUI
				guiManager.Create2D(Utilz::GUID("GUITest.sei"));

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
				
				guiManager.CreateRenderableText(entText, guiText);
				guiManager.ToggleRenderableText(entText, true);

				// GUI texture creation
				auto entTexture = e.GetEntityManager().Create();
				Graphics::GUITextureInfo guiTexture;
				guiTexture.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture.effect = DirectX::SpriteEffects_FlipBoth;
				guiTexture.textureID = 0;	// Not needed gets set in the bind function
				guiTexture.layerDepth = 0;
				guiTexture.origin = DirectX::XMFLOAT2(0.0, 0.0);
				guiTexture.pos = DirectX::XMFLOAT2(940.0, 560.0);
				guiTexture.rotation = 0;
				guiTexture.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture.rect = nullptr;	//not needed default nullptr

				
				guiManager.Bind2D(entTexture, Utilz::GUID("GUITest.sei"), guiTexture);
				guiManager.ToggleRenderableTexture(entTexture, true);

				// GUI texture creation2
				auto entTexture2 = e.GetEntityManager().Create();
				Graphics::GUITextureInfo guiTexture2;
				guiTexture2.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture2.effect = DirectX::SpriteEffects_FlipHorizontally;
				guiTexture2.textureID = 0;	// Not needed gets set in the bind function
				guiTexture2.layerDepth = 0;
				guiTexture2.origin = DirectX::XMFLOAT2(0.0, 0.0);
				guiTexture2.pos = DirectX::XMFLOAT2(940.0, 200.0);
				guiTexture2.rotation = 0;
				guiTexture2.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture2.rect = nullptr;	//not needed default nullptr


				guiManager.Bind2D(entTexture2, Utilz::GUID("GUITest.sei"), guiTexture2);
				guiManager.ToggleRenderableTexture(entTexture2, true);

				// GUI texture creation3
				auto entTexture3 = e.GetEntityManager().Create();
				Graphics::GUITextureInfo guiTexture3;
				guiTexture3.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture3.effect = DirectX::SpriteEffects_FlipVertically;
				guiTexture3.textureID = 0;	// Not needed gets set in the bind function
				guiTexture3.layerDepth = 0;
				guiTexture3.origin = DirectX::XMFLOAT2(0.0, 0.0);
				guiTexture3.pos = DirectX::XMFLOAT2(300.0, 200.0);
				guiTexture3.rotation = 0;
				guiTexture3.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture3.rect = nullptr;	//not needed default nullptr

				guiManager.Bind2D(entTexture3, Utilz::GUID("GUITest.sei"), guiTexture3);
				guiManager.ToggleRenderableTexture(entTexture3, true);

				// GUI texture creation4
				auto entTexture4 = e.GetEntityManager().Create();
				Graphics::GUITextureInfo guiTexture4;
				guiTexture4.colour = DirectX::XMFLOAT4(1.0, 1.0, 1.0, 1.0);
				guiTexture4.effect = DirectX::SpriteEffects_None;
				guiTexture4.textureID = 0;	// Not needed gets set in the bind function
				guiTexture4.layerDepth = 0;
				guiTexture4.origin = DirectX::XMFLOAT2(0.0, 0.0);
				guiTexture4.pos = DirectX::XMFLOAT2(300.0, 560.0);
				guiTexture4.rotation = 0;
				guiTexture4.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture4.rect = nullptr;	//not needed default nullptr


				guiManager.Bind2D(entTexture4, Utilz::GUID("GUITest.sei"), guiTexture4);
				guiManager.ToggleRenderableTexture(entTexture4, true);

#pragma endregion GUI


				audio.StreamSound(soundEnt, soundID[0]);
				e.GetWindow()->MapActionButton(0, Window::KeyEscape);
				e.GetWindow()->MapActionButton(1, Window::KeyW);
				e.GetWindow()->MapActionButton(2, Window::KeyS);
				e.GetWindow()->MapActionButton(3, Window::KeyR);
				e.GetWindow()->MapActionButton(4, Window::KeyV);
				e.GetWindow()->MapActionButton(5, Window::KeyM);
				e.GetWindow()->MapActionButton(6, Window::Key1);
				e.GetWindow()->MapActionButton(7, Window::Key2);
				e.GetWindow()->MapActionButton(8, Window::Key3);
				e.GetWindow()->MapActionButton(9, Window::Key4);
				e.GetWindow()->MapActionButton(10, Window::Key5);

				console->Print("Start main loop!!\n");
				auto ren = e.GetRenderer();
				while (e.GetWindow()->ButtonPressed(0) != true)
				{
					e.Frame(0.0f);
					
					if (e.GetWindow()->ButtonPressed(1) == true)
					{
						for (int i = 0; i < 10; i++)
						{
							if (soundID[i] == -1)
							{
								soundID[i] = audio.CreateStream(soundEnt, Utilz::GUID("Canary.wav"), Audio::SoundIndexName::BakgroundSound);
								if (soundID[i] == -2)
								{
									console->Print("No device!!!!!!\n");
								}
								else
								{
									audio.StreamSound(soundEnt, soundID[i]);
								}
								i = 11;
							}
						}
					}
					if (e.GetWindow()->ButtonPressed(2) == true)
					{
						for (int i = 0; i < 10; i++)
						{
							if (soundID[i] != -1)
							{
								audio.StopSound(soundEnt, soundID[i]);
								audio.RemoveSound(soundEnt, soundID[i]);
								soundID[i] = -1;
								i = 11;
							}
						}
					}
					if (e.GetWindow()->ButtonPressed(3) == true)
					{
						for (int i = 0; i < 10; i++)
						{
							if (soundID[i] != -1)
							{
								audio.StopSound(soundEnt, soundID[i]);
								audio.StreamSound(soundEnt, soundID[i]);
								i = 11;
							}
						}
					}
					if (e.GetWindow()->ButtonPressed(4) == true)
					{
						console->Print("VRam: %d \n", Utilz::Memory::toMB(ren->GetVRam()));
					}
					if (e.GetWindow()->ButtonPressed(5) == true)
					{
						size_t physMem = Utilz::Memory::toMB(Utilz::Memory::GetPhysicalProcessMemory());
						size_t virtMem = Utilz::Memory::toMB(Utilz::Memory::GetVirtualProcessMemory());
						console->Print("PhysicalProcessMemory: %d \nVirtualProcessMemory: %d \n", physMem, virtMem);
					}
				}
				e.Release();
				return true;
			}
			return false;
		}
	}
}
