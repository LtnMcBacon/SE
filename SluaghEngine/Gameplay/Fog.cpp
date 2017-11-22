#include "Fog.h"

#include "CoreInit.h"


using namespace SE;
using namespace Gameplay;


void SE::Gameplay::Fog::Set(bool status)
{
	if (status && rjHandle == -1)
	{
		CreatePlane();
	}
	else if (!status && rjHandle != -1)
	{
		CoreInit::subSystems.renderer->GetPipelineHandler()->DestroyVertexBuffer("Fog");
		CoreInit::subSystems.renderer->RemoveRenderJob(rjHandle);

		rjHandle = -1;
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
		plane.GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::TopLeft, paddingColumnI - 1, height, 0, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::TopRight, paddingColumnI, height, 0, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::BottomLeft, paddingColumnI - 1, height, -padding, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(paddingColumnI, 0).SetVertex(Quad::Vertices::BottomRight, paddingColumnI, height, -padding, TEMP_uvCoord, TEMP_uvCoord);

		AddAdjacentTiles(paddingColumnI - 1, 0);
	}

	for (int paddingColumnI = 1; paddingColumnI < 26; paddingColumnI++)
	{
		plane.GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::TopLeft, paddingColumnI - 1, height, 25 + padding, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::TopRight, paddingColumnI, height, 25 + padding, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::BottomLeft, paddingColumnI - 1, height, 25, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(paddingColumnI, 26).SetVertex(Quad::Vertices::BottomRight, paddingColumnI, height, 25, TEMP_uvCoord, TEMP_uvCoord);

		AddAdjacentTiles(paddingColumnI - 1, 24);
	}


	for (int paddingRowI = 1; paddingRowI < 26; paddingRowI++)
	{
		plane.GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::TopLeft, -padding, height, paddingRowI, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::TopRight, 0, height, paddingRowI, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::BottomLeft, -padding, height, paddingRowI - 1, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(0, paddingRowI).SetVertex(Quad::Vertices::BottomRight, 0, height, paddingRowI - 1, TEMP_uvCoord, TEMP_uvCoord);

		AddAdjacentTiles(0, paddingRowI - 1);
	}

	for (int paddingRowI = 1; paddingRowI < 26; paddingRowI++)
	{
		plane.GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::TopLeft, 25, height, paddingRowI, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::TopRight, 25 + padding, height, paddingRowI, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::BottomLeft, 25, height, paddingRowI - 1, TEMP_uvCoord, TEMP_uvCoord);
		plane.GetQuad(26, paddingRowI).SetVertex(Quad::Vertices::BottomRight, 25 + padding, height, paddingRowI - 1, TEMP_uvCoord, TEMP_uvCoord);

		AddAdjacentTiles(24, paddingRowI - 1);
	}


	plane.GetQuad(0, 0).SetVertex(Quad::Vertices::TopLeft, -padding, height, 0, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(0, 0).SetVertex(Quad::Vertices::TopRight, 0, height, 0, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(0, 0).SetVertex(Quad::Vertices::BottomLeft, -padding, height, -padding, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(0, 0).SetVertex(Quad::Vertices::BottomRight, 0, height, -padding, TEMP_uvCoord, TEMP_uvCoord);

	plane.GetQuad(26, 0).SetVertex(Quad::Vertices::TopLeft, 25, height, 0, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(26, 0).SetVertex(Quad::Vertices::TopRight, 25 + padding, height, 0, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(26, 0).SetVertex(Quad::Vertices::BottomLeft, 25, height, -padding, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(26, 0).SetVertex(Quad::Vertices::BottomRight, 25 + padding, height, -padding, TEMP_uvCoord, TEMP_uvCoord);

	plane.GetQuad(0, 26).SetVertex(Quad::Vertices::TopLeft, -padding, height, 25 + padding, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(0, 26).SetVertex(Quad::Vertices::TopRight, 0, height, 25 + padding, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(0, 26).SetVertex(Quad::Vertices::BottomLeft, -padding, height, 25, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(0, 26).SetVertex(Quad::Vertices::BottomRight, 0, height, 25, TEMP_uvCoord, TEMP_uvCoord);

	plane.GetQuad(26, 26).SetVertex(Quad::Vertices::TopLeft, 25, height, 25 + padding, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(26, 26).SetVertex(Quad::Vertices::TopRight, 25 + padding, height, 25 + padding, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(26, 26).SetVertex(Quad::Vertices::BottomLeft, 25, height, 25, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(26, 26).SetVertex(Quad::Vertices::BottomRight, 25 + padding, height, 25, TEMP_uvCoord, TEMP_uvCoord);


	for (unsigned int rowI = 0; rowI < 25; rowI++)
	{
		for (unsigned int columnI = 0; columnI < 25; columnI++)
		{
			if (plane.GetQuadStatus(columnI + 1, rowI + 1))
				continue;

			if (plane.GetQuadStatus(columnI + 2, rowI + 1) || plane.GetQuadStatus(columnI, rowI + 1) || plane.GetQuadStatus(columnI + 1, rowI + 2) || plane.GetQuadStatus(columnI + 1, rowI))
				AddSlope(columnI, rowI);
		}
	}



	unsigned int planeVertexCount = plane.GetQuadCount() * 2 * 3;

	Vertex *planeVertexBuffer = new Vertex[planeVertexCount];
	plane.GetVertexBuffer(planeVertexBuffer);

	CoreInit::subSystems.renderer->GetPipelineHandler()->CreateVertexBuffer("Fog", planeVertexBuffer, planeVertexCount, sizeof(Vertex));


	Graphics::RenderJob fogRj;

	fogRj.pipeline.OMStage.renderTargets[0] = "backbuffer";
	fogRj.pipeline.OMStage.renderTargetCount = 1;

	fogRj.pipeline.VSStage.shader = "FogVS.hlsl";
	fogRj.pipeline.PSStage.shader = "FogPS.hlsl";

	fogRj.pipeline.PSStage.samplers[0] = "FogSampler";
	fogRj.pipeline.PSStage.samplerCount = 1;

	fogRj.pipeline.PSStage.textures[0] = "Fog_AlbedoTexture.png";
	fogRj.pipeline.PSStage.textures[1] = "Fog_NormalTexture.png";
	fogRj.pipeline.PSStage.textureCount = 2;
	fogRj.pipeline.PSStage.textureBindings[0] = "albedoTexture";
	fogRj.pipeline.PSStage.textureBindings[1] = "normalTexture";

	fogRj.pipeline.IAStage.inputLayout = "FogVS.hlsl";
	fogRj.pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;
	fogRj.pipeline.IAStage.vertexBuffer = "Fog";

	fogRj.vertexCount = planeVertexCount;
	fogRj.maxInstances = 1;

	rjHandle = CoreInit::subSystems.renderer->AddRenderJob(fogRj, Graphics::RenderGroup::POST_PASS_5);
}

void SE::Gameplay::Fog::AddAdjacentTiles(unsigned int column, unsigned int row)
{
	if (column > 24 || row > 24)
		return;

	if (plane.GetQuadStatus(column + 1, row + 1))
		return;

	char tileType = tileValues[column][row];
	if (tileType != id_DeadArea && tileType != id_Wall && tileType != id_Door1 && tileType != id_Door2)
		return;


	plane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::TopLeft, column, height, row + 1, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::BottomLeft, column, height, row, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::BottomRight, column + 1, height, row, TEMP_uvCoord, TEMP_uvCoord);


	AddAdjacentTiles(column - 1, row);
	AddAdjacentTiles(column + 1, row);
	AddAdjacentTiles(column, row - 1);
	AddAdjacentTiles(column, row + 1);
}

void SE::Gameplay::Fog::AddSlope(unsigned int column, unsigned int row)
{
	if (plane.GetQuadStatus(column + 1, row + 2))
	{
		plane.AddQuad();


		plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, height, row + 1, TEMP_uvCoord, TEMP_uvCoord);

		if (plane.GetQuadStatus(column, row + 1))
			plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else if (plane.GetQuadStatus(column, row + 2))
			plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else
			plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column - slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);


		plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1, TEMP_uvCoord, TEMP_uvCoord);

		if (plane.GetQuadStatus(column + 2, row + 1))
			plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else if (plane.GetQuadStatus(column + 2, row + 2))
			plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else
			plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);



		char tileType = GetTileValue(column, row + 1);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			plane.AddQuad();


			if (plane.GetQuadStatus(column, row + 1))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeOffset, slopeBottom, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else if (plane.GetQuadStatus(column, row + 2))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, slopeBottom, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column - slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column - slopeOffset, slopeBottom, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}


			if (plane.GetQuadStatus(column + 2, row + 1))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeOffset, slopeBottom, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else if (plane.GetQuadStatus(column + 2, row + 2))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, slopeBottom, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 + slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeOffset, slopeBottom, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
		}
	}

	if (plane.GetQuadStatus(column + 1, row))
	{
		plane.AddQuad();


		plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, height, row, TEMP_uvCoord, TEMP_uvCoord);

		if (plane.GetQuadStatus(column, row + 1))
			plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else if (plane.GetQuadStatus(column, row))
			plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else
			plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column - slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);


		plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, height, row, TEMP_uvCoord, TEMP_uvCoord);

		if (plane.GetQuadStatus(column + 2, row + 1))
			plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else if (plane.GetQuadStatus(column + 2, row))
			plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else
			plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 + slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);



		char tileType = GetTileValue(column, row - 1);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			plane.AddQuad();


			if (plane.GetQuadStatus(column, row + 1))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeOffset, slopeBottom, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else if (plane.GetQuadStatus(column, row))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, slopeBottom, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column - slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column - slopeOffset, slopeBottom, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}


			if (plane.GetQuadStatus(column + 2, row + 1))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeOffset, slopeBottom, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else if (plane.GetQuadStatus(column + 2, row))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, slopeBottom, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 + slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 + slopeOffset, slopeBottom, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
		}
	}

	if (plane.GetQuadStatus(column, row + 1))
	{
		plane.AddQuad();


		plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column, height, row + 1, TEMP_uvCoord, TEMP_uvCoord);

		if (plane.GetQuadStatus(column + 1, row + 2))
			plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else if (plane.GetQuadStatus(column, row + 2))
			plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeOffset, slopeTop, row + 1, TEMP_uvCoord, TEMP_uvCoord);
		else
			plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeOffset, slopeTop, row + 1 + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);


		plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column, height, row, TEMP_uvCoord, TEMP_uvCoord);

		if (plane.GetQuadStatus(column + 1, row))
			plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else if (plane.GetQuadStatus(column, row))
			plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeOffset, slopeTop, row, TEMP_uvCoord, TEMP_uvCoord);
		else
			plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeOffset, slopeTop, row - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);



		char tileType = GetTileValue(column - 1, row);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			plane.AddQuad();


			if (plane.GetQuadStatus(column + 1, row + 2))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeOffset, slopeBottom, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else if (plane.GetQuadStatus(column, row + 2))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeOffset, slopeTop, row + 1, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeOffset, slopeBottom, row + 1, TEMP_uvCoord, TEMP_uvCoord);
			}
			else
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + slopeOffset, slopeTop, row + 1 + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + slopeOffset, slopeBottom, row + 1 + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}


			if (plane.GetQuadStatus(column + 1, row))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeOffset, slopeBottom, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else if (plane.GetQuadStatus(column, row))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeOffset, slopeTop, row, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeOffset, slopeBottom, row, TEMP_uvCoord, TEMP_uvCoord);
			}
			else
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + slopeOffset, slopeTop, row - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + slopeOffset, slopeBottom, row - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
		}
	}

	if (plane.GetQuadStatus(column + 2, row + 1))
	{
		plane.AddQuad();


		plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1, height, row + 1, TEMP_uvCoord, TEMP_uvCoord);

		if (plane.GetQuadStatus(column + 1, row + 2))
			plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else if (plane.GetQuadStatus(column + 2, row + 2))
			plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeOffset, slopeTop, row + 1, TEMP_uvCoord, TEMP_uvCoord);
		else
			plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeOffset, slopeTop, row + 1 + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);


		plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1, height, row, TEMP_uvCoord, TEMP_uvCoord);

		if (plane.GetQuadStatus(column + 1, row))
			plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
		else if (plane.GetQuadStatus(column + 2, row))
			plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeOffset, slopeTop, row, TEMP_uvCoord, TEMP_uvCoord);
		else
			plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeOffset, slopeTop, row - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);



		char tileType = GetTileValue(column + 1, row);
		if (tileType != id_Door1 && tileType != id_Door2)
		{
			plane.AddQuad();


			if (plane.GetQuadStatus(column + 1, row + 2))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeOffset, slopeTop, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeOffset, slopeBottom, row + 1 - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else if (plane.GetQuadStatus(column + 2, row + 2))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeOffset, slopeTop, row + 1, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeOffset, slopeBottom, row + 1, TEMP_uvCoord, TEMP_uvCoord);
			}
			else
			{
				plane.GetQuad().SetVertex(Quad::Vertices::TopRight, column + 1 - slopeOffset, slopeTop, row + 1 + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::TopLeft, column + 1 - slopeOffset, slopeBottom, row + 1 + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}


			if (plane.GetQuadStatus(column + 1, row))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeOffset, slopeTop, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeOffset, slopeBottom, row + slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
			}
			else if (plane.GetQuadStatus(column + 2, row))
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeOffset, slopeTop, row, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeOffset, slopeBottom, row, TEMP_uvCoord, TEMP_uvCoord);
			}
			else
			{
				plane.GetQuad().SetVertex(Quad::Vertices::BottomRight, column + 1 - slopeOffset, slopeTop, row - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
				plane.GetQuad().SetVertex(Quad::Vertices::BottomLeft, column + 1 - slopeOffset, slopeBottom, row - slopeOffset, TEMP_uvCoord, TEMP_uvCoord);
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
