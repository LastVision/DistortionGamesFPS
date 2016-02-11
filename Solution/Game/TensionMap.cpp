#include "stdafx.h"

#include "InfluenceMap.h"
#include "TensionMap.h"


TensionMap::TensionMap(InfluenceMap* aInfluence, InfluenceMap* aPlayerInfluence, InfluenceMap* aNeutralInfluence
	, InfluenceMap* aGoalInfluence)
	: AIMap()
	, myInfluence(aInfluence)
	, myPlayerInfluence(aPlayerInfluence)
	, myNeutralInfluence(aNeutralInfluence)
	, myGoalInfluence(aGoalInfluence)
{
}


TensionMap::~TensionMap()
{
}

void TensionMap::Update()
{
	for (int i = 0; i < myGrid.Size(); ++i)
	{
		myGrid[i] = myInfluence->GetGrid()[i] + myPlayerInfluence->GetGrid()[i]
			+ myNeutralInfluence->GetGrid()[i] + myGoalInfluence->GetGrid()[i];
	}
}
