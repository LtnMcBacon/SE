#include "Fog.h"

#include "CoreInit.h"
#include <memory.h>
#include <vector>
#include <algorithm>
#include <cmath>


#undef min
#undef max


using namespace SE;
using namespace Gameplay;


unsigned int SE::Gameplay::Fog::fogIndex = 0;


class SE::Gameplay::Fog::Vertex
{
public:
	float position[3] = {};
	float uv[2] = {};
	float opacity = 0;

public:
	Vertex() {};
	Vertex(float x, float y, float z, float u, float v, float o = 1)
	{
		position[0] = x; position[1] = y; position[2] = z;
		uv[0] = u; uv[1] = v;
		opacity = o;
	}
};

class SE::Gameplay::Fog::Quad
{
public:
	enum class Vertices { UNDEFINED, TopLeft, TopRight, BottomLeft, BottomRight };
	enum class SeamOrientation { UNDEFINED, TopToBottom, BottomToTop };

private:
	Vertex vertices[6];

	SeamOrientation seamOrientation = SeamOrientation::UNDEFINED;

	unsigned int topLeftVertices[2] = { UNDEFINED, UNDEFINED };
	unsigned int topRightVertices[2] = { UNDEFINED, UNDEFINED };
	unsigned int bottomLeftVertices[2] = { UNDEFINED, UNDEFINED };
	unsigned int bottomRightVertices[2] = { UNDEFINED, UNDEFINED };

	// Flags
	static const unsigned int UNDEFINED = -1;

public:
	Quad(SeamOrientation seamOrientation = SeamOrientation::TopToBottom)
	{
		FlipSeam(seamOrientation);
	}
	void SetVertex(Vertices vertex, float x, float y, float z, float opacity = 1)
	{
		float u = x;
		float v = z;

		SetVertex(vertex, x, y, z, u, v, opacity);
	}
	void SetVertex(Vertices vertex, float x, float y, float z, float u, float v, float opacity = 1)
	{
		unsigned int vertexI = 0;

		for (unsigned int nextVertexIndex = VertexIterator(vertex, vertexI); nextVertexIndex != UNDEFINED; nextVertexIndex = VertexIterator(vertex, vertexI))
		{
			vertices[nextVertexIndex] = Vertex(x, y, z, u, v, opacity);

			++vertexI;
		}
	}
	void SetPosition(Vertices vertex, float x, float y, float z, bool adjustUv = true)
	{
		unsigned int vertexI = 0;

		for (unsigned int nextVertexIndex = VertexIterator(vertex, vertexI); nextVertexIndex != UNDEFINED; nextVertexIndex = VertexIterator(vertex, vertexI))
		{
			vertices[nextVertexIndex].position[0] = x;
			vertices[nextVertexIndex].position[1] = y;
			vertices[nextVertexIndex].position[2] = z;


			if (adjustUv)
			{
				vertices[nextVertexIndex].uv[0] = x;
				vertices[nextVertexIndex].uv[1] = z;
			}


			++vertexI;
		}
	}
	void AdjustUv(Quad &parentQuad, Vertices parent, Vertices child)
	{
		unsigned int vertexIndex_parent = parentQuad.VertexIterator(parent, 0);
		unsigned int vertexIndex_child = VertexIterator(child, 0);


		float parentU, parentV;

		parentU = parentQuad.vertices[vertexIndex_parent].uv[0];
		parentV = parentQuad.vertices[vertexIndex_parent].uv[1];


		float uVector[2];

		uVector[0] = vertices[vertexIndex_child].position[0];
		uVector[1] = vertices[vertexIndex_child].position[1];

		uVector[0] -= parentQuad.vertices[vertexIndex_parent].position[0];
		uVector[1] -= parentQuad.vertices[vertexIndex_parent].position[1];


		float vVector[2];

		vVector[0] = vertices[vertexIndex_child].position[2];
		vVector[1] = vertices[vertexIndex_child].position[1];

		vVector[0] -= parentQuad.vertices[vertexIndex_parent].position[2];
		vVector[1] -= parentQuad.vertices[vertexIndex_parent].position[1];


		float uDistance = sqrt(pow(uVector[0], 2) + pow(uVector[1], 2));
		float vDistance = sqrt(pow(vVector[0], 2) + pow(vVector[1], 2));


		unsigned int vertexI = 0;

		for (unsigned int nextVertexIndex = VertexIterator(child, vertexI); nextVertexIndex != UNDEFINED; nextVertexIndex = VertexIterator(child, vertexI))
		{
			vertices[nextVertexIndex].uv[0] = parentU + ((uVector[0] > 0) - (uVector[0] < 0)) * uDistance;
			vertices[nextVertexIndex].uv[1] = parentV + ((vVector[0] > 0) - (vVector[0] < 0)) * vDistance;

			++vertexI;
		}
	}
	void AdjustUv(Vertices parent, Vertices child)
	{
		AdjustUv(*this, parent, child);
	}
	void AdjustV(Quad &parentQuad, Vertices parent, Vertices child)
	{
		unsigned int vertexIndex_parent = parentQuad.VertexIterator(parent, 0);
		unsigned int vertexIndex_child = VertexIterator(child, 0);


		float parentV;

		parentV = parentQuad.vertices[vertexIndex_parent].uv[1];


		float vVector[3];

		vVector[0] = vertices[vertexIndex_child].position[0];
		vVector[1] = vertices[vertexIndex_child].position[1];
		vVector[2] = vertices[vertexIndex_child].position[2];

		vVector[0] -= parentQuad.vertices[vertexIndex_parent].position[0];
		vVector[1] -= parentQuad.vertices[vertexIndex_parent].position[1];
		vVector[2] -= parentQuad.vertices[vertexIndex_parent].position[2];


		float vDistance = sqrt(pow(vVector[0], 2) + pow(vVector[1], 2) + pow(vVector[2], 2));


		unsigned int vertexI = 0;

		for (unsigned int nextVertexIndex = VertexIterator(child, vertexI); nextVertexIndex != UNDEFINED; nextVertexIndex = VertexIterator(child, vertexI))
		{
			vertices[nextVertexIndex].uv[1] = parentV + ((vVector[1] > 0) - (vVector[1] < 0)) * vDistance;

			++vertexI;
		}
	}
	void AdjustV(Vertices parent, Vertices child)
	{
		AdjustV(*this, parent, child);
	}
	void OffsetUv(float offset_u, float offset_v, Vertices vertex = Vertices::UNDEFINED)
	{
		if (vertex == Vertices::UNDEFINED)
		{
			for (unsigned int vertexI = 0; vertexI < 6; ++vertexI)
			{
				vertices[vertexI].uv[0] += offset_u;
				vertices[vertexI].uv[1] += offset_v;
			}
		}
		else
		{
			unsigned int vertexI = 0;

			for (unsigned int nextVertexIndex = VertexIterator(vertex, vertexI); nextVertexIndex != UNDEFINED; nextVertexIndex = VertexIterator(vertex, vertexI))
			{
				vertices[nextVertexIndex].uv[0] += offset_u;
				vertices[nextVertexIndex].uv[1] += offset_v;

				++vertexI;
			}
		}
	}
	void FlipSeam(SeamOrientation newOrientation = SeamOrientation::UNDEFINED)
	{
		if (newOrientation == SeamOrientation::UNDEFINED)
		{
			if (seamOrientation == SeamOrientation::TopToBottom)
				newOrientation = SeamOrientation::BottomToTop;
			else if (seamOrientation == SeamOrientation::BottomToTop)
				newOrientation = SeamOrientation::TopToBottom;
		}

		if (newOrientation == SeamOrientation::UNDEFINED || newOrientation == seamOrientation)
			return;


		unsigned int oldTopLeftIndex = topLeftVertices[0];
		unsigned int oldTopRightIndex = topRightVertices[0];
		unsigned int oldBottomLeftIndex = bottomLeftVertices[0];
		unsigned int oldBottomRightIndex = bottomRightVertices[0];


		if (newOrientation == SeamOrientation::TopToBottom)
		{
			topLeftVertices[0] = 0;
			topLeftVertices[1] = 3;

			topRightVertices[0] = 1;
			topRightVertices[1] = UNDEFINED;

			bottomLeftVertices[0] = 5;
			bottomLeftVertices[1] = UNDEFINED;

			bottomRightVertices[0] = 2;
			bottomRightVertices[1] = 4;
		}
		else if (newOrientation == SeamOrientation::BottomToTop)
		{
			topLeftVertices[0] = 1;
			topLeftVertices[1] = UNDEFINED;

			topRightVertices[0] = 2;
			topRightVertices[1] = 4;

			bottomLeftVertices[0] = 0;
			bottomLeftVertices[1] = 3;

			bottomRightVertices[0] = 5;
			bottomRightVertices[1] = UNDEFINED;
		}


		if (seamOrientation != SeamOrientation::UNDEFINED)
		{
			Vertex oldTopLeftVertex = vertices[oldTopLeftIndex];
			Vertex oldTopRightVertex = vertices[oldTopRightIndex];
			Vertex oldBottomLeftVertex = vertices[oldBottomLeftIndex];
			Vertex oldBottomRightVertex = vertices[oldBottomRightIndex];


			for (unsigned int quadCornerI = 0; quadCornerI < 4; quadCornerI++)
			{
				Vertices quadCorner;
				Vertex *quadCornerVertex;

				if (quadCornerI == 0)
				{
					quadCorner = Vertices::TopLeft;
					quadCornerVertex = &oldTopLeftVertex;
				}
				else if (quadCornerI == 1)
				{
					quadCorner = Vertices::TopRight;
					quadCornerVertex = &oldTopRightVertex;
				}
				else if (quadCornerI == 2)
				{
					quadCorner = Vertices::BottomLeft;
					quadCornerVertex = &oldBottomLeftVertex;
				}
				else // quadCornerI == 3
				{
					quadCorner = Vertices::BottomRight;
					quadCornerVertex = &oldBottomRightVertex;
				}


				unsigned int vertexI = 0;

				for (unsigned int nextVertexIndex = VertexIterator(quadCorner, vertexI); nextVertexIndex != UNDEFINED; nextVertexIndex = VertexIterator(quadCorner, vertexI))
				{
					vertices[nextVertexIndex] = *quadCornerVertex;

					++vertexI;
				}
			}
		}


		seamOrientation = newOrientation;
	}
	void* GetData()
	{
		return (void*)vertices;
	}

private:
	unsigned int VertexIterator(Vertices vertex, unsigned int index)
	{
		unsigned int vertexIndex = UNDEFINED;

		if (index >= 2)
			return vertexIndex;


		if (vertex == Vertices::TopLeft)
			vertexIndex = topLeftVertices[index];
		else if (vertex == Vertices::TopRight)
			vertexIndex = topRightVertices[index];
		else if (vertex == Vertices::BottomLeft)
			vertexIndex = bottomLeftVertices[index];
		else if (vertex == Vertices::BottomRight)
			vertexIndex = bottomRightVertices[index];


		return vertexIndex;
	}
};

