#include <Core\GUIManager.h>
#include <Profiler.h>
#include <Utilz\Console.h>
#include <OBJParser\Parsers.h>
#include <list>
#include "GUIMANAGER.H"

#ifdef _DEBUG
#pragma comment(lib, "OBJParserD.lib")
#else
#pragma comment(lib, "OBJParser.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "UtilzD.lib")
#else
#pragma comment(lib, "Utilz.lib")
#endif

namespace SE {
	namespace Core {
		GUIManager::GUIManager(ResourceHandler::IResourceHandler * resourceHandler, Graphics::IRenderer* renderer, const EntityManager & entityManager)
			:resourceHandler(resourceHandler), renderer(renderer), entityManager(entityManager)
		{
			_ASSERT(resourceHandler);
			_ASSERT(renderer);

			StopProfile;
		}

		GUIManager::~GUIManager()
		{

		}

		void GUIManager::CreateRenderableText(const Entity& entity, Graphics::TextGUI inTextInfo)
		{
			StartProfile;

			// Check if the entity is alive
			if (!entityManager.Alive(entity))
				ProfileReturnVoid;

			Graphics::TextGUI tempText;
			tempText = inTextInfo;

			entID[entity] = loadedTexts.size();
			loadedTexts.push_back(tempText);
			ProfileReturnVoid;	
		}

		void GUIManager::ToggleRenderableText(const Entity & entity, bool show)
		{
			StartProfile;
			// chexk if entity exist in text 
			auto fileLoaded = entID.find(entity);
			if (fileLoaded != entID.end())
			{
				if (show)
				{
					renderer->EnableTextRendering(loadedTexts[entID[entity]]);
				}
				else
				{
					renderer->DisableTextRendering(loadedTexts[entID[entity]]);
				}
				ProfileReturnVoid;
			}
			StopProfile;
		}

		void GUIManager::Frame()
		{

		}

		void GUIManager::Shutdown()
		{
			entID.clear();
			loadedTexts.clear();
		}


	}
}