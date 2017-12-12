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

			class Vertex;
			class Quad;
			class Plane;

			struct EdgeAttributes;

			class SlopeRandomization;

		private:
			static unsigned int fogIndex;

			static const char id_Props = 137;
			static const char id_Torch = 203;
			static const char id_Floor = 0;
			static const char id_DeadArea = 76;
			static const char id_Door1 = 22;
			static const char id_Door2 = 48;
			static const char id_Wall = 255;
			static const char id_Pillar = 225;
			static const char id_Bush = 13;

			const float roomCenter[2] = { 12.5, 12.5 };

			const float padding = 10.f;
			const float height = 3.1f;

			const float slopeRimOffset[2] = { 0.4f, height - 0.4f };
			const float slopeTopOffset[2] = { 0.15f, height + 0.17f };
			const float slopeMiddleOffset[2] = { 0.3f, height / 2.f };
			const float slopeBottomOffset[2] = { 0.75f, 0.f };

			const float rimOpacity = 0;
			const float slopeTopOpacity = 0.85;
			const float slopeMiddleOpacity = 0.5;
			const float slopeBottomOpacity = 0;

			const float slopeTopOpacity_door = 1;
			const float slopeMiddleOpacity_door = 0.1;
			const float slopeBottomOpacity_door = 0;

			const float randomizationMultiplier = 1;

			static const unsigned int minimumCenterSpaceRadius = 5;
			static const unsigned int maximumCenterOffsetRadius = 10;
			static const unsigned int spaceCheckRotationIterationCount = 16;

			static const Graphics::RenderGroup topPlane_renderGroup = Graphics::RenderGroup::POST_PASS_4;
			static const Graphics::RenderGroup bottomPlane_renderGroup = Graphics::RenderGroup::POST_PASS_3;

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

			void Enable(bool status);
			void Instantiate(char tileValues[25][25], float *time);

		private:
			void CreatePlane(float *time);
			void AddAdjacentTiles(unsigned int column, unsigned int row);
			unsigned int GetSlopeCount();
			void SetFirstWall();
			void ContinueToNextWall();
			void AddSlope(unsigned int slopeIndex, EdgeAttributes& attributes, SlopeRandomization& randomization, bool onlyInstantiate = false);
			void AdjustRimVertex(float position[3]);
			float GetFreeSpaceAt(float column, float row);
			void AdjustOptimalCenter(float (&centerPosition)[2]);
			bool IsOpenDoorSide(const unsigned int (&tileOutsideDoor)[2], WallSide doorSide);
			char GetTileValue(unsigned int column, unsigned int row);
		};
	}
}


#endif