class SE::Gameplay::Fog::Plane
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
		if (column < 27 && row < 27)
			return quads_used[row * 27 + column];
		else
			return true;
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
				memcpy(pVertexBuffer + vertexI, quads[quadI].GetData(), 2 * 3 * sizeof(Vertex));

				vertexI += 2 * 3;
			}
		}

		for (unsigned int quadI = 0; quadI < extraQuads.size(); quadI++)
		{
			memcpy(pVertexBuffer + vertexI, extraQuads[quadI].GetData(), 2 * 3 * sizeof(Vertex));

			vertexI += 2 * 3;
		}
	}
	void OffsetUvs(float uOffset, float vOffset)
	{
		for (unsigned int quadI = 0; quadI < 27 * 27; quadI++)
		{
			if (quads_used[quadI])
				quads[quadI].OffsetUv(uOffset, vOffset);
		}

		for (unsigned int extraQuadI = 0; extraQuadI < extraQuads.size(); extraQuadI++)
		{
			extraQuads[extraQuadI].OffsetUv(uOffset, vOffset);
		}
	}
};


struct SE::Gameplay::Fog::EdgeAttributes
{
	Vertex rim;
	Vertex top;
	Vertex middle;
	Vertex bottom;
};


class SE::Gameplay::Fog::SlopeRandomization
{
public:
	EdgeAttributes offsets;

public:
	void Randomize(float multiplier = 1)
	{
		offsets.rim.position[0] = offsets.rim.position[2] = multiplier * GetRandomNumberWithin(0, 0, 0);
		offsets.rim.position[1] = multiplier * GetRandomNumberWithin(-0.4, 0.15, 2.5);

		offsets.top.position[0] = offsets.top.position[2] = multiplier * GetRandomNumberWithin(0, 0, 0);
		offsets.top.position[1] = multiplier * GetRandomNumberWithin(-0.169, 0.1, 2.5);

		offsets.middle.position[0] = offsets.middle.position[2] = multiplier * GetRandomNumberWithin(-0.2, 0.2, 3);
		offsets.middle.position[1] = multiplier * GetRandomNumberWithin(-0.3, 0.3, 2.5);

		offsets.bottom.position[0] = offsets.bottom.position[2] = multiplier * GetRandomNumberWithin(-0.35, 0.35, 2.5);
		offsets.bottom.position[1] = multiplier * GetRandomNumberWithin(-0.1, 0.1, 1);


		offsets.rim.opacity = multiplier * GetRandomNumberWithin(-0.2, 0.1, 5);
		offsets.top.opacity = multiplier * GetRandomNumberWithin(-0.15, 0.3, 3);
		offsets.middle.opacity = multiplier * GetRandomNumberWithin(-0.2, 0.2, 4);
		offsets.bottom.opacity = multiplier * GetRandomNumberWithin(-0.25, 0.05, 4);
	}
	void Reset()
	{
		offsets = EdgeAttributes();
	}

private:
	float GetRandomNumberWithin(float min, float max, float origoAttraction)
	{
		float randomNumber = 0;

		if (min >= max)
			return randomNumber;


		randomNumber = rand() % RAND_MAX;

		randomNumber *= (max - min) / (RAND_MAX - 1);
		randomNumber += min;


		if (origoAttraction)
		{
			float normalizationValue = (randomNumber < 0) * min + (randomNumber > 0) * max;

			float attractionMultiplier = pow(fabs(randomNumber / normalizationValue), origoAttraction + 1);


			randomNumber *= attractionMultiplier;
		}


		return randomNumber;
	}
};


