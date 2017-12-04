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
	float position[3];
	float uv[2];
	float opacity;

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
	void AdjustUv(Quad &parentQuad, Vertices parent, Vertices child)
	{
		float parentU, parentV;

		if (parent == Vertices::TopLeft)
		{
			parentU = parentQuad.vertices[0].uv[0];
			parentV = parentQuad.vertices[0].uv[1];
		}
		else if (parent == Vertices::TopRight)
		{
			parentU = parentQuad.vertices[1].uv[0];
			parentV = parentQuad.vertices[1].uv[1];
		}
		else if (parent == Vertices::BottomLeft)
		{
			parentU = parentQuad.vertices[5].uv[0];
			parentV = parentQuad.vertices[5].uv[1];
		}
		else if (parent == Vertices::BottomRight)
		{
			parentU = parentQuad.vertices[2].uv[0];
			parentV = parentQuad.vertices[2].uv[1];
		}


		float uVector[2];

		if (child == Vertices::TopLeft)
		{
			uVector[0] = vertices[0].position[0];
			uVector[1] = vertices[0].position[1];
		}
		else if (child == Vertices::TopRight)
		{
			uVector[0] = vertices[1].position[0];
			uVector[1] = vertices[1].position[1];
		}
		else if (child == Vertices::BottomLeft)
		{
			uVector[0] = vertices[5].position[0];
			uVector[1] = vertices[5].position[1];
		}
		else if (child == Vertices::BottomRight)
		{
			uVector[0] = vertices[2].position[0];
			uVector[1] = vertices[2].position[1];
		}

		if (parent == Vertices::TopLeft)
		{
			uVector[0] -= parentQuad.vertices[0].position[0];
			uVector[1] -= parentQuad.vertices[0].position[1];
		}
		else if (parent == Vertices::TopRight)
		{
			uVector[0] -= parentQuad.vertices[1].position[0];
			uVector[1] -= parentQuad.vertices[1].position[1];
		}
		else if (parent == Vertices::BottomLeft)
		{
			uVector[0] -= parentQuad.vertices[5].position[0];
			uVector[1] -= parentQuad.vertices[5].position[1];
		}
		else if (parent == Vertices::BottomRight)
		{
			uVector[0] -= parentQuad.vertices[2].position[0];
			uVector[1] -= parentQuad.vertices[2].position[1];
		}


		float vVector[2];

		if (child == Vertices::TopLeft)
		{
			vVector[0] = vertices[0].position[2];
			vVector[1] = vertices[0].position[1];
		}
		else if (child == Vertices::TopRight)
		{
			vVector[0] = vertices[1].position[2];
			vVector[1] = vertices[1].position[1];
		}
		else if (child == Vertices::BottomLeft)
		{
			vVector[0] = vertices[5].position[2];
			vVector[1] = vertices[5].position[1];
		}
		else if (child == Vertices::BottomRight)
		{
			vVector[0] = vertices[2].position[2];
			vVector[1] = vertices[2].position[1];
		}

		if (parent == Vertices::TopLeft)
		{
			vVector[0] -= parentQuad.vertices[0].position[2];
			vVector[1] -= parentQuad.vertices[0].position[1];
		}
		else if (parent == Vertices::TopRight)
		{
			vVector[0] -= parentQuad.vertices[1].position[2];
			vVector[1] -= parentQuad.vertices[1].position[1];
		}
		else if (parent == Vertices::BottomLeft)
		{
			vVector[0] -= parentQuad.vertices[5].position[2];
			vVector[1] -= parentQuad.vertices[5].position[1];
		}
		else if (parent == Vertices::BottomRight)
		{
			vVector[0] -= parentQuad.vertices[2].position[2];
			vVector[1] -= parentQuad.vertices[2].position[1];
		}


		float uDistance = sqrt(pow(uVector[0], 2) + pow(uVector[1], 2));
		float vDistance = sqrt(pow(vVector[0], 2) + pow(vVector[1], 2));

		if (child == Vertices::TopLeft)
		{
			vertices[0].uv[0] = parentU + ((uVector[0] > 0) - (uVector[0] < 0)) * uDistance;
			vertices[0].uv[1] = parentV + ((vVector[0] > 0) - (vVector[0] < 0)) * vDistance;

			vertices[3].uv[0] = parentU + ((uVector[0] > 0) - (uVector[0] < 0)) * uDistance;
			vertices[3].uv[1] = parentV + ((vVector[0] > 0) - (vVector[0] < 0)) * vDistance;
		}
		else if (child == Vertices::TopRight)
		{
			vertices[1].uv[0] = parentU + ((uVector[0] > 0) - (uVector[0] < 0)) * uDistance;
			vertices[1].uv[1] = parentV + ((vVector[0] > 0) - (vVector[0] < 0)) * vDistance;
		}
		else if (child == Vertices::BottomLeft)
		{
			vertices[5].uv[0] = parentU + ((uVector[0] > 0) - (uVector[0] < 0)) * uDistance;
			vertices[5].uv[1] = parentV + ((vVector[0] > 0) - (vVector[0] < 0)) * vDistance;
		}
		else if (child == Vertices::BottomRight)
		{
			vertices[2].uv[0] = parentU + ((uVector[0] > 0) - (uVector[0] < 0)) * uDistance;
			vertices[2].uv[1] = parentV + ((vVector[0] > 0) - (vVector[0] < 0)) * vDistance;

			vertices[4].uv[0] = parentU + ((uVector[0] > 0) - (uVector[0] < 0)) * uDistance;
			vertices[4].uv[1] = parentV + ((vVector[0] > 0) - (vVector[0] < 0)) * vDistance;
		}
	}
	void AdjustUv(Vertices parent, Vertices child)
	{
		AdjustUv(*this, parent, child);
	}
	void AdjustV(Quad &parentQuad, Vertices parent, Vertices child)
	{
		float parentV;

		if (parent == Vertices::TopLeft)
		{
			parentV = parentQuad.vertices[0].uv[1];
		}
		else if (parent == Vertices::TopRight)
		{
			parentV = parentQuad.vertices[1].uv[1];
		}
		else if (parent == Vertices::BottomLeft)
		{
			parentV = parentQuad.vertices[5].uv[1];
		}
		else if (parent == Vertices::BottomRight)
		{
			parentV = parentQuad.vertices[2].uv[1];
		}


		float vVector[3];

		if (child == Vertices::TopLeft)
		{
			vVector[0] = vertices[0].position[0];
			vVector[1] = vertices[0].position[1];
			vVector[2] = vertices[0].position[2];
		}
		else if (child == Vertices::TopRight)
		{
			vVector[0] = vertices[1].position[0];
			vVector[1] = vertices[1].position[1];
			vVector[2] = vertices[1].position[2];
		}
		else if (child == Vertices::BottomLeft)
		{
			vVector[0] = vertices[5].position[0];
			vVector[1] = vertices[5].position[1];
			vVector[2] = vertices[5].position[2];
		}
		else if (child == Vertices::BottomRight)
		{
			vVector[0] = vertices[2].position[0];
			vVector[1] = vertices[2].position[1];
			vVector[2] = vertices[2].position[2];
		}

		if (parent == Vertices::TopLeft)
		{
			vVector[0] -= parentQuad.vertices[0].position[0];
			vVector[1] -= parentQuad.vertices[0].position[1];
			vVector[2] -= parentQuad.vertices[0].position[2];
		}
		else if (parent == Vertices::TopRight)
		{
			vVector[0] -= parentQuad.vertices[1].position[0];
			vVector[1] -= parentQuad.vertices[1].position[1];
			vVector[2] -= parentQuad.vertices[1].position[2];
		}
		else if (parent == Vertices::BottomLeft)
		{
			vVector[0] -= parentQuad.vertices[5].position[0];
			vVector[1] -= parentQuad.vertices[5].position[1];
			vVector[2] -= parentQuad.vertices[5].position[2];
		}
		else if (parent == Vertices::BottomRight)
		{
			vVector[0] -= parentQuad.vertices[2].position[0];
			vVector[1] -= parentQuad.vertices[2].position[1];
			vVector[2] -= parentQuad.vertices[2].position[2];
		}


		float vDistance = sqrt(pow(vVector[0], 2) + pow(vVector[1], 2) + pow(vVector[2], 2));

		if (child == Vertices::TopLeft)
		{
			vertices[0].uv[1] = parentV + ((vVector[1] > 0) - (vVector[1] < 0)) * vDistance;

			vertices[3].uv[1] = parentV + ((vVector[1] > 0) - (vVector[1] < 0)) * vDistance;
		}
		else if (child == Vertices::TopRight)
		{
			vertices[1].uv[1] = parentV + ((vVector[1] > 0) - (vVector[1] < 0)) * vDistance;
		}
		else if (child == Vertices::BottomLeft)
		{
			vertices[5].uv[1] = parentV + ((vVector[1] > 0) - (vVector[1] < 0)) * vDistance;
		}
		else if (child == Vertices::BottomRight)
		{
			vertices[2].uv[1] = parentV + ((vVector[1] > 0) - (vVector[1] < 0)) * vDistance;

			vertices[4].uv[1] = parentV + ((vVector[1] > 0) - (vVector[1] < 0)) * vDistance;
		}
	}
	void AdjustV(Vertices parent, Vertices child)
	{
		AdjustV(*this, parent, child);
	}
	void OffsetUV(float uOffset, float vOffset)
	{
		for (unsigned int vertexI = 0; vertexI < 6; vertexI++)
		{
			vertices[vertexI].uv[0] += uOffset;
			vertices[vertexI].uv[1] += vOffset;
		}
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
		if (row * 27 + column < 27 * 27)
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
	void OffsetUvs(float uOffset, float vOffset)
	{
		for (unsigned int quadI = 0; quadI < 27 * 27; quadI++)
		{
			if (quads_used[quadI])
				quads[quadI].OffsetUV(uOffset, vOffset);
		}

		for (unsigned int extraQuadI = 0; extraQuadI < extraQuads.size(); extraQuadI++)
		{
			extraQuads[extraQuadI].OffsetUV(uOffset, vOffset);
		}
	}
};


