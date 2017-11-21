#include "Fog.h"

#include "CoreInit.h"


using namespace SE;
using namespace Gameplay;


void SE::Gameplay::Fog::Set(bool status)
{
	if (status && fogRjHandle == -1)
	{
		CreateFogPlane();
	}
	else if (!status && fogRjHandle != -1)
	{
		CoreInit::subSystems.renderer->GetPipelineHandler()->DestroyVertexBuffer("Fog");
		CoreInit::subSystems.renderer->RemoveRenderJob(fogRjHandle);

		fogRjHandle = -1;
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

void SE::Gameplay::Fog::CreateFogPlane()
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



	for (int paddingRowI = -1; paddingRowI < 26; paddingRowI += 26)
	{
		for (int paddingColumnI = -1; paddingColumnI < 26; paddingColumnI++)
		{
			plane.GetQuad(paddingColumnI + 1, paddingRowI + 1).SetVertex(Quad::Vertices::TopLeft, paddingColumnI, fogHeight, paddingRowI + 1, TEMP_uvCoord, TEMP_uvCoord);
			plane.GetQuad(paddingColumnI + 1, paddingRowI + 1).SetVertex(Quad::Vertices::TopRight, paddingColumnI + 1, fogHeight, paddingRowI + 1, TEMP_uvCoord, TEMP_uvCoord);
			plane.GetQuad(paddingColumnI + 1, paddingRowI + 1).SetVertex(Quad::Vertices::BottomLeft, paddingColumnI, fogHeight, paddingRowI, TEMP_uvCoord, TEMP_uvCoord);
			plane.GetQuad(paddingColumnI + 1, paddingRowI + 1).SetVertex(Quad::Vertices::BottomRight, paddingColumnI + 1, fogHeight, paddingRowI, TEMP_uvCoord, TEMP_uvCoord);

			if (paddingRowI == -1)
				CheckAdjacentTiles(paddingColumnI, 0);
			else
				CheckAdjacentTiles(paddingColumnI, 24);
		}
	}

	for (int paddingRowI = 0; paddingRowI < 25; paddingRowI++)
	{
		for (int paddingColumnI = -1; paddingColumnI < 26; paddingColumnI += 26)
		{
			plane.GetQuad(paddingColumnI + 1, paddingRowI + 1).SetVertex(Quad::Vertices::TopLeft, paddingColumnI, fogHeight, paddingRowI + 1, TEMP_uvCoord, TEMP_uvCoord);
			plane.GetQuad(paddingColumnI + 1, paddingRowI + 1).SetVertex(Quad::Vertices::TopRight, paddingColumnI + 1, fogHeight, paddingRowI + 1, TEMP_uvCoord, TEMP_uvCoord);
			plane.GetQuad(paddingColumnI + 1, paddingRowI + 1).SetVertex(Quad::Vertices::BottomLeft, paddingColumnI, fogHeight, paddingRowI, TEMP_uvCoord, TEMP_uvCoord);
			plane.GetQuad(paddingColumnI + 1, paddingRowI + 1).SetVertex(Quad::Vertices::BottomRight, paddingColumnI + 1, fogHeight, paddingRowI, TEMP_uvCoord, TEMP_uvCoord);

			if (paddingColumnI == -1)
				CheckAdjacentTiles(0, paddingRowI);
			else
				CheckAdjacentTiles(24, paddingRowI);
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

	fogRj.pipeline.PSStage.textures[0] = "DefaultNormal.jpg";
	fogRj.pipeline.PSStage.textureCount = 1;
	fogRj.pipeline.PSStage.textureBindings[0] = "fogTexture";

	fogRj.pipeline.IAStage.inputLayout = "FogVS.hlsl";
	fogRj.pipeline.IAStage.topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;
	fogRj.pipeline.IAStage.vertexBuffer = "Fog";

	fogRj.vertexCount = planeVertexCount;
	fogRj.maxInstances = 1;

	fogRjHandle = CoreInit::subSystems.renderer->AddRenderJob(fogRj, Graphics::RenderGroup::POST_PASS_5);
}

void SE::Gameplay::Fog::CheckAdjacentTiles(unsigned int column, unsigned int row)
{
	if (column > 24 || row > 24)
		return;

	if (plane.GetQuadStatus(column + 1, row + 1))
		return;

	char tileType = tileValues[column][row];
	if (tileType != id_DeadArea && tileType != id_Wall && tileType != id_Door1 && tileType != id_Door2)
		return;


	plane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::TopLeft, column, fogHeight, row + 1, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::TopRight, column + 1, fogHeight, row + 1, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::BottomLeft, column, fogHeight, row, TEMP_uvCoord, TEMP_uvCoord);
	plane.GetQuad(column + 1, row + 1).SetVertex(Quad::Vertices::BottomRight, column + 1, fogHeight, row, TEMP_uvCoord, TEMP_uvCoord);


	CheckAdjacentTiles(column - 1, row - 1);
	CheckAdjacentTiles(column - 1, row + 1);
	CheckAdjacentTiles(column + 1, row - 1);
	CheckAdjacentTiles(column + 1, row + 1);
}