SE::Gameplay::Fog::Fog()
{
	topPlane = new Plane();
	bottomPlane = new Plane();


	srand(time(NULL));
}

SE::Gameplay::Fog::~Fog()
{
	delete topPlane;
	delete bottomPlane;
}

void SE::Gameplay::Fog::Enable(bool status)
{
	if (status && !rjEnabled && rjInitialized)
	{
		topPlaneRjHandle = CoreInit::subSystems.renderer->AddRenderJob(topPlaneRj, topPlane_renderGroup);
		bottomPlaneRjHandle = CoreInit::subSystems.renderer->AddRenderJob(bottomPlaneRj, bottomPlane_renderGroup);

		rjEnabled = true;
	}
	else if (!status && rjEnabled && topPlaneRjHandle != -1)
	{
		CoreInit::subSystems.renderer->RemoveRenderJob(topPlaneRjHandle);
		CoreInit::subSystems.renderer->RemoveRenderJob(bottomPlaneRjHandle);

		rjEnabled = false;
	}
}

void SE::Gameplay::Fog::Instantiate(char tileValues[25][25], float *time)
{
	for (unsigned int row = 0; row < 25; row++)
	{
		for (unsigned int column = 0; column < 25; column++)
		{
			this->tileValues[column][row] = tileValues[column][row];
		}
	}

	CreatePlane(time);
}

