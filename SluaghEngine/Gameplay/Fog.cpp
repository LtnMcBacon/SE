#include "Fog.h"

#include "CoreInit.h"


using namespace SE;
using namespace Gameplay;


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
		topPlane.GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::TopLeft, paddingColumnI - 1, height, 0);
		topPlane.GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::TopRight, paddingColumnI, height, 0);
		topPlane.GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::BottomLeft, paddingColumnI - 1, height, -padding);
		topPlane.GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::BottomRight, paddingColumnI, height, -padding);

		AddAdjacentTiles(paddingColumnI - 1, 0);
	}

	for (int paddingColumnI = 1; paddingColumnI < 26; paddingColumnI++)
	{
		topPlane.GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::TopLeft, paddingColumnI - 1, height, 25 + padding);
		topPlane.GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::TopRight, paddingColumnI, height, 25 + padding);
		topPlane.GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::BottomLeft, paddingColumnI - 1, height, 25);
		topPlane.GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::BottomRight, paddingColumnI, height, 25);

		AddAdjacentTiles(paddingColumnI - 1, 24);
	}


	for (int paddingRowI = 1; paddingRowI < 26; paddingRowI++)
	{
		topPlane.GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::TopLeft, -padding, height, paddingRowI);
		topPlane.GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::TopRight, 0, height, paddingRowI);
		topPlane.GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::BottomLeft, -padding, height, paddingRowI - 1);
		topPlane.GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::BottomRight, 0, height, paddingRowI - 1);

		AddAdjacentTiles(0, paddingRowI - 1);
	}

	for (int paddingRowI = 1; paddingRowI < 26; paddingRowI++)
	{
		topPlane.GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::TopLeft, 25, height, paddingRowI);
		topPlane.GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::TopRight, 25 + padding, height, paddingRowI);
		topPlane.GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::BottomLeft, 25, height, paddingRowI - 1);
		topPlane.GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::BottomRight, 25 + padding, height, paddingRowI - 1);

		AddAdjacentTiles(24, paddingRowI - 1);
	}


	topPlane.GetQuad(0, 0).SetVertex(Quad::Vertices::TopLeft, -padding, height, 0);
	topPlane.GetQuad(0, 0).SetVertex(Quad::Vertices::TopRight, 0, height, 0);
	topPlane.GetQuad(0, 0).SetVertex(Quad::Vertices::BottomLeft, -padding, height, -padding);
	topPlane.GetQuad(0, 0).SetVertex(Quad::Vertices::BottomRight, 0, height, -padding);

	topPlane.GetQuad(26, 0).SetVertex(Quad::Vertices::TopLeft, 25, height, 0);
	topPlane.GetQuad(26, 0).SetVertex(Quad::Vertices::TopRight, 25 + padding, height, 0);
	topPlane.GetQuad(26, 0).SetVertex(Quad::Vertices::BottomLeft, 25, height, -padding);
	topPlane.GetQuad(26, 0).SetVertex(Quad::Vertices::BottomRight, 25 + padding, height, -padding);

	topPlane.GetQuad(0, 26).SetVertex(Quad::Vertices::TopLeft, -padding, height, 25 + padding);
	topPlane.GetQuad(0, 26).SetVertex(Quad::Vertices::TopRight, 0, height, 25 + padding);
	topPlane.GetQuad(0, 26).SetVertex(Quad::Vertices::BottomLeft, -padding, height, 25);
	topPlane.GetQuad(0, 26).SetVertex(Quad::Vertices::BottomRight, 0, height, 25);

	topPlane.GetQuad(26, 26).SetVertex(Quad::Vertices::TopLeft, 25, height, 25 + padding);
	topPlane.GetQuad(26, 26).SetVertex(Quad::Vertices::TopRight, 25 + padding, height, 25 + padding);
	topPlane.GetQuad(26, 26).SetVertex(Quad::Vertices::BottomLeft, 25, height, 25);
	topPlane.GetQuad(26, 26).SetVertex(Quad::Vertices::BottomRight, 25 + padding, height, 25);


	for (unsigned int rowI = 0; rowI < 25; rowI++)
	{
		for (unsigned int columnI = 0; columnI < 25; columnI++)
		{
			if (topPlane.GetQuadStatus(columnI + 1, rowI + 1))
				continue;

			if (topPlane.GetQuadStatus(columnI + 2, rowI + 1) || topPlane.GetQuadStatus(columnI, rowI + 1) || topPlane.GetQuadStatus(columnI + 1, rowI + 2) || topPlane.GetQuadStatus(columnI + 1, rowI))
				AddSlope(columnI, rowI);
		}
	}



	unsigned int topPlaneVertexCount = topPlane.GetQuadCount() * 2 * 3;

	Vertex *topPlaneVertexBuffer = new Vertex[topPlaneVertexCount];
	topPlane.GetVertexBuffer(topPlaneVertexBuffer);

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



	unsigned int bottomPlaneVertexCount = bottomPlane.GetQuadCount() * 2 * 3;

	Vertex *bottomPlaneVertexBuffer = new Vertex[bottomPlaneVertexCount];
	bottomPlane.GetVertexBuffer(bottomPlaneVertexBuffer);

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

	if (topPlane.GetQuadStatus(column + 1, row + 1))
		return;

	char tileType = tileValues[column][row];
	if (tileType != id_DeadArea && tileType != id_Wall && tileType != id_Door1 && tileType != id_Door2)
		return;


	topPlane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);
	topPlane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);
	topPlane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::BottomLeft, column, height, row);
	topPlane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);


	AddAdjacentTiles(column - 1, row);
	AddAdjacentTiles(column + 1, row);
	AddAdjacentTiles(column, row - 1);
	AddAdjacentTiles(column, row + 1);
}

