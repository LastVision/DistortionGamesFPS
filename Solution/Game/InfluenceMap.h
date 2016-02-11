#pragma once
#include "AIMap.h"
class InfluenceMap : public AIMap
{
public:
	InfluenceMap();
	~InfluenceMap();

	void Update() override;

private:
	float myForgetSpeed;
};

