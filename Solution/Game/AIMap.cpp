#include "stdafx.h"
#include "AIMap.h"
#include <RenderPlane.h>

#define HEIGHT_MAP_WIDTH 513.f

AIMap::AIMap(int aSide)
	: mySide(aSide)
	, myGrid(mySide * mySide)
	, myLevelWidth(256)
	, myMaxValue(40.f)
{
	for (int i = 0; i < mySide * mySide; ++i)
	{
		myGrid.Add(0.f);
	}

	myOrientation.SetPos({ 0.f, 15.f, 0.f });

	myTileSize = static_cast<float>(myLevelWidth) / static_cast<float>(mySide);
	myLevelCellSize = myLevelWidth / HEIGHT_MAP_WIDTH;


	myPlane = new Prism::RenderPlane(aSide, myTileSize);
}


AIMap::~AIMap()
{
	SAFE_DELETE(myPlane);
}

void AIMap::Render(const Prism::Camera& aCamera)
{
	UpdateRenderPlane();

	myPlane->Render(aCamera, myOrientation);
}

void AIMap::UpdateRenderPlane()
{
	myPlane->StartModify();

	for (int i = 0; i < myGrid.Size(); ++i)
	{
		myPlane->SetVertexColor(i, GetColor(myGrid[i] / 40.f));
	}

	myPlane->EndModify();
}

CU::Vector2<float> AIMap::GetPosition(int aIndex) const
{
	int x = aIndex % mySide;
	int y = aIndex / mySide;

	return GetPosition(x, y);
}

CU::Vector2<int> AIMap::GetIntPosition(const CU::Vector2<float>& aPosition) const
{
	int index = GetIndex(aPosition);
	return CU::Vector2<int>(index % mySide, index / mySide);
}

CU::Vector2<float> AIMap::GetPosition(int aX, int aY) const
{
	return CU::Vector2<float>(aX * myTileSize, aY * myTileSize);
}

int AIMap::GetIndex(const CU::Vector2<float>& aPosition) const
{
	CU::Vector2<float> position(aPosition);
	position.x = fminf(255.f, fmaxf(0.f, position.x));
	position.y = fminf(255.f, fmaxf(0.f, position.y));
	int x = static_cast<int>((position.x + 0.5f) / myTileSize);
	int y = static_cast<int>((position.y + 0.5f) / myTileSize);

	return x + y * mySide;
}

void AIMap::AddValue(float aValue, float aRadius, const CU::Vector2<float>& aPosition)
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
				myGrid[index] += aValue / (distance/myTileSize);
				myGrid[index] = fminf(myGrid[index], myMaxValue);
			}
		}
	}

	int index = GetIndex(aPosition);
	if (index >= 0 && index < myGrid.Size())
	{
		myGrid[index] += aValue;
	}
}

void AIMap::SetValue(int aIndex, float aValue)
{
	myGrid[aIndex] = aValue;
}

float AIMap::GetValue(const CU::Vector2<float>& aPosition) const
{
	int index = GetIndex(aPosition);
	if (index >= 0 && index < myGrid.Size())
	{
		return myGrid[index];
	}

	DL_ASSERT("AIMap::GetValue, Invalid position given");
	return 0.f;
}

Prism::RenderPlane* AIMap::GetRenderPlane()
{
	return myPlane;
}

CU::Vector4<float> AIMap::GetColor(float aValue) const
{
	if (aValue < 0.f)
	{
		return{ 0.f, 0.f, fabsf(aValue), 1.f };
	}

	return{ aValue, aValue, aValue, 1.f };
}

bool AIMap::ValidIndex(int aX, int aY) const
{
	if (aX < 0 || aX > mySide - 1) return false;
	if (aY < 0 || aY > mySide - 1) return false;

	return true;
}
