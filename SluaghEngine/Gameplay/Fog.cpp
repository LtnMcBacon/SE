#include "Fog.h"

#include "CoreInit.h"
#include <memory.h>
#include <vector>
#include <algorithm>


#undef min
#undef max


using namespace SE;
using namespace Gameplay;


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
	void AdjustUv(Vertices parent, Vertices child)
	{
		float parentU, parentV;

		if (parent == Vertices::TopLeft)
		{
			parentU = vertices[0].uv[0];
			parentV = vertices[0].uv[1];
		}
		else if (parent == Vertices::TopRight)
		{
			parentU = vertices[1].uv[0];
			parentV = vertices[1].uv[1];
		}
		else if (parent == Vertices::BottomLeft)
		{
			parentU = vertices[5].uv[0];
			parentV = vertices[5].uv[1];
		}
		else if (parent == Vertices::BottomRight)
		{
			parentU = vertices[2].uv[0];
			parentV = vertices[2].uv[1];
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
			uVector[0] -= vertices[0].position[0];
			uVector[1] -= vertices[0].position[1];
		}
		else if (parent == Vertices::TopRight)
		{
			uVector[0] -= vertices[1].position[0];
			uVector[1] -= vertices[1].position[1];
		}
		else if (parent == Vertices::BottomLeft)
		{
			uVector[0] -= vertices[5].position[0];
			uVector[1] -= vertices[5].position[1];
		}
		else if (parent == Vertices::BottomRight)
		{
			uVector[0] -= vertices[2].position[0];
			uVector[1] -= vertices[2].position[1];
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
			vVector[0] -= vertices[0].position[2];
			vVector[1] -= vertices[0].position[1];
		}
		else if (parent == Vertices::TopRight)
		{
			vVector[0] -= vertices[1].position[2];
			vVector[1] -= vertices[1].position[1];
		}
		else if (parent == Vertices::BottomLeft)
		{
			vVector[0] -= vertices[5].position[2];
			vVector[1] -= vertices[5].position[1];
		}
		else if (parent == Vertices::BottomRight)
		{
			vVector[0] -= vertices[2].position[2];
			vVector[1] -= vertices[2].position[1];
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

void SE::Gameplay::Fog::Set(bool status)
{
	if (status && topPlaneRjHandle == -1)
	{
		CreatePlane();
	}
	else if (!status && topPlaneRjHandle != -1)
	{
		CoreInit::subSystems.renderer->GetPipelineHandler()->DestroyVertexBuffer("FogTopPlaneVb");
		CoreInit::subSystems.renderer->RemoveRenderJob(topPlaneRjHandle);

		topPlaneRjHandle = -1;


		CoreInit::subSystems.renderer->GetPipelineHandler()->DestroyVertexBuffer("FogBottomPlaneVb");
		CoreInit::subSystems.renderer->RemoveRenderJob(bottomPlaneRjHandle);

		bottomPlaneRjHandle = -1;
	}
}

void SE::Gameplay::Fog::SetTileValues(char tileValues[25][25])
{
	for (unsigned int row = 0; row < 25; row++)
	{
		for (unsigned int column = 0; column < 25; column++)
		{
			this->tileValues[column][row] = tileValues[column][row];
		}
	}
}

void SE::Gameplay::Fog::CreatePlane()
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


	for (unsigned int rowI = 0; rowI < 25; rowI++)
	{
		for (unsigned int columnI = 0; columnI < 25; columnI++)
		{
			if (topPlane->GetQuadStatus(columnI + 1, rowI + 1))
				continue;

			if (topPlane->GetQuadStatus(columnI + 2, rowI + 1) || topPlane->GetQuadStatus(columnI, rowI + 1) || topPlane->GetQuadStatus(columnI + 1, rowI + 2) || topPlane->GetQuadStatus(columnI + 1, rowI))
				AddSlope(columnI, rowI);
		}
	}



	unsigned int topPlaneVertexCount = topPlane->GetQuadCount() * 2 * 3;

	Vertex *topPlaneVertexBuffer = new Vertex[topPlaneVertexCount];
	topPlane->GetVertexBuffer(topPlaneVertexBuffer);

	CoreInit::subSystems.renderer->GetPipelineHandler()->CreateVertexBuffer("FogTopPlaneVb", topPlaneVertexBuffer, topPlaneVertexCount, sizeof(Vertex));


	Graphics::RenderJob topPlaneRj;

	topPlaneRj.pipeline.OMStage.renderTargets[0] = "backbuffer";
	topPlaneRj.pipeline.OMStage.renderTargetCount = 1;

	topPlaneRj.pipeline.VSStage.shader = "FogVS.hlsl";
	topPlaneRj.pipeline.PSStage.shader = "FogPS.hlsl";

	topPlaneRj.pipeline.PSStage.samplers[0] = "FogSampler";
	topPlaneRj.pipeline.PSStage.samplerCount = 1;

	topPlaneRj.pipeline.PSStage.textures[0] = "Fog_AlbedoTexture.png";
	topPlaneRj.pipeline.PSStage.textures[1] = "Fog_NormalTexture.png";
	topPlaneRj.pipeline.PSStage.textureCount = 2;
	topPlaneRj.pipeline.PSStage.textureBindings[0] = "albedoTexture";
	topPlaneRj.pipeline.PSStage.textureBindings[1] = "normalTexture";

	topPlaneRj.pipeline.IAStage.inputLayout = "FogVS.hlsl";
	topPlaneRj.pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;
	topPlaneRj.pipeline.IAStage.vertexBuffer = "FogTopPlaneVb";

	topPlaneRj.pipeline.OMStage.blendState = "FogBs";
	topPlaneRj.pipeline.OMStage.depthStencilState = "backbuffer";
	topPlaneRj.pipeline.OMStage.depthStencilView = "backbuffer";

	topPlaneRj.vertexCount = topPlaneVertexCount;
	topPlaneRj.maxInstances = 1;

	topPlaneRjHandle = CoreInit::subSystems.renderer->AddRenderJob(topPlaneRj, Graphics::RenderGroup::RENDER_PASS_5);



	unsigned int bottomPlaneVertexCount = bottomPlane->GetQuadCount() * 2 * 3;

	Vertex *bottomPlaneVertexBuffer = new Vertex[bottomPlaneVertexCount];
	bottomPlane->GetVertexBuffer(bottomPlaneVertexBuffer);

	CoreInit::subSystems.renderer->GetPipelineHandler()->CreateVertexBuffer("FogBottomPlaneVb", bottomPlaneVertexBuffer, bottomPlaneVertexCount, sizeof(Vertex));


	Graphics::RenderJob bottomPlaneRj;

	bottomPlaneRj.pipeline.OMStage.renderTargets[0] = "backbuffer";
	bottomPlaneRj.pipeline.OMStage.renderTargetCount = 1;

	bottomPlaneRj.pipeline.VSStage.shader = "FogVS.hlsl";
	bottomPlaneRj.pipeline.PSStage.shader = "FogPS.hlsl";

	bottomPlaneRj.pipeline.PSStage.samplers[0] = "FogSampler";
	bottomPlaneRj.pipeline.PSStage.samplerCount = 1;

	bottomPlaneRj.pipeline.PSStage.textures[0] = "Fog_AlbedoTexture.png";
	bottomPlaneRj.pipeline.PSStage.textures[1] = "Fog_NormalTexture.png";
	bottomPlaneRj.pipeline.PSStage.textureCount = 2;
	bottomPlaneRj.pipeline.PSStage.textureBindings[0] = "albedoTexture";
	bottomPlaneRj.pipeline.PSStage.textureBindings[1] = "normalTexture";

	bottomPlaneRj.pipeline.IAStage.inputLayout = "FogVS.hlsl";
	bottomPlaneRj.pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;
	bottomPlaneRj.pipeline.IAStage.vertexBuffer = "FogBottomPlaneVb";

	bottomPlaneRj.pipeline.OMStage.blendState = "FogBs";
	bottomPlaneRj.pipeline.OMStage.depthStencilState = "backbuffer";
	bottomPlaneRj.pipeline.OMStage.depthStencilView = "backbuffer";

	bottomPlaneRj.vertexCount = bottomPlaneVertexCount;
	bottomPlaneRj.maxInstances = 1;

	bottomPlaneRjHandle = CoreInit::subSystems.renderer->AddRenderJob(bottomPlaneRj, Graphics::RenderGroup::RENDER_PASS_4);
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

void SE::Gameplay::Fog::AddSlope(unsigned int column, unsigned int row)
{
	if (topPlane->GetQuadStatus(column + 1, row + 2))
	{
		topPlane->AddQuad();


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);

		if (topPlane->GetQuadStatus(column, row + 1))
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else if (topPlane->GetQuadStatus(column, row + 2))
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column - slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopLeft, Quad::Vertices::BottomLeft);


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);

		if (topPlane->GetQuadStatus(column + 2, row + 1))
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else if (topPlane->GetQuadStatus(column + 2, row + 2))
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopRight, Quad::Vertices::BottomRight);



		char tileType = GetTileValue(column, row + 1);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane->AddQuad();
			bottomPlane->AddQuad();


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);

			if (topPlane->GetQuadStatus(column, row + 1))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else if (topPlane->GetQuadStatus(column, row + 2))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column, slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column, slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column - slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);

			if (topPlane->GetQuadStatus(column + 2, row + 1))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else if (topPlane->GetQuadStatus(column + 2, row + 2))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1, slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
		}
	}

	if (topPlane->GetQuadStatus(column + 1, row))
	{
		topPlane->AddQuad();


		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, height, row);

		if (topPlane->GetQuadStatus(column, row + 1))
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);
		else if (topPlane->GetQuadStatus(column, row))
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column, slopeTopOffset[1], row + slopeTopOffset[0]);
		else
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column - slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::BottomLeft, Quad::Vertices::TopLeft);


		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);

		if (topPlane->GetQuadStatus(column + 2, row + 1))
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);
		else if (topPlane->GetQuadStatus(column + 2, row))
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, slopeTopOffset[1], row + slopeTopOffset[0]);
		else
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 + slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::BottomRight, Quad::Vertices::TopRight);



		char tileType = GetTileValue(column, row - 1);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane->AddQuad();
			bottomPlane->AddQuad();


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column, height, row);

			if (topPlane->GetQuadStatus(column, row + 1))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else if (topPlane->GetQuadStatus(column, row))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column, slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column, slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column - slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);

			if (topPlane->GetQuadStatus(column + 2, row + 1))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else if (topPlane->GetQuadStatus(column + 2, row))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1, slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 + slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
		}
	}

	if (topPlane->GetQuadStatus(column, row + 1))
	{
		topPlane->AddQuad();


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);

		if (topPlane->GetQuadStatus(column + 1, row + 2))
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else if (topPlane->GetQuadStatus(column, row + 2))
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeTopOffset[0], slopeTopOffset[1], row + 1);
		else
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeTopOffset[0], slopeTopOffset[1], row + 1 + slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopLeft, Quad::Vertices::TopRight);


		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, height, row);

		if (topPlane->GetQuadStatus(column + 1, row))
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);
		else if (topPlane->GetQuadStatus(column, row))
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeTopOffset[0], slopeTopOffset[1], row);
		else
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeTopOffset[0], slopeTopOffset[1], row - slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::BottomLeft, Quad::Vertices::BottomRight);



		char tileType = GetTileValue(column - 1, row);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane->AddQuad();
			bottomPlane->AddQuad();


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);

			if (topPlane->GetQuadStatus(column + 1, row + 2))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else if (topPlane->GetQuadStatus(column, row + 2))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row + 1);
			}
			else
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row + 1 + slopeBottomOffset[0]);
			}


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column, height, row);

			if (topPlane->GetQuadStatus(column + 1, row))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else if (topPlane->GetQuadStatus(column, row))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row);
			}
			else
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row - slopeBottomOffset[0]);
			}
		}
	}

	if (topPlane->GetQuadStatus(column + 2, row + 1))
	{
		topPlane->AddQuad();


		topPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);

		if (topPlane->GetQuadStatus(column + 1, row + 2))
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else if (topPlane->GetQuadStatus(column + 2, row + 2))
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + 1);
		else
			topPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + 1 + slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::TopRight, Quad::Vertices::TopLeft);


		topPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);

		if (topPlane->GetQuadStatus(column + 1, row))
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);
		else if (topPlane->GetQuadStatus(column + 2, row))
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row);
		else
			topPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row - slopeTopOffset[0]);

		topPlane->GetQuad().AdjustUv(Quad::Vertices::BottomRight, Quad::Vertices::BottomLeft);



		char tileType = GetTileValue(column + 1, row);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane->AddQuad();
			bottomPlane->AddQuad();


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);

			if (topPlane->GetQuadStatus(column + 1, row + 2))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else if (topPlane->GetQuadStatus(column + 2, row + 2))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + 1);
			}
			else
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + 1 + slopeBottomOffset[0]);
			}


			bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);

			if (topPlane->GetQuadStatus(column + 1, row))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else if (topPlane->GetQuadStatus(column + 2, row))
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row);
			}
			else
			{
				bottomPlane->GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row - slopeMiddleOffset[0]);

				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row - slopeMiddleOffset[0]);
				bottomPlane->GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row - slopeBottomOffset[0]);
			}
		}
	}
}

char SE::Gameplay::Fog::GetTileValue(unsigned int column, unsigned int row)
{
	char tileValue = 0;

	if (column < 25 && row < 25)
		tileValue = tileValues[column][row];

	return tileValue;
}