void SE::Gameplay::Fog::CreatePlane(float *time)
{
	/*
	std::string roomValues = "\n";

	for (int rowI = 24; rowI >= 0; rowI--)
	{
		for (int columnI = 0; columnI < 25; columnI++)
		{
			std::string tileValue = std::to_string((unsigned char)tileValues[columnI][rowI]);
			roomValues += tileValue;

			for (int spaceI = 0; spaceI < 3 - tileValue.size(); spaceI++)
				roomValues += ' ';
		}

		roomValues += '\n';
	}

	CoreInit::subSystems.devConsole->PrintChannel("Room Values", "%s", roomValues.c_str());
	*/


	{
		for (int paddingColumnI = 1; paddingColumnI < 26; paddingColumnI++)
		{
			topPlane->GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::TopLeft, paddingColumnI - 1, height, 0);
			topPlane->GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::TopRight, paddingColumnI, height, 0);
			topPlane->GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::BottomLeft, paddingColumnI - 1, height, -padding);
			topPlane->GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::BottomRight, paddingColumnI, height, -padding);

			AddAdjacentTiles(paddingColumnI - 1, 0);
		}

		for (int paddingColumnI = 1; paddingColumnI < 26; paddingColumnI++)
		{
			topPlane->GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::TopLeft, paddingColumnI - 1, height, 25 + padding);
			topPlane->GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::TopRight, paddingColumnI, height, 25 + padding);
			topPlane->GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::BottomLeft, paddingColumnI - 1, height, 25);
			topPlane->GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::BottomRight, paddingColumnI, height, 25);

			AddAdjacentTiles(paddingColumnI - 1, 24);
		}


		for (int paddingRowI = 1; paddingRowI < 26; paddingRowI++)
		{
			topPlane->GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::TopLeft, -padding, height, paddingRowI);
			topPlane->GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::TopRight, 0, height, paddingRowI);
			topPlane->GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::BottomLeft, -padding, height, paddingRowI - 1);
			topPlane->GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::BottomRight, 0, height, paddingRowI - 1);

			AddAdjacentTiles(0, paddingRowI - 1);
		}

		for (int paddingRowI = 1; paddingRowI < 26; paddingRowI++)
		{
			topPlane->GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::TopLeft, 25, height, paddingRowI);
			topPlane->GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::TopRight, 25 + padding, height, paddingRowI);
			topPlane->GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::BottomLeft, 25, height, paddingRowI - 1);
			topPlane->GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::BottomRight, 25 + padding, height, paddingRowI - 1);

			AddAdjacentTiles(24, paddingRowI - 1);
		}


		topPlane->GetQuad(0, 0).SetVertex(Quad::Vertices::TopLeft, -padding, height, 0);
		topPlane->GetQuad(0, 0).SetVertex(Quad::Vertices::TopRight, 0, height, 0);
		topPlane->GetQuad(0, 0).SetVertex(Quad::Vertices::BottomLeft, -padding, height, -padding);
		topPlane->GetQuad(0, 0).SetVertex(Quad::Vertices::BottomRight, 0, height, -padding);

		topPlane->GetQuad(26, 0).SetVertex(Quad::Vertices::TopLeft, 25, height, 0);
		topPlane->GetQuad(26, 0).SetVertex(Quad::Vertices::TopRight, 25 + padding, height, 0);
		topPlane->GetQuad(26, 0).SetVertex(Quad::Vertices::BottomLeft, 25, height, -padding);
		topPlane->GetQuad(26, 0).SetVertex(Quad::Vertices::BottomRight, 25 + padding, height, -padding);

		topPlane->GetQuad(0, 26).SetVertex(Quad::Vertices::TopLeft, -padding, height, 25 + padding);
		topPlane->GetQuad(0, 26).SetVertex(Quad::Vertices::TopRight, 0, height, 25 + padding);
		topPlane->GetQuad(0, 26).SetVertex(Quad::Vertices::BottomLeft, -padding, height, 25);
		topPlane->GetQuad(0, 26).SetVertex(Quad::Vertices::BottomRight, 0, height, 25);

		topPlane->GetQuad(26, 26).SetVertex(Quad::Vertices::TopLeft, 25, height, 25 + padding);
		topPlane->GetQuad(26, 26).SetVertex(Quad::Vertices::TopRight, 25 + padding, height, 25 + padding);
		topPlane->GetQuad(26, 26).SetVertex(Quad::Vertices::BottomLeft, 25, height, 25);
		topPlane->GetQuad(26, 26).SetVertex(Quad::Vertices::BottomRight, 25 + padding, height, 25);
	}


	{
		EdgeAttributes& attributes = EdgeAttributes();
		SlopeRandomization& randomization = SlopeRandomization();

		unsigned int slopeCount = GetSlopeCount();


		SetFirstWall();


		{
			unsigned int nextWallTile_old[2] = { nextWallTile[0], nextWallTile[1] };
			unsigned int currentWallTile_old[2] = { currentWallTile[0], currentWallTile[1] };

			WallSide nextWallSide_old = nextWallSide;
			WallSide currentWallSide_old = currentWallSide;


			nextWallTile[0] = currentWallTile[0];
			nextWallTile[1] = currentWallTile[1];

			currentWallTile[0] = previousWallTile[0];
			currentWallTile[1] = previousWallTile[1];

			nextWallSide = currentWallSide;
			currentWallSide = previousWallSide;


			AddSlope(0, attributes, randomization, true);


			nextWallTile[0] = nextWallTile_old[0];
			nextWallTile[1] = nextWallTile_old[1];

			currentWallTile[0] = currentWallTile_old[0];
			currentWallTile[1] = currentWallTile_old[1];

			nextWallSide = nextWallSide_old;
			currentWallSide = currentWallSide_old;
		}


		unsigned int slopeIndex = 1;
		do
		{
			if (slopeIndex && slopeIndex + 1 != slopeCount)
				randomization.Randomize(randomizationMultiplier);
			else if (slopeIndex == slopeCount)
				randomization.Reset();


			AddSlope(slopeIndex, attributes, randomization);

			ContinueToNextWall();


			slopeIndex++;

		} while (slopeIndex <= slopeCount);
	}


	{
		float positionSum[2] = {};
		unsigned int positionSum_count = 0;

		for (unsigned int rowI = 0; rowI < 25; rowI++)
		{
			for (unsigned int columnI = 0; columnI < 25; columnI++)
			{
				if (!topPlane->GetQuadStatus(columnI + 1, rowI + 1))
				{
					positionSum[0] += columnI + 0.5;
					positionSum[1] += rowI + 0.5;

					positionSum_count++;
				}
			}
		}

		float positionAverage[2] = { positionSum[0] / positionSum_count, positionSum[1] / positionSum_count };


		float optimalCenter[2] = { positionAverage[0], positionAverage[1] };
		float freeSpaceRadius = GetFreeSpaceAt(positionAverage[0], positionAverage[1]);

		float offset = 1;
		unsigned int rotationIterationCount = 8;
		while (freeSpaceRadius < minimumCenterSpaceRadius && offset <= maximumCenterOffsetRadius)
		{
			float rotationSine = sin(6.283 / rotationIterationCount);
			float rotationCosine = cos(6.283 / rotationIterationCount);
			

			float rotationVector[2] = { 0, offset };

			for (unsigned int rotationI = 0; rotationI < rotationIterationCount; rotationI++)
			{
				float new_freeSpaceRadius = GetFreeSpaceAt(positionAverage[0] + rotationVector[0], positionAverage[1] + rotationVector[1]);

				if (new_freeSpaceRadius > freeSpaceRadius)
				{
					optimalCenter[0] = positionAverage[0] + rotationVector[0];
					optimalCenter[1] = positionAverage[1] + rotationVector[1];

					freeSpaceRadius = new_freeSpaceRadius;
				}


				float rotationVector_oldX = rotationVector[0];

				rotationVector[0] = rotationCosine * rotationVector[0] - rotationSine * rotationVector[1];
				rotationVector[1] = rotationSine * rotationVector_oldX + rotationCosine * rotationVector[1];
			}

			offset++;
			rotationIterationCount *= 2;
		}

		if (freeSpaceRadius < minimumCenterSpaceRadius)
			AdjustOptimalCenter(optimalCenter);


		topPlane->OffsetUvs(12.5 - optimalCenter[0], 12.5 - optimalCenter[1]);
	}


	{
		unsigned int topPlaneVertexCount = topPlane->GetQuadCount() * 2 * 3;

		Vertex *topPlaneVertexBuffer = new Vertex[topPlaneVertexCount];
		topPlane->GetVertexBuffer(topPlaneVertexBuffer);

		std::string topPlaneVbName = std::string("FogTopPlaneVb#" + std::to_string(fogIndex));

		CoreInit::subSystems.renderer->GetPipelineHandler()->CreateVertexBuffer(topPlaneVbName, topPlaneVertexBuffer, topPlaneVertexCount, sizeof(Vertex));


		topPlaneRj.pipeline.OMStage.renderTargets[0] = "backbuffer";
		topPlaneRj.pipeline.OMStage.renderTargetCount = 1;

		topPlaneRj.pipeline.VSStage.shader = "FogTopVS.hlsl";
		topPlaneRj.pipeline.GSStage.shader = "FogTopGS.hlsl";
		topPlaneRj.pipeline.PSStage.shader = "FogTopPS.hlsl";

		topPlaneRj.pipeline.PSStage.samplers[0] = "FogSs";
		topPlaneRj.pipeline.PSStage.samplerCount = 1;

		topPlaneRj.pipeline.PSStage.textures[0] = "Fog_AlbedoTexture.png";
		topPlaneRj.pipeline.PSStage.textures[1] = "Fog_NormalTexture.png";
		topPlaneRj.pipeline.PSStage.textureCount = 2;
		topPlaneRj.pipeline.PSStage.textureBindings[0] = "albedoTexture";
		topPlaneRj.pipeline.PSStage.textureBindings[1] = "normalTexture";

		topPlaneRj.pipeline.IAStage.inputLayout = "FogTopVS.hlsl";
		topPlaneRj.pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;
		topPlaneRj.pipeline.IAStage.vertexBuffer = topPlaneVbName;

		topPlaneRj.pipeline.OMStage.blendState = "FogBs";
		topPlaneRj.pipeline.OMStage.depthStencilState = "FogDss";
		topPlaneRj.pipeline.OMStage.depthStencilView = "backbuffer";

		topPlaneRj.vertexCount = topPlaneVertexCount;
		topPlaneRj.maxInstances = 1;



		unsigned int bottomPlaneVertexCount = bottomPlane->GetQuadCount() * 2 * 3;

		Vertex *bottomPlaneVertexBuffer = new Vertex[bottomPlaneVertexCount];
		bottomPlane->GetVertexBuffer(bottomPlaneVertexBuffer);

		std::string bottomPlaneVbName = std::string("FogBottomPlaneVb#" + std::to_string(fogIndex));

		CoreInit::subSystems.renderer->GetPipelineHandler()->CreateVertexBuffer(bottomPlaneVbName, bottomPlaneVertexBuffer, bottomPlaneVertexCount, sizeof(Vertex));

		fogIndex++;


		bottomPlaneRj.pipeline.OMStage.renderTargets[0] = "backbuffer";
		bottomPlaneRj.pipeline.OMStage.renderTargetCount = 1;

		bottomPlaneRj.pipeline.VSStage.shader = "FogBottomVS.hlsl";
		bottomPlaneRj.pipeline.PSStage.shader = "FogBottomPS.hlsl";

		bottomPlaneRj.pipeline.PSStage.samplers[0] = "FogSs";
		bottomPlaneRj.pipeline.PSStage.samplerCount = 1;

		bottomPlaneRj.pipeline.PSStage.textures[0] = "Fog_AlbedoTexture.png";
		bottomPlaneRj.pipeline.PSStage.textures[1] = "Fog_NormalTexture.png";
		bottomPlaneRj.pipeline.PSStage.textureCount = 2;
		bottomPlaneRj.pipeline.PSStage.textureBindings[0] = "albedoTexture";
		bottomPlaneRj.pipeline.PSStage.textureBindings[1] = "normalTexture";

		bottomPlaneRj.pipeline.IAStage.inputLayout = "FogBottomVS.hlsl";
		bottomPlaneRj.pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;
		bottomPlaneRj.pipeline.IAStage.vertexBuffer = bottomPlaneVbName;

		bottomPlaneRj.pipeline.OMStage.blendState = "FogBs";
		bottomPlaneRj.pipeline.OMStage.depthStencilState = "FogDss";
		bottomPlaneRj.pipeline.OMStage.depthStencilView = "backbuffer";

		bottomPlaneRj.vertexCount = bottomPlaneVertexCount;
		bottomPlaneRj.maxInstances = 1;
	}


	rjInitialized = true;
}

