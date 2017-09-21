#include "FlowField.h"
#include <Profiler.h>

using namespace SE;
using namespace Gameplay;

FlowField::FlowField(const char mapData[MAXSIZE][MAXSIZE], float sideLength, const pos & initialPullingPosition, float bottomLeftCornerXCoord, float bottomLeftCornerYCoord)
{
	StartProfile;
	this->sideLength = sideLength;
	lowerLeftCornerXCoord = bottomLeftCornerXCoord;
	lowerLeftCornerYCoord = bottomLeftCornerYCoord;

	for (int i = 0; i < MAXSIZE; i++)
	{
		for (int j = 0; j < MAXSIZE; j++)
		{
			if (mapData[i][j] == 0)
			{
				data[i][j].type = Type::WALKABLE;
			}
			else if (mapData[i][j] == 1)
			{
				data[i][j].type = Type::BLOCKED;
			}

			data[i][j].posX = bottomLeftCornerXCoord + i * sideLength + sideLength / 2;
			data[i][j].posY = bottomLeftCornerYCoord + j * sideLength + sideLength / 2;

		}
	}

	Update(initialPullingPosition);
	StopProfile;
}

FlowField::~FlowField()
{

}

void FlowField::Update(const pos & playerPos)
{
	StartProfile;
	ResetField();
	data[(int)playerPos.x][(int)playerPos.y].cost = 0;
	RecursiveLeifUpdate((int)playerPos.x, (int)playerPos.y);
	SetDirectionsBasedOnCosts();
	StopProfile;
}

void FlowField::SampleFromMap(const pos & enemyPos, float & xMagnitude, float & yMagnitude)
{
	StartProfile;
	int xIndex = (int)enemyPos.x - lowerLeftCornerXCoord;
	int yIndex = (int)enemyPos.y - lowerLeftCornerYCoord;



	switch (data[xIndex][yIndex].dir)
	{
		case Direction::NONE:
			xMagnitude = 0.0f;
			yMagnitude = 0.0f;
			StopProfile;
			return;
		
		case Direction::UP: 
			xMagnitude = 0.0f;
			yMagnitude = 1.0f;
			StopProfile;
			return;

		case Direction::LEFT: 
			xMagnitude = -1.0f;
			yMagnitude = 0.0f;
			StopProfile;
			return;

		case Direction::DOWN: 
			xMagnitude = 0.0f;
			yMagnitude = -1.0f;
			StopProfile;
			return;

		case Direction::RIGHT: 
			xMagnitude = 1.0f;
			yMagnitude = 0.0f;
			StopProfile;
			return;

	}
	StopProfile;
}

void FlowField::ResetField()
{
	StartProfile;
	for (int i = 0; i < MAXSIZE; i++)
	{
		for (int j = 0; j < MAXSIZE; j++)
		{
			data[i][j].cost = -1;
		}
	}
	StopProfile;
}

void FlowField::RecursiveLeifUpdate(unsigned int currentX, unsigned int currentY)
{
	StartProfile;
	if (currentX > 0 && data[currentX - 1][currentY].type != Type::BLOCKED && data[currentX - 1][currentY].cost > data[currentX][currentY].cost + 1)
	{
		data[currentX - 1][currentY].cost = data[currentX][currentY].cost + 1;
		RecursiveLeifUpdate(currentX - 1, currentY);
	}

	if (currentX < MAXSIZE - 1 && data[currentX + 1][currentY].type != Type::BLOCKED && data[currentX + 1][currentY].cost > data[currentX][currentY].cost + 1)
	{
		data[currentX + 1][currentY].cost = data[currentX][currentY].cost + 1;
		RecursiveLeifUpdate(currentX + 1, currentY);
	}

	if (currentY > 0 && data[currentX][currentY - 1].type != Type::BLOCKED && data[currentX][currentY - 1].cost > data[currentX][currentY].cost + 1)
	{
		data[currentX][currentY - 1].cost = data[currentX][currentY].cost + 1;
		RecursiveLeifUpdate(currentX, currentY - 1);
	}

	if (currentX < MAXSIZE - 1 && data[currentX][currentY + 1].type != Type::BLOCKED && data[currentX][currentY + 1].cost > data[currentX][currentY].cost + 1)
	{
		data[currentX][currentY + 1].cost = data[currentX][currentY].cost + 1;
		RecursiveLeifUpdate(currentX, currentY + 1);
	}
	StopProfile;
}

void FlowField::SetDirectionsBasedOnCosts()
{
	StartProfile;
	unsigned int smallestCost = -1;
	int xModifier;
	int yModifier;

	for (int i = 1; i < MAXSIZE - 1; i++)
	{
		for (int j = 1; j < MAXSIZE - 1; j++)
		{
			smallestCost = -1;

			for (int k = 0; k < 4; k++)
			{
				if (k == 1)
				{
					xModifier = -1;
				}
				else if (k == 2)
				{
					xModifier = 1;
				}
				else
				{
					xModifier = 0;
				}

				if (k == 0)
				{
					yModifier = 1;
				}
				else if (k == 3)
				{
					yModifier = -1;
				}
				else
				{
					yModifier = 0;
				}

				if (data[i + xModifier][j + yModifier].cost < smallestCost)
				{
					if (xModifier == 1)
					{
						data[i][j].dir = Direction::RIGHT;
					}
					else if (xModifier == -1)
					{
						data[i][j].dir = Direction::LEFT;
					}
					else if (yModifier == 1)
					{
							data[i][j].dir = Direction::UP;
					}
					else
					{
							data[i][j].dir = Direction::DOWN;
					}

					smallestCost = data[i + xModifier][j + yModifier].cost;
				}
			}
		}
	}
	StopProfile;
}