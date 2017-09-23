#ifndef SE_GRAPHICS_SHADERSETTINGS_H_
#define SE_GRAPHICS_SHADERSETTINGS_H_

#include <map>
#include <cstdint>
#include <Utilz/GUID.h>

namespace SE
{
	namespace Graphics
	{
		struct ShaderSettings
		{			
			struct CBuffer
			{
				struct CBufferVar
				{
					uint32_t offset;
					uint32_t size;
				};
				uint32_t size;
				uint32_t bindSlot;
				std::map<Utilz::GUID, CBufferVar, Utilz::GUID::Compare> variables;
			};
			std::map<Utilz::GUID, CBuffer, Utilz::GUID::Compare> bufferNameToBufferInfo;
			std::map<Utilz::GUID, uint32_t, Utilz::GUID::Compare> textureNameToBindSlot;
		};
	}
}



#endif