void SE::Gameplay::Fog::AddAdjacentTiles(unsigned int column, unsigned int row)
{
	if (column > 24 || row > 24)
		return;

	if (topPlane->GetQuadStatus(column + 1, row + 1))
		return;

	char tileType = tileValues[column][row];
	if (tileType != id_DeadArea && tileType != id_Wall && tileType != id_Door1 && tileType != id_Door2)
		return;


	topPlane->GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);
	topPlane->GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);
	topPlane->GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::BottomLeft, column, height, row);
	topPlane->GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);


	AddAdjacentTiles(column - 1, row);
	AddAdjacentTiles(column + 1, row);
	AddAdjacentTiles(column, row - 1);
	AddAdjacentTiles(column, row + 1);
}

unsigned int SE::Gameplay::Fog::GetSlopeCount()
{
	unsigned int slopeCount = 0;

	for (unsigned int rowI = 0; rowI < 25; rowI++)
	{
		for (unsigned int columnI = 0; columnI < 25; columnI++)
		{
			if (!topPlane->GetQuadStatus(columnI + 1, rowI + 1))
			{
				if (topPlane->GetQuadStatus(columnI + 2, rowI + 1))
					slopeCount++;

				if (topPlane->GetQuadStatus(columnI, rowI + 1))
					slopeCount++;

				if (topPlane->GetQuadStatus(columnI + 1, rowI))
					slopeCount++;

				if (topPlane->GetQuadStatus(columnI + 1, rowI + 2))
					slopeCount++;
			}
		}
	}

	return slopeCount;
}

void SE::Gameplay::Fog::SetFirstWall()
{
	for (unsigned int radiusI = 0; radiusI < 25; radiusI++)
	{
		for (unsigned int rowI = 0; rowI <= radiusI; rowI++)
		{
			for (unsigned int columnI = (rowI == radiusI) ? 0 : radiusI; columnI <= radiusI; columnI++)
			{
				if (!topPlane->GetQuadStatus(columnI + 1, rowI + 1))
				{
					currentWallTile[0] = firstWallTile[0] = columnI;
					currentWallTile[1] = firstWallTile[1] = rowI;

					currentWallSide = firstWallSide = WallSide::Bottom;

					goto endOfLoops;
				}
			}
		}
	}
endOfLoops:


	previousWallTile[0] = currentWallTile[0];
	previousWallTile[1] = currentWallTile[1];

	previousWallSide = WallSide::Left;


	if (!topPlane->GetQuadStatus(currentWallTile[0] + 2, currentWallTile[1]))
	{
		nextWallTile[0] = currentWallTile[0] + 1;
		nextWallTile[1] = currentWallTile[1] - 1;

		nextWallSide = WallSide::Left;
	}
	else if (!topPlane->GetQuadStatus(currentWallTile[0] + 2, currentWallTile[1] + 1))
	{
		nextWallTile[0] = currentWallTile[0] + 1;
		nextWallTile[1] = currentWallTile[1];

		nextWallSide = WallSide::Bottom;
	}
	else
	{
		nextWallTile[0] = currentWallTile[0];
		nextWallTile[1] = currentWallTile[1];

		nextWallSide = WallSide::Right;
	}
}

void SE::Gameplay::Fog::ContinueToNextWall()
{
	previousWallTile[0] = currentWallTile[0];
	previousWallTile[1] = currentWallTile[1];
	previousWallSide = currentWallSide;

	currentWallTile[0] = nextWallTile[0];
	currentWallTile[1] = nextWallTile[1];
	currentWallSide = nextWallSide;


	if (currentWallSide == WallSide::Top)
	{
		if (!topPlane->GetQuadStatus(currentWallTile[0], currentWallTile[1] + 2))
		{
			nextWallTile[0] = currentWallTile[0] - 1;
			nextWallTile[1] = currentWallTile[1] + 1;

			nextWallSide = WallSide::Right;
		}
		else if (!topPlane->GetQuadStatus(currentWallTile[0], currentWallTile[1] + 1))
		{
			nextWallTile[0] = currentWallTile[0] - 1;
			nextWallTile[1] = currentWallTile[1];

			nextWallSide = currentWallSide;
		}
		else
		{
			nextWallTile[0] = currentWallTile[0];
			nextWallTile[1] = currentWallTile[1];

			nextWallSide = WallSide::Left;
		}
	}
	else if (currentWallSide == WallSide::Bottom)
	{
		if (!topPlane->GetQuadStatus(currentWallTile[0] + 2, currentWallTile[1]))
		{
			nextWallTile[0] = currentWallTile[0] + 1;
			nextWallTile[1] = currentWallTile[1] - 1;

			nextWallSide = WallSide::Left;
		}
		else if (!topPlane->GetQuadStatus(currentWallTile[0] + 2, currentWallTile[1] + 1))
		{
			nextWallTile[0] = currentWallTile[0] + 1;
			nextWallTile[1] = currentWallTile[1];

			nextWallSide = currentWallSide;
		}
		else
		{
			nextWallTile[0] = currentWallTile[0];
			nextWallTile[1] = currentWallTile[1];

			nextWallSide = WallSide::Right;
		}
	}
	else if (currentWallSide == WallSide::Left)
	{
		if (!topPlane->GetQuadStatus(currentWallTile[0], currentWallTile[1]))
		{
			nextWallTile[0] = currentWallTile[0] - 1;
			nextWallTile[1] = currentWallTile[1] - 1;

			nextWallSide = WallSide::Top;
		}
		else if (!topPlane->GetQuadStatus(currentWallTile[0] + 1, currentWallTile[1]))
		{
			nextWallTile[0] = currentWallTile[0];
			nextWallTile[1] = currentWallTile[1] - 1;

			nextWallSide = currentWallSide;
		}
		else
		{
			nextWallTile[0] = currentWallTile[0];
			nextWallTile[1] = currentWallTile[1];

			nextWallSide = WallSide::Bottom;
		}
	}
	else // currentWallSide == WallSide::Right
	{
		if (!topPlane->GetQuadStatus(currentWallTile[0] + 2, currentWallTile[1] + 2))
		{
			nextWallTile[0] = currentWallTile[0] + 1;
			nextWallTile[1] = currentWallTile[1] + 1;

			nextWallSide = WallSide::Bottom;
		}
		else if (!topPlane->GetQuadStatus(currentWallTile[0] + 1, currentWallTile[1] + 2))
		{
			nextWallTile[0] = currentWallTile[0];
			nextWallTile[1] = currentWallTile[1] + 1;

			nextWallSide = currentWallSide;
		}
		else
		{
			nextWallTile[0] = currentWallTile[0];
			nextWallTile[1] = currentWallTile[1];

			nextWallSide = WallSide::Top;
		}
	}
}

