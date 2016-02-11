#include "stdafx.h"
#include "DecisionMap.h"
#include "DifferenceMap.h"
#include "GoalMap.h"
#include "TensionMap.h"
#include "VulnerabilityMap.h"

DecisionMap::DecisionMap()
	: AIMap()
	, myDifference(nullptr)
	, myGoal(nullptr)
	, myInfluence(nullptr)
	, myTension(nullptr)
	, myVulnerability(nullptr)
{
}


DecisionMap::~DecisionMap()
{
}

void DecisionMap::SetDifferenceMap(DifferenceMap* aDifferenceMap)
{
	myDifference = aDifferenceMap;
}

void DecisionMap::SetGoalMap(GoalMap* aGoalMap)
{
	myGoal = aGoalMap;
}

void DecisionMap::SetInfluenceMap(InfluenceMap* aInfluenceMap)
{
	myInfluence = aInfluenceMap;
}

void DecisionMap::SetTensionMap(TensionMap* aTensionMap)
{
	myTension = aTensionMap;
}

void DecisionMap::SetVulnerabilityMap(VulnerabilityMap* aVulnerabilityMap)
{
	myVulnerability = aVulnerabilityMap;
}

CU::Vector2<float> DecisionMap::GetOffensivePosition(const CU::Vector2<float>& aPosition) const
{
	float bestValue = -1.f;
	int bestIndex = -1;

	for (int i = 0; i < myGrid.Size(); ++i)
	{
		float distance = CU::Length(GetPosition(i) - aPosition);
		float tileValue = (-myDifference->GetGrid()[i] + myVulnerability->GetGrid()[i]) / distance;

		if (tileValue > bestValue)
		{
			bestValue = tileValue;
			bestIndex = i;
		}
	}

	return GetPosition(bestIndex);
}