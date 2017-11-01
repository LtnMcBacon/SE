//#ifndef SE_CORE_MATERIAL_LOADING_H_
//#define SE_CORE_MATERIAL_LOADING_H_
//#include <Utilz\GUID.h>
//#include <Graphics\Pipeline.h>
//#include <Graphics\FileHeaders.h>
//#include <list>
//#include <Entity.h>
//#include <map>
//#include <Graphics\IRenderer.h>
//#include <ResourceHandler\IResourceHandler.h>
//#include <Utilz\CircularFiFo.h>
//#include <mutex>
//namespace SE
//{
//	namespace Core
//	{
//		struct TextureInfo
//		{
//			uint32_t numTextures;
//			Utilz::GUID bindings[Graphics::ShaderStage::maxTextures];
//			Utilz::GUID textures[Graphics::ShaderStage::maxTextures];
//		};
//
//		struct MaterialFileData
//		{
//			Graphics::MaterialAttributes attrib;
//			TextureInfo textureInfo;
//		};
//		struct TextureData
//		{
//			size_t refCount = 0;
//		};
//		struct ShaderData
//		{
//			size_t refCount = 0;
//		};
//
//		class MaterialLoading
//		{
//		public:
//			MaterialLoading(Graphics::IRenderer* renderer,
//			ResourceHandler::IResourceHandler* resourceHandler);
//			~MaterialLoading();
//
//
//
//
//
//			int LoadShader(const Utilz::GUID& guid);
//			bool IsShaderLoaded(const Utilz::GUID& guid)const;
//
//			int LoadMaterialFile(const Utilz::GUID& guid);
//			MaterialFileData& GetMaterialFile(const Utilz::GUID& guid);
//			bool IsMaterialFileLoaded(const Utilz::GUID& guid)const;
//			void UnloadMaterialFile(const Utilz::GUID& guid, const Entity& entity);
//
//
//			int LoadShaderAndMaterialFileAndTextures(const Utilz::GUID& shader, const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior);
//			int LoadShaderAndTextures(const Utilz::GUID& shader, const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior);
//			int LoadMaterialFileAndTextures(const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior);
//
//
//			int LoadTextures(const Utilz::GUID& materialFile, bool async, ResourceHandler::Behavior behavior, const std::function<void(const Utilz::GUID&, int)>& errorCallback);
//			int LoadTexture(const Utilz::GUID& guid);
//			bool IsTextureLoaded(const Utilz::GUID& guid)const;
//
//		private:
//			int LoadTexture(const Utilz::GUID& guid, void*data, size_t size)const;
//
//
//			Graphics::IRenderer* renderer;
//			ResourceHandler::IResourceHandler* resourceHandler;
//
//			std::map<Utilz::GUID, ShaderData, Utilz::GUID::Compare> guidToShader;
//			std::mutex shaderLock;
//
//			std::map<Utilz::GUID, TextureData, Utilz::GUID::Compare> guidToTexture;
//			std::mutex textureLock;
//
//			std::map<Utilz::GUID, MaterialFileData, Utilz::GUID::Compare> guidToMaterial;
//			std::mutex materialLock;
//
//			struct MaterialUpdateStruct
//			{
//				Utilz::GUID material;
//			};
//			Utilz::CircularFiFo<MaterialUpdateStruct, 10> toUpdateFull;
//		};
//	}
//}
//
//
//#endif //SE_CORE_MATERIAL_LOADING_H_