void SE::Gameplay::Fog::AddSlope(unsigned int slopeIndex, EdgeAttributes& attributes, SlopeRandomization& randomization, bool onlyInstantiate)
{
	unsigned int basePosition[2] = {
		currentWallTile[0] + (currentWallSide == WallSide::Bottom || currentWallSide == WallSide::Right),
		currentWallTile[1] + (currentWallSide == WallSide::Top || currentWallSide == WallSide::Right)
	};


	bool wallIs_horizontal = currentWallSide == WallSide::Top || currentWallSide == WallSide::Bottom;
	bool wallIs_vertical = !wallIs_horizontal;


	int offsetMultiplier_side = (currentWallSide == WallSide::Bottom) - (currentWallSide == WallSide::Top) +
		(currentWallSide == WallSide::Left) - (currentWallSide == WallSide::Right);


	int offsetMultiplier_corner = (currentWallSide != nextWallSide);

	offsetMultiplier_corner -= 2 *
		((currentWallSide == WallSide::Top && nextWallSide == WallSide::Right) +
		(currentWallSide == WallSide::Bottom && nextWallSide == WallSide::Right) +
			(currentWallSide == WallSide::Left && nextWallSide == WallSide::Top) +
			(currentWallSide == WallSide::Right && nextWallSide == WallSide::Top));


	bool previousWallIsOpenDoor = IsOpenDoorSide(previousWallTile, previousWallSide);
	bool currentWallIsOpenDoor = IsOpenDoorSide(currentWallTile, currentWallSide);
	bool nextWallIsOpenDoor = IsOpenDoorSide(nextWallTile, nextWallSide);

	bool nextEdgeAtOpenDoor = currentWallIsOpenDoor || nextWallIsOpenDoor;

	bool shouldFlipSeam = previousWallIsOpenDoor;


	float offsetMultiplier_horizontal_side = wallIs_horizontal * offsetMultiplier_side;
	float offsetMultiplier_horizontal_corner = wallIs_horizontal * offsetMultiplier_corner;
	float offsetMultiplier_vertical_side = wallIs_vertical * offsetMultiplier_side;
	float offsetMultiplier_vertical_corner = wallIs_vertical * offsetMultiplier_corner;



	EdgeAttributes previousEdge = attributes;
	EdgeAttributes& nextEdge = attributes = EdgeAttributes();


	{
		nextEdge.rim.position[0] = basePosition[0] +
			offsetMultiplier_vertical_side * slopeRimOffset[0] +
			offsetMultiplier_horizontal_corner * slopeRimOffset[0];

		nextEdge.rim.position[1] = slopeRimOffset[1];

		nextEdge.rim.position[2] = basePosition[1] +
			offsetMultiplier_horizontal_side * slopeRimOffset[0] +
			offsetMultiplier_vertical_corner * slopeRimOffset[0];


		nextEdge.top.position[0] = basePosition[0] +
			offsetMultiplier_vertical_side * slopeTopOffset[0] +
			offsetMultiplier_horizontal_corner * slopeTopOffset[0];

		nextEdge.top.position[1] = slopeTopOffset[1];

		nextEdge.top.position[2] = basePosition[1] +
			offsetMultiplier_horizontal_side * slopeTopOffset[0] +
			offsetMultiplier_vertical_corner * slopeTopOffset[0];


		nextEdge.middle.position[0] = basePosition[0] +
			offsetMultiplier_vertical_side * slopeMiddleOffset[0] +
			offsetMultiplier_horizontal_corner * slopeMiddleOffset[0];

		nextEdge.middle.position[1] = slopeMiddleOffset[1];

		nextEdge.middle.position[2] = basePosition[1] +
			offsetMultiplier_horizontal_side * slopeMiddleOffset[0] +
			offsetMultiplier_vertical_corner * slopeMiddleOffset[0];


		nextEdge.bottom.position[0] = basePosition[0] +
			offsetMultiplier_vertical_side * slopeBottomOffset[0] +
			offsetMultiplier_horizontal_corner * slopeBottomOffset[0];

		nextEdge.bottom.position[1] = slopeBottomOffset[1];

		nextEdge.bottom.position[2] = basePosition[1] +
			offsetMultiplier_horizontal_side * slopeBottomOffset[0] +
			offsetMultiplier_vertical_corner * slopeBottomOffset[0];
	}


	{
		nextEdge.rim.uv[0] = nextEdge.top.uv[0] = nextEdge.middle.uv[0] = nextEdge.bottom.uv[0] =
			slopeIndex;

		nextEdge.rim.uv[1] = nextEdge.rim.position[1];
		nextEdge.top.uv[1] = nextEdge.top.position[1];
		nextEdge.middle.uv[1] = nextEdge.middle.position[1];
		nextEdge.bottom.uv[1] = nextEdge.bottom.position[1];
	}


	{
		nextEdge.rim.opacity = rimOpacity;

		nextEdge.top.opacity = !nextEdgeAtOpenDoor * slopeTopOpacity + nextEdgeAtOpenDoor * slopeTopOpacity_door;

		nextEdge.middle.opacity = !nextEdgeAtOpenDoor * slopeMiddleOpacity + nextEdgeAtOpenDoor * slopeMiddleOpacity_door;

		nextEdge.bottom.opacity = !nextEdgeAtOpenDoor * slopeBottomOpacity + nextEdgeAtOpenDoor * slopeBottomOpacity_door;
	}


	{
		nextEdge.rim.position[0] += randomization.offsets.rim.position[0];
		nextEdge.rim.position[1] += randomization.offsets.rim.position[1];
		nextEdge.rim.position[2] += randomization.offsets.rim.position[2];

		nextEdge.rim.uv[0] += randomization.offsets.rim.uv[0];
		nextEdge.rim.uv[1] += randomization.offsets.rim.uv[1];

		nextEdge.rim.opacity += randomization.offsets.rim.opacity;


		nextEdge.top.position[0] += randomization.offsets.top.position[0];
		nextEdge.top.position[1] += randomization.offsets.top.position[1];
		nextEdge.top.position[2] += randomization.offsets.top.position[2];

		nextEdge.top.uv[0] += randomization.offsets.top.uv[0];
		nextEdge.top.uv[1] += randomization.offsets.top.uv[1];

		nextEdge.top.opacity += randomization.offsets.top.opacity;


		nextEdge.middle.position[0] += randomization.offsets.middle.position[0];
		nextEdge.middle.position[1] += randomization.offsets.middle.position[1];
		nextEdge.middle.position[2] += randomization.offsets.middle.position[2];

		nextEdge.middle.uv[0] += randomization.offsets.middle.uv[0];
		nextEdge.middle.uv[1] += randomization.offsets.middle.uv[1];

		nextEdge.middle.opacity += randomization.offsets.middle.opacity;


		nextEdge.bottom.position[0] += randomization.offsets.bottom.position[0];
		nextEdge.bottom.position[1] += randomization.offsets.bottom.position[1];
		nextEdge.bottom.position[2] += randomization.offsets.bottom.position[2];

		nextEdge.bottom.uv[0] += randomization.offsets.bottom.uv[0];
		nextEdge.bottom.uv[1] += randomization.offsets.bottom.uv[1];

		nextEdge.bottom.opacity += randomization.offsets.bottom.opacity;
	}



	if (onlyInstantiate)
		return;



	AdjustRimVertex(nextEdge.top.position);



	{
		topPlane->AddQuad();


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, previousEdge.top.position[0], previousEdge.top.position[1], previousEdge.top.position[2], previousEdge.top.opacity);
		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, previousEdge.rim.position[0], previousEdge.rim.position[1], previousEdge.rim.position[2], previousEdge.rim.opacity);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopRight, Quad::Vertices::BottomRight);


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, nextEdge.top.position[0], nextEdge.top.position[1], nextEdge.top.position[2], nextEdge.top.opacity);
		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, nextEdge.rim.position[0], nextEdge.rim.position[1], nextEdge.rim.position[2], nextEdge.rim.opacity);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopLeft, Quad::Vertices::BottomLeft);
	}


	{
		bottomPlane->AddQuad();


		bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, previousEdge.top.position[0], previousEdge.top.position[1], previousEdge.top.position[2], slopeIndex - 1, previousEdge.top.position[1], previousEdge.top.opacity);
		bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, previousEdge.middle.position[0], previousEdge.middle.position[1], previousEdge.middle.position[2], slopeIndex - 1, previousEdge.middle.position[1], previousEdge.middle.opacity);

		bottomPlane->GetQuad().AdjustV(Quad::Vertices::TopRight, Quad::Vertices::BottomRight);


		bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, nextEdge.top.position[0], nextEdge.top.position[1], nextEdge.top.position[2], slopeIndex, nextEdge.top.position[1], nextEdge.top.opacity);
		bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, nextEdge.middle.position[0], nextEdge.middle.position[1], nextEdge.middle.position[2], slopeIndex, nextEdge.middle.position[1], nextEdge.middle.opacity);

		bottomPlane->GetQuad().AdjustV(Quad::Vertices::TopLeft, Quad::Vertices::BottomLeft);



		bottomPlane->AddQuad();


		bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, previousEdge.middle.position[0], previousEdge.middle.position[1], previousEdge.middle.position[2], slopeIndex - 1, previousEdge.middle.position[1], previousEdge.middle.opacity);
		bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, previousEdge.bottom.position[0], previousEdge.bottom.position[1], previousEdge.bottom.position[2], slopeIndex - 1, previousEdge.bottom.position[1], previousEdge.bottom.opacity);

		bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::TopRight, Quad::Vertices::TopRight);
		bottomPlane->GetQuad().AdjustV(Quad::Vertices::TopRight, Quad::Vertices::BottomRight);


		bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, nextEdge.middle.position[0], nextEdge.middle.position[1], nextEdge.middle.position[2], slopeIndex, nextEdge.middle.position[1], nextEdge.middle.opacity);
		bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, nextEdge.bottom.position[0], nextEdge.bottom.position[1], nextEdge.bottom.position[2], slopeIndex, nextEdge.bottom.position[1], nextEdge.bottom.opacity);

		bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::TopLeft, Quad::Vertices::TopLeft);
		bottomPlane->GetQuad().AdjustV(Quad::Vertices::TopLeft, Quad::Vertices::BottomLeft);



		if (shouldFlipSeam)
		{
			bottomPlane->GetQuad(-1).FlipSeam();
			bottomPlane->GetQuad().FlipSeam();
		}
	}
}

