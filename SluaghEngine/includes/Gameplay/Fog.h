#ifndef _SE_GAMEPLAY_FOG_H_
#define _SE_GAMEPLAY_FOG_H_


namespace SE
{
	namespace Gameplay
	{
		class Fog
		{
		private:
			class Vertex;
			class Quad;
			class Plane;

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

			Plane *topPlane;
			Plane *bottomPlane;

		public:
			Fog();
			~Fog();

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
