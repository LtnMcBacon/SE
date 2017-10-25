#ifndef SE_CORE_MATERIAL_LOADING_H_
#define SE_CORE_MATERIAL_LOADING_H_
#include <Utilz\GUID.h>
#include <Graphics\Pipeline.h>
#include <Graphics\FileHeaders.h>
#include <list>
#include <Entity.h>

namespace SE
{
	namespace Core
	{
		struct TextureInfo
		{
			uint32_t amountOfTex;
			Utilz::GUID bindings[Graphics::ShaderStage::maxTextures];
			Utilz::GUID textures[Graphics::ShaderStage::maxTextures];
		};

		struct MaterialInfo
		{
			Graphics::MaterialAttributes attrib;
			TextureInfo textureInfo;
			std::list<Entity> entities;
		};

		class MaterialLoading
		{
		public:
			static int LoadMaterialFile(void * data, size_t size, MaterialInfo& dataIinfo);


		private:
			MaterialLoading();
			~MaterialLoading();
		};
	}
}


#endif //SE_CORE_MATERIAL_LOADING_H_