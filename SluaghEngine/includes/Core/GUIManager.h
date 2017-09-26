#pragma once
#ifndef SE_CORE_GUI_MANAGER_H_
#define SE_CORE_GUI_MANAGER_H_
#include "EntityManager.h"
#include <Utilz\GUID.h>
#include <unordered_map>
#include <stack>
#include <vector>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>
#include <Graphics\GUIInfo.h>


namespace SE
{
	namespace Core
	{
		/**
		*
		* @brief This class is used to bind renderable text to an entity.
		*
		* @details Use this class when you want an entity to be rendered as a text in the GUI.
		*
		**/
		class GUIManager
		{
		public:
			GUIManager(ResourceHandler::IResourceHandler* resourceHandler, Graphics::IRenderer* renderer, const EntityManager& entityManager);
			~GUIManager();

			/**
			* @brief	Bind a renderable text to entity
			*
			* @param[in] entity The entity to bind the renderable text to.
			*
			*/
			void CreateRenderableText(const Entity& entity, Graphics::TextGUI textInfo);

			/**
			* @brief	Hide/Show the renderable object
			*
			* @param[in] entity Which entity.
			* @param[in] show True to show, false to hide.
			*
			*/
			void ToggleRenderableText(const Entity& entity, bool show);

			/**
			* @brief	Called each frame, to update the state.
			*/
			void Frame();

			void Shutdown();
		private:
			// Text variables
			std::unordered_map<Entity, size_t, EntityHasher> entID;
			std::vector<Graphics::TextGUI> loadedTexts;

			ResourceHandler::IResourceHandler* resourceHandler;
			Graphics::IRenderer* renderer;
			const EntityManager& entityManager;
		};
	}
}
#endif