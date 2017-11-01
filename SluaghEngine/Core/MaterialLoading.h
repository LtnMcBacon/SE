#ifndef SE_CORE_MATERIAL_LOADING_H_
#define SE_CORE_MATERIAL_LOADING_H_
#include <Utilz\GUID.h>
#include <Graphics\Pipeline.h>
#include <Graphics\FileHeaders.h>
#include <list>
#include <Entity.h>
#include <map>
#include <Graphics\IRenderer.h>
#include <ResourceHandler\IResourceHandler.h>
#include <Utilz\CircularFiFo.h>
#include <DevConsole\IConsole.h>
#include <Entity.h>

#include <mutex>
namespace SE
{
	namespace Core
	{
		struct TextureInfo
		{
			uint8_t numTextures;
			Utilz::GUID bindings[Graphics::ShaderStage::maxTextures];
			Utilz::GUID textures[Graphics::ShaderStage::maxTextures];
		};

		struct MaterialFileData
		{
			Graphics::MaterialAttributes attrib;
			TextureInfo textureInfo;
			std::list<Entity> entities;
		};
		struct TextureData
		{
			size_t refCount = 0;
		};
		struct ShaderData
		{
			size_t refCount = 0;
		};

		class MaterialLoading
		{
		public:
			MaterialLoading(Graphics::IRenderer* renderer,
			ResourceHandler::IResourceHandler* resourceHandler,
				DevConsole::IConsole* console);
			~MaterialLoading();

			struct LoadInfo
			{
				Entity& entity; Utilz::GUID shader; Utilz::GUID material;
			};
			void LoadStuff(const LoadInfo& info, bool async,ResourceHandler::Behavior b);
			bool DoUpdate(std::vector<Entity>& entitiesToUpdate);

			int LoadShader(const Utilz::GUID& guid);
			bool IsShaderLoaded(const Utilz::GUID& guid)const;

			int LoadMaterialFile(const Utilz::GUID& material);
			MaterialFileData& GetMaterialFile(const Utilz::GUID& guid);
			bool IsMaterialFileLoaded(const Utilz::GUID& guid)const;
			void UnloadMaterialFile(const Utilz::GUID& guid, const Entity& entity);


			int LoadShaderAndMaterialFileAndTextures(const Utilz::GUID& shader, const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior);
			int LoadShaderAndTextures(const Utilz::GUID& shader, const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior);
			int LoadMaterialFileAndTextures(const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior);


			int LoadTextures(const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior, const std::function<void(const Utilz::GUID&, int)>& errorCallback);
			int LoadTexture(const Utilz::GUID& guid);
			void LoadTextures(const Entity& entity, const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior);
			bool IsTextureLoaded(const Utilz::GUID& guid)const;


		private:
			int LoadTexture(const Utilz::GUID& guid, void*data, size_t size)const;


			Graphics::IRenderer* renderer;
			ResourceHandler::IResourceHandler* resourceHandler;
			DevConsole::IConsole* console;

			std::map<Utilz::GUID, ShaderData, Utilz::GUID::Compare> guidToShader;
			std::mutex shaderLock;

			std::map<Utilz::GUID, TextureData, Utilz::GUID::Compare> guidToTexture;
			std::recursive_mutex textureLock;

			std::map<Utilz::GUID, MaterialFileData, Utilz::GUID::Compare> guidToMaterial;
			std::mutex materialLock;

			struct MaterialUpdateStruct
			{
				Utilz::GUID material;
				size_t index;
				Utilz::GUID texture;
				Entity entity;
			};
			Utilz::CircularFiFo<MaterialUpdateStruct, 100> toUpdate;
		};
	}
}


#endif //SE_CORE_MATERIAL_LOADING_H_