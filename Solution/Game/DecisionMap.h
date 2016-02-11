#pragma once
#include "AIMap.h"

class DifferenceMap;
class GoalMap;
class InfluenceMap;
class TensionMap;
class VulnerabilityMap;

class DecisionMap : public AIMap
{
public:
	DecisionMap();
	~DecisionMap();

	void Update(){};

	void SetDifferenceMap(DifferenceMap* aDifferenceMap);
	void SetGoalMap(GoalMap* aGoalMap);
	void SetInfluenceMap(InfluenceMap* aInfluenceMap);
	void SetTensionMap(TensionMap* aTensionMap);
	void SetVulnerabilityMap(VulnerabilityMap* aVulnerabilityMap);

	CU::Vector2<float> GetOffensivePosition(const CU::Vector2<float>& aPosition) const;

private:
	DifferenceMap* myDifference;
	GoalMap* myGoal;
	InfluenceMap* myInfluence;
	TensionMap* myTension;
	VulnerabilityMap* myVulnerability;
};