void SE::Gameplay::Fog::AdjustRimVertex(float position[3])
{
	unsigned int tileOffset[2] = {};


	if (currentWallSide == WallSide::Top)
		++tileOffset[1];
	else if (currentWallSide == WallSide::Bottom)
		--tileOffset[1];
	else if (currentWallSide == WallSide::Left)
		--tileOffset[0];
	else // currentWallSide == WallSide::Right
		++tileOffset[0];


	unsigned int rimTile[2] = { currentWallTile[0] + tileOffset[0] + 1, currentWallTile[1] + tileOffset[1] + 1 };

	if (topPlane->GetQuadStatus(rimTile[0], rimTile[1]))
	{
		if (currentWallSide == WallSide::Top)
		{
			topPlane->GetQuad(rimTile[0], rimTile[1]).SetPosition(Quad::Vertices::BottomLeft, position[0], position[1], position[2]);

			if (nextWallSide == WallSide::Top || nextWallSide == WallSide::Left)
				topPlane->GetQuad(rimTile[0] - 1, rimTile[1]).SetPosition(Quad::Vertices::BottomRight, position[0], position[1], position[2]);

			if (nextWallSide == WallSide::Left)
				topPlane->GetQuad(rimTile[0] - 1, rimTile[1] - 1).SetPosition(Quad::Vertices::TopRight, position[0], position[1], position[2]);
		}
		else if (currentWallSide == WallSide::Bottom)
		{
			topPlane->GetQuad(rimTile[0], rimTile[1]).SetPosition(Quad::Vertices::TopRight, position[0], position[1], position[2]);

			if (nextWallSide == WallSide::Bottom || nextWallSide == WallSide::Right)
				topPlane->GetQuad(rimTile[0] + 1, rimTile[1]).SetPosition(Quad::Vertices::TopLeft, position[0], position[1], position[2]);

			if (nextWallSide == WallSide::Right)
				topPlane->GetQuad(rimTile[0] + 1, rimTile[1] + 1).SetPosition(Quad::Vertices::BottomLeft, position[0], position[1], position[2]);
		}
		else if (currentWallSide == WallSide::Left)
		{
			topPlane->GetQuad(rimTile[0], rimTile[1]).SetPosition(Quad::Vertices::BottomRight, position[0], position[1], position[2]);

			if (nextWallSide == WallSide::Left || nextWallSide == WallSide::Bottom)
				topPlane->GetQuad(rimTile[0], rimTile[1] - 1).SetPosition(Quad::Vertices::TopRight, position[0], position[1], position[2]);

			if (nextWallSide == WallSide::Bottom)
				topPlane->GetQuad(rimTile[0] + 1, rimTile[1] - 1).SetPosition(Quad::Vertices::TopLeft, position[0], position[1], position[2]);
		}
		else // currentWallSide == WallSide::Right
		{
			topPlane->GetQuad(rimTile[0], rimTile[1]).SetPosition(Quad::Vertices::TopLeft, position[0], position[1], position[2]);

			if (nextWallSide == WallSide::Right || nextWallSide == WallSide::Top)
				topPlane->GetQuad(rimTile[0], rimTile[1] + 1).SetPosition(Quad::Vertices::BottomLeft, position[0], position[1], position[2]);

			if (nextWallSide == WallSide::Top)
				topPlane->GetQuad(rimTile[0] - 1, rimTile[1] + 1).SetPosition(Quad::Vertices::BottomRight, position[0], position[1], position[2]);
		}
	}
}

