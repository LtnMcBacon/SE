#include "HUD_Functionality.h"
#include <HUD_Parsing.h>
#include "CoreInit.h"

using namespace std;
namespace SE
{
	namespace Gameplay
	{
		HUDButtons::HUDButtons()
		{
		}

		HUDButtons::~HUDButtons()
		{
		}

		void HUDButtons::HoverButton(HUDInputCheck InputCheck)
		{
			
			

		}
		void HUDButtons::texHovered(HUDInputCheck InputCheck)
		{
			Core::IGUIManager::CreateInfo GuiManager;


			auto entity = CoreInit::managers.entityManager->Create();

			GuiManager.textureInfo.width = InputCheck.width;
			GuiManager.textureInfo.height = InputCheck.height;
			GuiManager.textureInfo.posX = InputCheck.posX;
			GuiManager.textureInfo.posY = InputCheck.posY;

			GuiManager.textureInfo.layerDepth = 1.0f - InputCheck.layerDepth / 1000.0f;

			GuiManager.texture = InputCheck.hoverTex;
			

			CoreInit::managers.guiManager->Create(entity, GuiManager);

			CoreInit::managers.guiManager->ToggleRenderableTexture(entity, true);
		}

		void HUDButtons::texPressed(HUDInputCheck InputCheck)
		{
			Core::IGUIManager::CreateInfo GuiManager;


			auto entity = CoreInit::managers.entityManager->Create();

			GuiManager.textureInfo.width = InputCheck.width;
			GuiManager.textureInfo.height = InputCheck.height;
			GuiManager.textureInfo.posX = InputCheck.posX;
			GuiManager.textureInfo.posY = InputCheck.posY;

			GuiManager.textureInfo.layerDepth = 1.0f - InputCheck.layerDepth / 1000.0f;

			GuiManager.texture = InputCheck.pressTex;

			CoreInit::managers.guiManager->Create(entity, GuiManager);

			CoreInit::managers.guiManager->ToggleRenderableTexture(entity, true);
		}

		void HUDButtons::texIdle(HUDInputCheck InputCheck)
		{
			Core::IGUIManager::CreateInfo GuiManager;


			auto entity = CoreInit::managers.entityManager->Create();

			GuiManager.textureInfo.width = InputCheck.width;
			GuiManager.textureInfo.height = InputCheck.height;
			GuiManager.textureInfo.posX = InputCheck.posX;
			GuiManager.textureInfo.posY = InputCheck.posY;

			GuiManager.textureInfo.layerDepth = 1.0f - InputCheck.layerDepth / 1000.0f;

			GuiManager.texture = InputCheck.idleTex;

			CoreInit::managers.guiManager->Create(entity, GuiManager);

			CoreInit::managers.guiManager->ToggleRenderableTexture(entity, true);
		}

	}
}

