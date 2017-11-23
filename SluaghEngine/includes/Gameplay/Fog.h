#ifndef _SE_GAMEPLAY_FOG_H_
#define _SE_GAMEPLAY_FOG_H_


#undef min
#undef max


#include <memory.h>
#include <vector>
#include <algorithm>


namespace SE
{
	namespace Gameplay
	{
		class Fog
		{
		private:
			struct Vertex
			{
				float position[3];
				float uv[2];
				float opacity;

				Vertex() {};
				Vertex(float x, float y, float z, float u, float v, float o = 1)
				{
					position[0] = x; position[1] = y; position[2] = z;
					uv[0] = u; uv[1] = v;
					opacity = o;
				}
			};

			struct Quad
			{
				enum class Vertices { TopLeft, TopRight, BottomLeft, BottomRight };

			private:
				Vertex vertices[6];

			public:
				Quad() {};
				void SetVertex(Vertices vertex, float x, float y, float z, float opacity = 1)
				{
					float u = x;
					float v = z;

					SetVertex(vertex, x, y, z, u, v, opacity);
				}
				void SetVertex(Vertices vertex, float x, float y, float z, float u, float v, float opacity = 1)
				{
					if (vertex == Vertices::TopLeft)
					{
						vertices[0] = Vertex(x, y, z, u, v, opacity);
						vertices[3] = Vertex(x, y, z, u, v, opacity);
					}
					else if (vertex == Vertices::TopRight)
					{
						vertices[1] = Vertex(x, y, z, u, v, opacity);
					}
					else if (vertex == Vertices::BottomLeft)
					{
						vertices[5] = Vertex(x, y, z, u, v, opacity);
					}
					else if (vertex == Vertices::BottomRight)
					{
						vertices[2] = Vertex(x, y, z, u, v, opacity);
						vertices[4] = Vertex(x, y, z, u, v, opacity);
					}
				}
			};

			struct Plane
			{
			private:
				Quad quads[27 * 27];
				bool quads_used[27 * 27] = {};

				std::vector<Quad> extraQuads;

			public:
				Plane() {};
				Quad& GetQuad(unsigned int column, unsigned int row)
				{
					quads_used[row * 27 + column] = true;
					return quads[row * 27 + column];
				}
				Quad& GetQuad(int extraQuadIOffset = 0)
				{
					int extraQuadI = extraQuads.size() - 1 + std::min(0, extraQuadIOffset);

					return extraQuads[extraQuadI];
				}
				void AddQuad()
				{
					extraQuads.push_back(Quad());
				}
				bool GetQuadStatus(unsigned int column, unsigned int row)
				{
					return quads_used[row * 27 + column];
				}
				unsigned int GetQuadCount()
				{
					unsigned int quadCount = 0;
					for (int quadI = 0; quadI < 27 * 27; quadI++)
					{
						if (quads_used[quadI])
							quadCount++;
					}

					quadCount += extraQuads.size();

					return quadCount;
				}
				void GetVertexBuffer(Vertex *pVertexBuffer)
				{
					unsigned int vertexI = 0;
					for (int quadI = 0; quadI < 27 * 27; quadI++)
					{
						if (quads_used[quadI])
						{
							memcpy(pVertexBuffer + vertexI, &quads[quadI], 2 * 3 * sizeof(Vertex));

							vertexI += 2 * 3;
						}
					}

					for (unsigned int quadI = 0; quadI < extraQuads.size(); quadI++)
					{
						memcpy(pVertexBuffer + vertexI, &extraQuads[quadI], 2 * 3 * sizeof(Vertex));

						vertexI += 2 * 3;
					}
				}
			};

		private:
			static const char id_Props = 137;
			static const char id_Torch = 203;
			static const char id_Floor = 0;
			static const char id_DeadArea = 76;
			static const char id_Door1 = 22;
			static const char id_Door2 = 48;
			static const char id_Wall = 255;
			static const char id_Pillar = 225;
			static const char id_Bush = 13;

			const float padding = 10.f;
			const float height = 3.1f;
			const float slopeTopOffset[2] = { 0.3f, height - 0.3f };
			const float slopeMiddleOffset[2] = { 0.3f, height / 2.f };
			const float slopeBottomOffset[2] = { 0.6f, 0.6f };

			char tileValues[25][25];
			unsigned int topPlaneRjHandle = -1;
			unsigned int bottomPlaneRjHandle = -1;

			Plane topPlane;
			Plane bottomPlane;

		public:
			void Set(bool status);
			void SetTileValues(char tileValues[25][25]);

		private:
			void CreatePlane();
			void AddAdjacentTiles(unsigned int column, unsigned int row);
			void AddSlope(unsigned int column, unsigned int row);
			char GetTileValue(unsigned int column, unsigned int row);
		};
	}
}


#endif
