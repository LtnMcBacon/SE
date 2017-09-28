#include "AudioTest.h"
#include <Core\Engine.h>
#include <portaudio\portaudio.h>
#include <Utilz\MemoryMeasuring.h>

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
			Utilz::MemoryMeasuring mm;
			mm.Init();
			auto& e = Core::Engine::GetInstance();
			auto& info = Core::Engine::InitializationInfo();
			auto re = e.Init(info);
			if (re)
			{
				console->Print("Could not init Core, Error: %d.", re);
				return false;
			}
			auto& audio = e.GetAudioManager();
			auto& optHandler = Core::Engine::GetInstance().GetOptionHandler();

			if (audio.LoadSound(Utilz::GUID("Cout.wav")) == 0)
			{
				console->Print("Sound already loaded??????");
				e.Release();
				return false;
			}

			while (audio.CheckIfLoaded(Utilz::GUID("Cout.wav")) == 0)
			{

			}

			auto soundEnt = e.GetEntityManager().Create();
			int soundID[10]{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
			soundID[0] = audio.CreateStream(soundEnt, Utilz::GUID("Cout.wav"), Audio::SoundIndexName::EffectSound);
			if (soundID[0] == -1)
			{
				console->Print("Sound is not loaded!!!!!!!!");
				e.Release();
				return false;
			}
			else if (soundID[0] == -2)
			{
				console->Print("No device!!!!!!");
				e.Release();
				return false;
			}
			else
			{
				// Object creation
				auto ent = e.GetEntityManager().Create();
				e.GetRenderableManager().CreateRenderableObject(ent, Utilz::GUID("Placeholder_Arrow.obj"));
				e.GetTransformManager().Create(ent);
				e.GetRenderableManager().ToggleRenderableObject(ent, true);

				
				auto entText = e.GetEntityManager().Create();
				auto& guiManager = e.GetGUIManager();

				// Load textures for GUI
				guiManager.Create2D(Utilz::GUID("GUITest.sei"));

				// Text creation
				Graphics::TextGUI guiText;
				guiText.colour = DirectX::XMFLOAT3(0.5, 0.5, 0.4);
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
				guiTexture.colour = DirectX::XMFLOAT3(0.5, 0.5, 0.4);
				guiTexture.effect = DirectX::SpriteEffects_FlipBoth;
				guiTexture.textureID = 0;	// Not needed gets set in the bind function
				guiTexture.layerDepth = 0;
				guiTexture.origin = DirectX::XMFLOAT2(-640.0, -360.0);
				guiTexture.pos = DirectX::XMFLOAT2(0.0, 0.0);
				guiTexture.rotation = 0;
				guiTexture.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture.rect = nullptr;	//not needed default nullptr

				
				guiManager.Bind2D(entTexture, Utilz::GUID("GUITest.sei"), guiTexture);
				guiManager.ToggleRenderableTexture(entTexture, true);

				// GUI texture creation2
				auto entTexture2 = e.GetEntityManager().Create();
				Graphics::GUITextureInfo guiTexture2;
				guiTexture2.colour = DirectX::XMFLOAT3(0.5, 0.5, 0.4);
				guiTexture2.effect = DirectX::SpriteEffects_FlipHorizontally;
				guiTexture2.textureID = 0;	// Not needed gets set in the bind function
				guiTexture2.layerDepth = 0;
				guiTexture2.origin = DirectX::XMFLOAT2(-640.0, 0.0);
				guiTexture2.pos = DirectX::XMFLOAT2(0.0, 0.0);
				guiTexture2.rotation = 0;
				guiTexture2.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture2.rect = nullptr;	//not needed default nullptr


				guiManager.Bind2D(entTexture2, Utilz::GUID("GUITest.sei"), guiTexture2);
				guiManager.ToggleRenderableTexture(entTexture2, true);

				// GUI texture creation3
				auto entTexture3 = e.GetEntityManager().Create();
				Graphics::GUITextureInfo guiTexture3;
				guiTexture3.colour = DirectX::XMFLOAT3(0.5, 0.5, 0.4);
				guiTexture3.effect = DirectX::SpriteEffects_FlipVertically;
				guiTexture3.textureID = 0;	// Not needed gets set in the bind function
				guiTexture3.layerDepth = 0;
				guiTexture3.origin = DirectX::XMFLOAT2(0.0, 0.0);
				guiTexture3.pos = DirectX::XMFLOAT2(0.0, 0.0);
				guiTexture3.rotation = 0;
				guiTexture3.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture3.rect = nullptr;	//not needed default nullptr


				guiManager.Bind2D(entTexture3, Utilz::GUID("GUITest.sei"), guiTexture3);
				guiManager.ToggleRenderableTexture(entTexture3, true);

				// GUI texture creation4
				auto entTexture4 = e.GetEntityManager().Create();
				Graphics::GUITextureInfo guiTexture4;
				guiTexture4.colour = DirectX::XMFLOAT3(0.5, 0.5, 0.4);
				guiTexture4.effect = DirectX::SpriteEffects_None;
				guiTexture4.textureID = 0;	// Not needed gets set in the bind function
				guiTexture4.layerDepth = 0;
				guiTexture4.origin = DirectX::XMFLOAT2(0.0, -360.0);
				guiTexture4.pos = DirectX::XMFLOAT2(0.0, 0.0);
				guiTexture4.rotation = 0;
				guiTexture4.scale = DirectX::XMFLOAT2(1.0, 1.0);
				guiTexture4.rect = nullptr;	//not needed default nullptr


				guiManager.Bind2D(entTexture4, Utilz::GUID("GUITest.sei"), guiTexture4);
				guiManager.ToggleRenderableTexture(entTexture4, true);

				audio.StreamSound(soundEnt, soundID[0]);
				e.GetWindow()->MapActionButton(0, Window::KeyEscape);
				e.GetWindow()->MapActionButton(1, Window::KeyW);
				e.GetWindow()->MapActionButton(2, Window::KeyS);
				e.GetWindow()->MapActionButton(3, Window::KeyR);

				while (e.GetWindow()->ButtonPressed(0) != true)
				{
					e.Frame(0.0f);
					//mm.printUsage(console);
					
					if (e.GetWindow()->ButtonPressed(1) == true)
					{
						for (int i = 0; i < 10; i++)
						{
							if (soundID[i] == -1)
							{
								soundID[i] = audio.CreateStream(soundEnt, Utilz::GUID("Cout.wav"), Audio::SoundIndexName::EffectSound);
								if (soundID[i] == -2)
								{
									console->Print("No device!!!!!!");
								}
								else
								{
									audio.StreamSound(soundEnt, soundID[i]);
								}
								i = 11;
							}
						}
						/*optHandler.SetOption("Window", "width", 800);
						optHandler.SetOption("Window", "height", 600);
						optHandler.SetOption("Window", "fullScreen", 0);
						optHandler.Trigger();*/
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
						/*optHandler.SetOption("Window", "width", 1280);
						optHandler.SetOption("Window", "height", 720);
						optHandler.SetOption("Window", "fullScreen", 1);
						optHandler.Trigger();*/
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
				}
				e.Release();
				return true;
			}
			return false;
		}
	}
}