void SE::Gameplay::Fog::AddSlope(unsigned int column, unsigned int row)
{
	if (topPlane.GetQuadStatus(column + 1, row + 2))
	{
		topPlane.AddQuad();


		topPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);

		if (topPlane.GetQuadStatus(column, row + 1))
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else if (topPlane.GetQuadStatus(column, row + 2))
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column - slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);


		topPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);

		if (topPlane.GetQuadStatus(column + 2, row + 1))
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else if (topPlane.GetQuadStatus(column + 2, row + 2))
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);



		char tileType = GetTileValue(column, row + 1);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane.AddQuad();
			bottomPlane.AddQuad();


			bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);

			if (topPlane.GetQuadStatus(column, row + 1))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else if (topPlane.GetQuadStatus(column, row + 2))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column, slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column - slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}


			bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);

			if (topPlane.GetQuadStatus(column + 2, row + 1))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else if (topPlane.GetQuadStatus(column + 2, row + 2))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1, slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
		}
	}

	if (topPlane.GetQuadStatus(column + 1, row))
	{
		topPlane.AddQuad();


		topPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, height, row);

		if (topPlane.GetQuadStatus(column, row + 1))
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);
		else if (topPlane.GetQuadStatus(column, row))
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, slopeTopOffset[1], row + slopeTopOffset[0]);
		else
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column - slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);


		topPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);

		if (topPlane.GetQuadStatus(column + 2, row + 1))
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);
		else if (topPlane.GetQuadStatus(column + 2, row))
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, slopeTopOffset[1], row + slopeTopOffset[0]);
		else
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 + slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);



		char tileType = GetTileValue(column, row - 1);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane.AddQuad();
			bottomPlane.AddQuad();


			bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column, height, row);

			if (topPlane.GetQuadStatus(column, row + 1))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else if (topPlane.GetQuadStatus(column, row))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column, slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column - slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}


			bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);

			if (topPlane.GetQuadStatus(column + 2, row + 1))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else if (topPlane.GetQuadStatus(column + 2, row))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1, slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 + slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
		}
	}

	if (topPlane.GetQuadStatus(column, row + 1))
	{
		topPlane.AddQuad();


		topPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);

		if (topPlane.GetQuadStatus(column + 1, row + 2))
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else if (topPlane.GetQuadStatus(column, row + 2))
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeTopOffset[0], slopeTopOffset[1], row + 1);
		else
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeTopOffset[0], slopeTopOffset[1], row + 1 + slopeTopOffset[0]);


		topPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, height, row);

		if (topPlane.GetQuadStatus(column + 1, row))
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);
		else if (topPlane.GetQuadStatus(column, row))
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeTopOffset[0], slopeTopOffset[1], row);
		else
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeTopOffset[0], slopeTopOffset[1], row - slopeTopOffset[0]);



		char tileType = GetTileValue(column - 1, row);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane.AddQuad();
			bottomPlane.AddQuad();


			bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column, height, row + 1);

			if (topPlane.GetQuadStatus(column + 1, row + 2))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else if (topPlane.GetQuadStatus(column, row + 2))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row + 1);
			}
			else
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row + 1 + slopeBottomOffset[0]);
			}


			bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column, height, row);

			if (topPlane.GetQuadStatus(column + 1, row))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else if (topPlane.GetQuadStatus(column, row))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row);
			}
			else
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeMiddleOffset[0], slopeMiddleOffset[1], row - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeBottomOffset[0], slopeBottomOffset[1], row - slopeBottomOffset[0]);
			}
		}
	}

	if (topPlane.GetQuadStatus(column + 2, row + 1))
	{
		topPlane.AddQuad();


		topPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);

		if (topPlane.GetQuadStatus(column + 1, row + 2))
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + 1 - slopeTopOffset[0]);
		else if (topPlane.GetQuadStatus(column + 2, row + 2))
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + 1);
		else
			topPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + 1 + slopeTopOffset[0]);


		topPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);

		if (topPlane.GetQuadStatus(column + 1, row))
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row + slopeTopOffset[0]);
		else if (topPlane.GetQuadStatus(column + 2, row))
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row);
		else
			topPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeTopOffset[0], slopeTopOffset[1], row - slopeTopOffset[0]);



		char tileType = GetTileValue(column + 1, row);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			bottomPlane.AddQuad();
			bottomPlane.AddQuad();


			bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1);

			if (topPlane.GetQuadStatus(column + 1, row + 2))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + 1 - slopeBottomOffset[0]);
			}
			else if (topPlane.GetQuadStatus(column + 2, row + 2))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + 1);
			}
			else
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + 1 + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + 1 + slopeBottomOffset[0]);
			}


			bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomRight, column + 1, height, row);

			if (topPlane.GetQuadStatus(column + 1, row))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row + slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row + slopeBottomOffset[0]);
			}
			else if (topPlane.GetQuadStatus(column + 2, row))
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row);
			}
			else
			{
				bottomPlane.GetQuad(-1).SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row - slopeMiddleOffset[0]);

				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeMiddleOffset[0], slopeMiddleOffset[1], row - slopeMiddleOffset[0]);
				bottomPlane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeBottomOffset[0], slopeBottomOffset[1], row - slopeBottomOffset[0]);
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
