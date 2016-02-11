#pragma once
#include "AIMap.h"

class InfluenceMap;
class DifferenceMap : public AIMap
{
public:
	DifferenceMap(InfluenceMap* aInfluence, InfluenceMap* aOtherInfluence);
	~DifferenceMap();

	void Update() override;

private:
	InfluenceMap* myInfluence;
	InfluenceMap* myOtherInfluence;
};

