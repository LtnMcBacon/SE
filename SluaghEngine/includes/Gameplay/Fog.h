#ifndef _SE_GAMEPLAY_FOG_H_
#define _SE_GAMEPLAY_FOG_H_


#include <Graphics\IRenderer.h>


namespace SE
{
	namespace Gameplay
	{
		class Fog
		{
		private:
			enum class WallSide { Top, Bottom, Left, Right };

		private:
			static unsigned int fogIndex;

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

			Graphics::RenderGroup topPlane_renderGroup = Graphics::RenderGroup::RENDER_PASS_5;
			Graphics::RenderGroup bottomPlane_renderGroup = Graphics::RenderGroup::RENDER_PASS_4;


			char tileValues[25][25];

			bool rjInitialized = false;
			bool rjEnabled = false;

			unsigned int topPlaneRjHandle = -1;
			unsigned int bottomPlaneRjHandle = -1;

			Graphics::RenderJob topPlaneRj;
			Graphics::RenderJob bottomPlaneRj;

			Plane *topPlane;
			Plane *bottomPlane;

			unsigned int firstWallTile[2];
			WallSide firstWallSide;

			unsigned int previousWallTile[2];
			unsigned int currentWallTile[2];
			unsigned int nextWallTile[2];

			WallSide previousWallSide;
			WallSide currentWallSide;
			WallSide nextWallSide;

		public:
			Fog();
			~Fog();

			void Set(bool status);
			void SetTileValues(char tileValues[25][25]);

		private:
			void CreatePlane();
			void AddAdjacentTiles(unsigned int column, unsigned int row);
			unsigned int GetSlopeCount();
			void SetFirstWall();
			void ContinueToNextWall();
			void AddSlope(unsigned int slopeIndex);
			char GetTileValue(unsigned int column, unsigned int row);
		};
	}
}


#endif
