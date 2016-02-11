#include "stdafx.h"
#include "BlockMap.h"


BlockMap::BlockMap()
{
}


BlockMap::~BlockMap()
{
}

void BlockMap::Update()
{
	//Do nothing
}

void BlockMap::RemoveValue(float aValue, float aRadius, const CU::Vector2<float>& aPosition)
{
	CU::Vector2<int> topLeft(GetIntPosition(aPosition - aRadius));
	CU::Vector2<int> botRight(GetIntPosition(aPosition + aRadius));

	for (int y = topLeft.y; y < botRight.y; ++y)
	{
		for (int x = topLeft.x; x < botRight.x; ++x)
		{
			float distance = CU::Length(GetPosition(x, y) - aPosition);
			if (distance < aRadius && ValidIndex(x, y))
			{
				int index = x + y * mySide;
				myGrid[index] -= aValue / (distance / myTileSize);
				myGrid[index] = fmaxf(myGrid[index], 0);
			}
		}
	}

	int index = GetIndex(aPosition);
	if (index >= 0 && index < myGrid.Size())
	{
		myGrid[index] -= aValue;
		myGrid[index] = fmaxf(myGrid[index], 0);
	}
}