SE::Gameplay::Fog::Fog()
{
	topPlane = new Plane();
	bottomPlane = new Plane();
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
		topPlaneRjHandles[0] = CoreInit::subSystems.renderer->AddRenderJob(topPlaneRjs[0], topPlane_renderGroup);
		topPlaneRjHandles[1] = CoreInit::subSystems.renderer->AddRenderJob(topPlaneRjs[1], topPlane_renderGroup);

		bottomPlaneRjHandle = CoreInit::subSystems.renderer->AddRenderJob(bottomPlaneRj, bottomPlane_renderGroup);

		rjEnabled = true;
	}
	else if (!status && rjEnabled && topPlaneRjHandles[0] != -1)
	{
		CoreInit::subSystems.renderer->RemoveRenderJob(topPlaneRjHandles[0]);
		CoreInit::subSystems.renderer->RemoveRenderJob(topPlaneRjHandles[1]);

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
		SetFirstWall();

		unsigned int slopeCount = GetSlopeCount();

		unsigned int slopeIndex = 0;
		do
		{
			AddSlope(slopeIndex);

			ContinueToNextWall();

			slopeIndex++;

		} while (slopeIndex < slopeCount);
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

				rotationVector[0] = rotationCosine * rotationVector[0] - rotationSine * rotationVector[1];
				rotationVector[1] = rotationSine * rotationVector[0] + rotationCosine * rotationVector[1];
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


		topPlaneRjs[0].pipeline.OMStage.renderTargets[0] = "backbuffer";
		topPlaneRjs[0].pipeline.OMStage.renderTargetCount = 1;

		topPlaneRjs[0].pipeline.VSStage.shader = "FogVS.hlsl";
		topPlaneRjs[0].pipeline.PSStage.shader = "FogTopPS.hlsl";

		topPlaneRjs[0].pipeline.PSStage.samplers[0] = "FogSs";
		topPlaneRjs[0].pipeline.PSStage.samplerCount = 1;

		topPlaneRjs[0].pipeline.PSStage.textures[0] = "Fog_AlbedoTexture.png";
		topPlaneRjs[0].pipeline.PSStage.textures[1] = "Fog_NormalTexture.png";
		topPlaneRjs[0].pipeline.PSStage.textureCount = 2;
		topPlaneRjs[0].pipeline.PSStage.textureBindings[0] = "albedoTexture";
		topPlaneRjs[0].pipeline.PSStage.textureBindings[1] = "normalTexture";

		topPlaneRjs[0].pipeline.IAStage.inputLayout = "FogVS.hlsl";
		topPlaneRjs[0].pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;
		topPlaneRjs[0].pipeline.IAStage.vertexBuffer = topPlaneVbName;

		topPlaneRjs[0].pipeline.OMStage.blendState = "FogBs";
		topPlaneRjs[0].pipeline.OMStage.depthStencilState = "FogDss";
		topPlaneRjs[0].pipeline.OMStage.depthStencilView = "backbuffer";

		topPlaneRjs[0].vertexCount = topPlaneVertexCount;
		topPlaneRjs[0].maxInstances = 1;


		topPlaneRjs[1] = topPlaneRjs[0];

		topPlaneRjs[1].mappingFunc.push_back([time](int a, int b)
		{
			float offsetTime = *time + 6;
			CoreInit::subSystems.renderer->GetPipelineHandler()->UpdateConstantBuffer("timeBuffer", &offsetTime, sizeof(float));
		});



		unsigned int bottomPlaneVertexCount = bottomPlane->GetQuadCount() * 2 * 3;

		Vertex *bottomPlaneVertexBuffer = new Vertex[bottomPlaneVertexCount];
		bottomPlane->GetVertexBuffer(bottomPlaneVertexBuffer);

		std::string bottomPlaneVbName = std::string("FogBottomPlaneVb#" + std::to_string(fogIndex));

		CoreInit::subSystems.renderer->GetPipelineHandler()->CreateVertexBuffer(bottomPlaneVbName, bottomPlaneVertexBuffer, bottomPlaneVertexCount, sizeof(Vertex));

		fogIndex++;


		bottomPlaneRj.pipeline.OMStage.renderTargets[0] = "backbuffer";
		bottomPlaneRj.pipeline.OMStage.renderTargetCount = 1;

		bottomPlaneRj.pipeline.VSStage.shader = "FogVS.hlsl";
		bottomPlaneRj.pipeline.PSStage.shader = "FogBottomPS.hlsl";

		bottomPlaneRj.pipeline.PSStage.samplers[0] = "FogSs";
		bottomPlaneRj.pipeline.PSStage.samplerCount = 1;

		bottomPlaneRj.pipeline.PSStage.textures[0] = "Fog_AlbedoTexture.png";
		bottomPlaneRj.pipeline.PSStage.textures[1] = "Fog_NormalTexture.png";
		bottomPlaneRj.pipeline.PSStage.textureCount = 2;
		bottomPlaneRj.pipeline.PSStage.textureBindings[0] = "albedoTexture";
		bottomPlaneRj.pipeline.PSStage.textureBindings[1] = "normalTexture";

		bottomPlaneRj.pipeline.IAStage.inputLayout = "FogVS.hlsl";
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

void SE::Gameplay::Fog::AddSlope(unsigned int slopeIndex)
{
	if (currentWallSide == WallSide::Top)
	{
		topPlane->AddQuad();


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0], height, currentWallTile[1] + 1);

		if (nextWallSide == WallSide::Left)
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1 - slopeTopOffset[0]);
		else if (nextWallSide == WallSide::Top)
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0], slopeTopOffset[1], currentWallTile[1] + 1 - slopeTopOffset[0]);
		else // nextWallSide == WallSide::Right
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1 - slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopLeft, Quad::Vertices::BottomLeft);


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1, height, currentWallTile[1] + 1);

		if (previousWallSide == WallSide::Right)
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1 - slopeTopOffset[0]);
		else if (previousWallSide == WallSide::Top)
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1, slopeTopOffset[1], currentWallTile[1] + 1 - slopeTopOffset[0]);
		else // previousWallSide == WallSide::Left
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1 - slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopRight, Quad::Vertices::BottomRight);



		char tileType = GetTileValue(currentWallTile[0], currentWallTile[1] + 1);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane->AddQuad();
			bottomPlane->AddQuad();


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, currentWallTile[0], height, currentWallTile[1] + 1, slopeIndex + 1, height);

			if (nextWallSide == WallSide::Left)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1 - slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}
			else if (nextWallSide == WallSide::Top)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0], slopeBottomOffset[1], currentWallTile[1] + 1 - slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}
			else // nextWallSide == WallSide::Right
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1 - slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}

			bottomPlane->GetQuad(-1).AdjustV(Quad::Vertices::TopLeft, Quad::Vertices::BottomLeft);

			bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::TopLeft, Quad::Vertices::TopLeft);
			bottomPlane->GetQuad().AdjustV(Quad::Vertices::TopLeft, Quad::Vertices::BottomLeft);


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1, height, currentWallTile[1] + 1, slopeIndex, height);

			if (previousWallSide == WallSide::Right)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1 - slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}
			else if (previousWallSide == WallSide::Top)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1, slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1, slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1, slopeBottomOffset[1], currentWallTile[1] + 1 - slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}
			else // previousWallSide == WallSide::Left
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1 - slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}

			bottomPlane->GetQuad(-1).AdjustV(Quad::Vertices::TopRight, Quad::Vertices::BottomRight);

			bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::TopRight, Quad::Vertices::TopRight);
			bottomPlane->GetQuad().AdjustV(Quad::Vertices::TopRight, Quad::Vertices::BottomRight);
		}
	}
	else if (currentWallSide == WallSide::Bottom)
	{
		topPlane->AddQuad();


		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0], height, currentWallTile[1]);

		if (previousWallSide == WallSide::Left)
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + slopeTopOffset[0]);
		else if (previousWallSide == WallSide::Bottom)
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0], slopeTopOffset[1], currentWallTile[1] + slopeTopOffset[0]);
		else // previousWallSide == WallSide::Right
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::BottomLeft, Quad::Vertices::TopLeft);


		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1, height, currentWallTile[1]);

		if (nextWallSide == WallSide::Right)
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + slopeTopOffset[0]);
		else if (nextWallSide == WallSide::Bottom)
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1, slopeTopOffset[1], currentWallTile[1] + slopeTopOffset[0]);
		else // nextWallSide == WallSide::Left
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::BottomRight, Quad::Vertices::TopRight);



		char tileType = GetTileValue(currentWallTile[0], currentWallTile[1] - 1);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane->AddQuad();
			bottomPlane->AddQuad();


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0], height, currentWallTile[1], slopeIndex, height);

			if (previousWallSide == WallSide::Left)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}
			else if (previousWallSide == WallSide::Bottom)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, currentWallTile[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0], slopeBottomOffset[1], currentWallTile[1] + slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}
			else // previousWallSide == WallSide::Right
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}

			bottomPlane->GetQuad(-1).AdjustV(Quad::Vertices::BottomLeft, Quad::Vertices::TopLeft);

			bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::BottomLeft, Quad::Vertices::BottomLeft);
			bottomPlane->GetQuad().AdjustV(Quad::Vertices::BottomLeft, Quad::Vertices::TopLeft);


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1, height, currentWallTile[1], slopeIndex + 1, height);

			if (nextWallSide == WallSide::Right)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}
			else if (nextWallSide == WallSide::Bottom)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1, slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1, slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1, slopeBottomOffset[1], currentWallTile[1] + slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}
			else // nextWallSide == WallSide::Left
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}

			bottomPlane->GetQuad(-1).AdjustV(Quad::Vertices::BottomRight, Quad::Vertices::TopRight);

			bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::BottomRight, Quad::Vertices::BottomRight);
			bottomPlane->GetQuad().AdjustV(Quad::Vertices::BottomRight, Quad::Vertices::TopRight);
		}
	}
	else if (currentWallSide == WallSide::Left)
	{
		topPlane->AddQuad();


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0], height, currentWallTile[1] + 1);

		if (previousWallSide == WallSide::Top)
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1 - slopeTopOffset[0]);
		else if (previousWallSide == WallSide::Left)
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1);
		else // previousWallSide == WallSide::Bottom
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1 + slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopLeft, Quad::Vertices::TopRight);


		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0], height, currentWallTile[1]);

		if (nextWallSide == WallSide::Bottom)
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + slopeTopOffset[0]);
		else if (nextWallSide == WallSide::Left)
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1]);
		else // nextWallSide == WallSide::Top
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] - slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::BottomLeft, Quad::Vertices::BottomRight);



		char tileType = GetTileValue(currentWallTile[0] - 1, currentWallTile[1]);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane->AddQuad();
			bottomPlane->AddQuad();


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, currentWallTile[0], height, currentWallTile[1] + 1, slopeIndex, height);

			if (previousWallSide == WallSide::Top)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1 - slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}
			else if (previousWallSide == WallSide::Left)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1, slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1, slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1, slopeIndex, slopeBottomOffset[1]);
			}
			else // previousWallSide == WallSide::Bottom
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1 + slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}

			bottomPlane->GetQuad(-1).AdjustV(Quad::Vertices::TopLeft, Quad::Vertices::TopRight);

			bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::TopLeft, Quad::Vertices::TopLeft);
			bottomPlane->GetQuad().AdjustV(Quad::Vertices::TopLeft, Quad::Vertices::TopRight);


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0], height, currentWallTile[1], slopeIndex + 1, height);

			if (nextWallSide == WallSide::Bottom)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}
			else if (nextWallSide == WallSide::Left)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1], slopeIndex + 1, slopeBottomOffset[1]);
			}
			else // nextWallSide == WallSide::Top
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] - slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}

			bottomPlane->GetQuad(-1).AdjustV(Quad::Vertices::BottomLeft, Quad::Vertices::BottomRight);

			bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::BottomLeft, Quad::Vertices::BottomLeft);
			bottomPlane->GetQuad().AdjustV(Quad::Vertices::BottomLeft, Quad::Vertices::BottomRight);
		}
	}
	else // currentWallSize == WallSide::Right
	{
		topPlane->AddQuad();


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1, height, currentWallTile[1] + 1);

		if (nextWallSide == WallSide::Top)
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + 1 - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1 - slopeTopOffset[0]);
		else if (nextWallSide == WallSide::Right)
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + 1 - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1);
		else // nextWallSide == WallSide::Bottom
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + 1 - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + 1 + slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopRight, Quad::Vertices::TopLeft);


		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1, height, currentWallTile[1]);

		if (previousWallSide == WallSide::Bottom)
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + 1 - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] + slopeTopOffset[0]);
		else if (previousWallSide == WallSide::Right)
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + 1 - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1]);
		else // previousWallSide == WallSide::Top
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + 1 - slopeTopOffset[0], slopeTopOffset[1], currentWallTile[1] - slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::BottomRight, Quad::Vertices::BottomLeft);



		char tileType = GetTileValue(currentWallTile[0] + 1, currentWallTile[1]);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane->AddQuad();
			bottomPlane->AddQuad();


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1, height, currentWallTile[1] + 1, slopeIndex + 1, height);

			if (nextWallSide == WallSide::Top)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 - slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + 1 - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1 - slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}
			else if (nextWallSide == WallSide::Right)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1, slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1, slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + 1 - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1, slopeIndex + 1, slopeBottomOffset[1]);
			}
			else // nextWallSide == WallSide::Bottom
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + 1 + slopeMiddleOffset[0], slopeIndex + 1, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, currentWallTile[0] + 1 - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + 1 + slopeBottomOffset[0], slopeIndex + 1, slopeBottomOffset[1]);
			}

			bottomPlane->GetQuad(-1).AdjustV(Quad::Vertices::TopRight, Quad::Vertices::TopLeft);

			bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::TopRight, Quad::Vertices::TopRight);
			bottomPlane->GetQuad().AdjustV(Quad::Vertices::TopRight, Quad::Vertices::TopLeft);


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1, height, currentWallTile[1], slopeIndex, height);

			if (previousWallSide == WallSide::Bottom)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] + slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + 1 - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] + slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}
			else if (previousWallSide == WallSide::Right)
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + 1 - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1], slopeIndex, slopeBottomOffset[1]);
			}
			else // previousWallSide == WallSide::Top
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, currentWallTile[0] + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], currentWallTile[1] - slopeMiddleOffset[0], slopeIndex, slopeMiddleOffset[1]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, currentWallTile[0] + 1 - slopeBottomOffset[0], slopeBottomOffset[1], currentWallTile[1] - slopeBottomOffset[0], slopeIndex, slopeBottomOffset[1]);
			}

			bottomPlane->GetQuad(-1).AdjustV(Quad::Vertices::BottomRight, Quad::Vertices::BottomLeft);

			bottomPlane->GetQuad().AdjustV(bottomPlane->GetQuad(-1), Quad::Vertices::BottomRight, Quad::Vertices::BottomRight);
			bottomPlane->GetQuad().AdjustV(Quad::Vertices::BottomRight, Quad::Vertices::BottomLeft);
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

	do {
		float rotationVector[2] = { 0, freeSpace };

		for (unsigned int rotationI = 0; rotationI < spaceCheckRotationIterationCount; rotationI++)
		{
			if (!topPlane->GetQuadStatus(column + rotationVector[0] + 1, row + rotationVector[1] + 1))
			{
				freeSpace += 1.f / spaceCheckRotationIterationCount;
			}

			rotationVector[0] = rotationCosine * rotationVector[0] - rotationSine * rotationVector[1];
			rotationVector[1] = rotationSine * rotationVector[0] + rotationCosine * rotationVector[1];
		}

	} while (freeSpace - (long)freeSpace < 0.00001 && freeSpace < 25);


	freeSpace -= (freeSpace - (long)freeSpace) / 1.5f;

	float centerOffsetVector[2] = { column + 0.5 - roomCenter[0], row + 0.5 - roomCenter[1] };
	float centerOffsetDistance = sqrt(centerOffsetVector[0] * centerOffsetVector[0] + centerOffsetVector[1] * centerOffsetVector[1]);

	freeSpace += centerOffsetDistance / 1.5f;


	return freeSpace;
}