float SE::Gameplay::Fog::GetFreeSpaceAt(float column, float row)
{
	if (column > 24 || row > 24 || topPlane->GetQuadStatus(column + 1, row + 1))
		return 0;


	float rotationSine = sin(6.283 / spaceCheckRotationIterationCount);
	float rotationCosine = cos(6.283 / spaceCheckRotationIterationCount);


	float freeSpace = 1;

	float freeSpace_old;
	do {
		freeSpace_old = freeSpace;


		float rotationVector[2] = { 0, freeSpace };

		for (unsigned int rotationI = 0; rotationI < spaceCheckRotationIterationCount; rotationI++)
		{
			if (!topPlane->GetQuadStatus(column + rotationVector[0] + 1, row + rotationVector[1] + 1))
			{
				freeSpace += 1.f / spaceCheckRotationIterationCount;
			}


			float rotationVector_oldX = rotationVector[0];

			rotationVector[0] = rotationCosine * rotationVector[0] - rotationSine * rotationVector[1];
			rotationVector[1] = rotationSine * rotationVector_oldX + rotationCosine * rotationVector[1];
		}

	} while (freeSpace - (long)freeSpace < 0.00001 && freeSpace != freeSpace_old);


	freeSpace -= (freeSpace - (long)freeSpace) / 1.5f;

	float centerOffsetVector[2] = { column - roomCenter[0], row - roomCenter[1] };
	float centerOffsetDistance = sqrt(centerOffsetVector[0] * centerOffsetVector[0] + centerOffsetVector[1] * centerOffsetVector[1]);

	float maxOffsetDistance = sqrt(roomCenter[0] * roomCenter[0] + roomCenter[1] * roomCenter[1]);

	freeSpace += (1 - (centerOffsetDistance / maxOffsetDistance)) / 1.5f;


	return freeSpace;
}

void SE::Gameplay::Fog::AdjustOptimalCenter(float (&centerPosition)[2])
{
	if (centerPosition[0] >= 25 || centerPosition[1] >= 25 || topPlane->GetQuadStatus(centerPosition[0] + 1, centerPosition[1] + 1))
		return;


	float rotationSine = sin(6.283 / spaceCheckRotationIterationCount);
	float rotationCosine = cos(6.283 / spaceCheckRotationIterationCount);


	for (unsigned int moveI = 0; moveI < 10; moveI++)
	{
		float wallPositionSum[2] = {};
		unsigned int wallPositionCount = 0;

		float freeSpace = 0.25;
		do {
			float rotationVector[2] = { 0, freeSpace };

			for (unsigned int rotationI = 0; rotationI < spaceCheckRotationIterationCount; rotationI++)
			{
				if (topPlane->GetQuadStatus(centerPosition[0] + rotationVector[0] + 1, centerPosition[1] + rotationVector[1] + 1))
				{
					wallPositionSum[0] += (unsigned int)(centerPosition[0] + rotationVector[0]) + 0.5;
					wallPositionSum[1] += (unsigned int)(centerPosition[1] + rotationVector[1]) + 0.5;

					wallPositionCount++;
				}


				float rotationVector_oldX = rotationVector[0];

				rotationVector[0] = rotationCosine * rotationVector[0] - rotationSine * rotationVector[1];
				rotationVector[1] = rotationSine * rotationVector_oldX + rotationCosine * rotationVector[1];
			}

			if (!wallPositionCount)
				freeSpace += std::min(1.f, freeSpace);

		} while (!wallPositionCount);


		float wallOffsetVector[2] = { wallPositionSum[0] / wallPositionCount - centerPosition[0], wallPositionSum[1] / wallPositionCount - centerPosition[1] };
		float wallOffsetDistance = sqrt(wallOffsetVector[0] * wallOffsetVector[0] + wallOffsetVector[1] * wallOffsetVector[1]);

		float wallOffsetVector_normalized[2] = { wallOffsetVector[0] / wallOffsetDistance, wallOffsetVector[1] / wallOffsetDistance };

		float stepSizeMultiplier = std::min(1.f, wallOffsetDistance / 1.5f);

		centerPosition[0] -= (1 - moveI / 10.f) * stepSizeMultiplier * wallOffsetVector_normalized[0];
		centerPosition[1] -= (1 - moveI / 10.f) * stepSizeMultiplier * wallOffsetVector_normalized[1];
	}
}

bool SE::Gameplay::Fog::IsOpenDoorSide(const unsigned int (&tileOutsideDoor)[2], WallSide doorSide)
{
	bool isOpenDoor = false;


	unsigned int doorTile[2] = { tileOutsideDoor[0], tileOutsideDoor[1] };

	if (doorSide == WallSide::Top)
		doorTile[1]++;
	else if (doorSide == WallSide::Bottom)
		doorTile[1]--;
	else if (doorSide == WallSide::Left)
		doorTile[0]--;
	else // doorSide == WallSide::Right
		doorTile[0]++;


	if (doorTile[0] < 25 && doorTile[1] < 25)
	{
		char doorTileValue = tileValues[doorTile[0]][doorTile[1]];

		if (doorTileValue == id_Door1 || doorTileValue == id_Door2)
		{
			unsigned int adjacentFloorTile[2] = { doorTile[0], doorTile[1] };

			if (doorTile[0] < 25 && doorTile[1] + 1 < 25 && tileValues[doorTile[0]][doorTile[1] + 1] == id_Floor)
				adjacentFloorTile[1]++;
			else if (doorTile[0] < 25 && (unsigned int)(doorTile[1] - 1) < 25 && tileValues[doorTile[0]][doorTile[1] - 1] == id_Floor)
				adjacentFloorTile[1]--;
			else if ((unsigned int)(doorTile[0] - 1) < 25 && doorTile[1] < 25 && tileValues[doorTile[0] - 1][doorTile[1]] == id_Floor)
				adjacentFloorTile[0]--;
			else if (doorTile[0] + 1 < 25 && doorTile[1] < 25 && tileValues[doorTile[0] + 1][doorTile[1]] == id_Floor)
				adjacentFloorTile[0]++;


			if (tileOutsideDoor[0] == adjacentFloorTile[0] && tileOutsideDoor[0] == adjacentFloorTile[0])
				isOpenDoor = true;
		}
	}


	return isOpenDoor;
}

char SE::Gameplay::Fog::GetTileValue(unsigned int column, unsigned int row)
{
	char tileValue = 0;

	if (column < 25 && row < 25)
		tileValue = tileValues[column][row];

	return tileValue;
}
