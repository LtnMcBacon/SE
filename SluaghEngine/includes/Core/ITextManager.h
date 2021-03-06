#ifndef SE_INTERFACE_2DTEXT_MANAGER_H_
#define SE_INTERFACE_2DTEXT_MANAGER_H_

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
		class ITextManager : public IManager
		{
		public:
			struct InitializationInfo
			{
				ResourceHandler::IResourceHandler* resourceHandler;
				Graphics::IRenderer* renderer;
				IEntityManager* entityManager;
			};

			struct CreateInfo
			{
				Utilz::GUID font;
				Graphics::TextGUI info;
			};


			virtual ~ITextManager() {};

			/**
			* @brief	Bind a renderable text to entity
			*
			* @param[in] entity The entity to bind the renderable text to.
			* @param[in] textInfo Struct with the required information.
			*
			*/
			virtual void Create(const Entity& entity, const CreateInfo& info) = 0;

			/**
			* @brief Create a new font
			* @param[in] fontFile The GUID to the font file.
			* @retval 0+ Font ID
			* @retval -1 Something went wrong.
			* @endcode
			*/
			virtual int MakeFont(const Utilz::GUID& fontFile) = 0;

			/**
			* @brief	Hide/Show the renderable text
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			virtual void ToggleRenderableText(const Entity& entity, bool show) = 0;

			// sets for Text
			virtual  void SetText(const Entity& entity, std::wstring text) = 0;
			virtual  void SetTextFontID(const Entity& entity, size_t fontID) = 0;
			virtual  void SetTextColour(const Entity& entity, DirectX::XMFLOAT4 colour) = 0;
			virtual  void SetTextPos(const Entity& entity, long x, long y) = 0;
			virtual  void SetTextScreenAnchor(const Entity& entity, DirectX::XMFLOAT2 anchor) = 0;

			virtual  void SetTextScale(const Entity& entity, DirectX::XMFLOAT2 scale) = 0;

			virtual  void SetTextEffect(const Entity& entity, Graphics::Effect effect) = 0;

			virtual  void SetTextRotation(const Entity& entity, float rotation) = 0;
			virtual  void SetTextLayerDepth(const Entity& entity, float layerDepth) = 0;

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
			virtual void updateText() = 0;

		protected:
			ITextManager() {};

			ITextManager(const ITextManager& other) = delete;
			ITextManager(const ITextManager&& other) = delete;
			ITextManager& operator=(const ITextManager& other) = delete;
			ITextManager& operator=(const ITextManager&& other) = delete;
		};


		/**
		* @brief Create an instance of the GUIManager
		**/
		DECLDIR_CORE ITextManager* CreateTextManager(const ITextManager::InitializationInfo& initInfo);
	}
}

#endif //SE_INTERFACE_GUI_MANAGER_H_