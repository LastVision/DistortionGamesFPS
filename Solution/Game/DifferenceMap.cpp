#include "stdafx.h"
#include "DifferenceMap.h"
#include "InfluenceMap.h"

DifferenceMap::DifferenceMap(InfluenceMap* aInfluence, InfluenceMap* aOtherInfluence)
	: AIMap()
	, myInfluence(aInfluence)
	, myOtherInfluence(aOtherInfluence)
{
}


DifferenceMap::~DifferenceMap()
{
}

void DifferenceMap::Update()
{
	for (int i = 0; i < myGrid.Size(); ++i)
	{
		myGrid[i] = myInfluence->GetGrid()[i] - myOtherInfluence->GetGrid()[i];
	}
}