void SE::Gameplay::Fog::AdjustOptimalCenter(float (&centerPosition)[2])
{
	if (centerPosition[0] > 25 || centerPosition[1] > 25 || topPlane->GetQuadStatus(centerPosition[0] + 1, centerPosition[1] + 1))
		return;


	float rotationSine = sin(6.283 / spaceCheckRotationIterationCount);
	float rotationCosine = cos(6.283 / spaceCheckRotationIterationCount);


	unsigned int freeSpace = 1;

	for (unsigned int moveI = 0; moveI < 10; moveI++)
	{
		unsigned int positionSum[2] = {};
		unsigned int positionCount = 0;

		do {
			float rotationVector[2] = { 0, freeSpace };

			for (unsigned int rotationI = 0; rotationI < spaceCheckRotationIterationCount; rotationI++)
			{
				if (topPlane->GetQuadStatus(centerPosition[0] + rotationVector[0] + 1, centerPosition[1] + rotationVector[1] + 1))
				{
					positionSum[0] += centerPosition[0] + rotationVector[0];
					positionSum[1] += centerPosition[1] + rotationVector[1];

					positionCount++;
				}

				rotationVector[0] = rotationCosine * rotationVector[0] - rotationSine * rotationVector[1];
				rotationVector[1] = rotationSine * rotationVector[0] + rotationCosine * rotationVector[1];
			}

			if (!positionCount)
				freeSpace++;

		} while (!positionCount);


		float centerOffsetVector[2] = { (float)positionSum[0] / positionCount - centerPosition[0], (float)positionSum[1] / positionCount - centerPosition[1] };
		float centerOffsetDistance = sqrt(centerOffsetVector[0] * centerOffsetVector[0] + centerOffsetVector[1] * centerOffsetVector[1]);

		float centerOffsetVector_normalized[2] = { centerOffsetVector[0] / centerOffsetDistance, centerOffsetVector[1] / centerOffsetDistance };

		centerPosition[0] += (1 - moveI / 10.f) * centerOffsetVector_normalized[0];
		centerPosition[1] += (1 - moveI / 10.f) * centerOffsetVector_normalized[1];
	}
}

char SE::Gameplay::Fog::GetTileValue(unsigned int column, unsigned int row)
{
	char tileValue = 0;

	if (column < 25 && row < 25)
		tileValue = tileValues[column][row];

	return tileValue;
}
