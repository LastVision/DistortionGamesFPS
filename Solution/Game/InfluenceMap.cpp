#include "stdafx.h"
#include "InfluenceMap.h"


InfluenceMap::InfluenceMap()
	: AIMap()
	, myForgetSpeed(0.01f)
{
}


InfluenceMap::~InfluenceMap()
{
}

void InfluenceMap::Update()
{
	for (int i = 0; i < myGrid.Size(); ++i)
	{
		myGrid[i] *= myForgetSpeed;
	}
}