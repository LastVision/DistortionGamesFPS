#pragma once
#include "AIMap.h"

class InfluenceMap;
class TensionMap : public AIMap
{
public:
	TensionMap(InfluenceMap* aInfluence, InfluenceMap* aPlayerInfluence, InfluenceMap* aNeutralInfluence
		, InfluenceMap* aGoalInfluence);
	~TensionMap();

	void Update() override;

private:
	InfluenceMap* myInfluence;
	InfluenceMap* myPlayerInfluence;
	InfluenceMap* myNeutralInfluence;
	InfluenceMap* myGoalInfluence;
};

