#ifndef SE_INTERFACE_GUI_MANAGER_H_
#define SE_INTERFACE_GUI_MANAGER_H_

#if defined DLL_EXPORT_CORE
#define DECLDIR_CORE __declspec(dllexport)
#else
#define DECLDIR_CORE __declspec(dllimport)
#endif


#include <Utilz\GUID.h>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>
#include <Graphics\GUIInfo.h>

#include "IManager.h"
#include "IEntityManager.h"
#include <DevConsole\IConsole.h>

namespace SE
{
	namespace Core
	{
		/**
		*
		* @brief This class is used to bind renderable objects to an entity.
		*
		* @details Use this class when you want an entity to be rendered as a object in the world.
		*
		**/
		class IGUIManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				ResourceHandler::IResourceHandler* resourceHandler;
				Graphics::IRenderer* renderer;
				IEntityManager* entityManager;
				DevConsole::IConsole* console;
			};

			struct CreateInfo
			{
				
				Utilz::GUID texture;		
				Graphics::GUITextureInfo textureInfo;
			};


			virtual ~IGUIManager() {};

			/**
			* @brief Create a new 2D texture for GUI
			* @param[in] texFile The GUID to the texture file.
			* @retval 0+ Texture ID
			* @retval -1 Already loaded or currently loading.
			* @endcode
			*/
			virtual int Create(const Entity& entity,const CreateInfo& info) = 0;

			/**
			* @brief	Hide/Show the renderable texture
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			virtual void ToggleRenderableTexture(const Entity& entity, bool show) = 0;

			// sets for texture
			virtual  void SetTextureColour(const Entity& entity, DirectX::XMFLOAT4 colour) = 0;
			virtual  void SetTexturePos(const Entity& entity, long x, long y) = 0;

			virtual  void SetTextureScale(const Entity& entity, DirectX::XMFLOAT2 scale) = 0;


			virtual  void SetTextureScreenAnchor(const Entity& entity, DirectX::XMFLOAT2 anchor) = 0;
			virtual  void SetTextureDimensions(const Entity& entity, long width, long height) = 0;

			virtual  void SetTextureEffect(const Entity& entity, Graphics::Effect effect) = 0;

			virtual  void SetTextureRotation(const Entity& entity, float rotation) = 0;

			virtual void SetTexture(const Entity& entity, Utilz::GUID texture) = 0;

			virtual  void SetTextureLayerDepth(const Entity& entity, float layerDepth) = 0;
			/**
			* @brief Sets the default height and width to be used in GUI scale calc
			* @param[in] inHeight The height.
			* @param[in] inWidth The width.
			* @endcode
			*/
			virtual void SetDefaultScale(float inHeight, float inWidth) = 0;

			/**
			* @brief Resets all GUI to be rescaled to new resolution
			* @endcode
			*/
			virtual void updateGUI() = 0;

		protected:
			IGUIManager() {};
	
			IGUIManager(const IGUIManager& other) = delete;
			IGUIManager(const IGUIManager&& other) = delete;
			IGUIManager& operator=(const IGUIManager& other) = delete;
			IGUIManager& operator=(const IGUIManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the GUIManager
		**/
		DECLDIR_CORE IGUIManager* CreateGUIManager(const IGUIManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_GUI_MANAGER_H_