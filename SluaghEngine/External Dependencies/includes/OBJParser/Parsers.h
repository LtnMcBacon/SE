#pragma once
#include "ArfData.h"

namespace Arf
{
	namespace Mesh
	{
		namespace InterleaveOption
		{
			enum : uint8_t
			{
				Position = 1 << 0,
				TexCoord = 1 << 1,
				Normal = 1 << 2
			};
		}
		
		struct Position
		{
			float x, y, z;
		};

		struct TexCoord
		{
			float u, v;
		};

		struct Normal
		{
			float x, y, z;
		};

		struct Vertex
		{
			Position pos;
			Normal norm;
			TexCoord tex;
		};
		struct VertexPositionTexcoord
		{
			Position pos;
			TexCoord tex;
		};
		struct VertexPositionNormal
		{
			Position pos;
			Normal norm;
		};
		struct Data
		{
			uint32_t NumVertices;
			void* vertices;
		};
	}


#define PARSER_FILE_NOT_FOUND -1
#define PARSER_SUCCESS 0
#define INTERLEAVE 1 << 0
#define FLIPZ 1 << 1
#define FLIPT 1 << 2

	int ParseObj(const char* filename, ArfData::Data* data, ArfData::DataPointers* dataPointers, uint32_t flags = 0);
	int ParseObj(const void* rawData, size_t size, ArfData::Data* data, ArfData::DataPointers* dataPointers, uint32_t flags = 0);

	int ParseArf(const char* filename, ArfData::Data* data, ArfData::DataPointers* dataPointers, uint32_t flags = 0);

	int Interleave(ArfData::Data& data, ArfData::DataPointers& dataPointers, Mesh::Data** dataOut, size_t* iSize, uint8_t interleaveOptions = ~0u, uint32_t flags = 0);

}