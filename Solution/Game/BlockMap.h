#pragma once
#include "AIMap.h"
class BlockMap : public AIMap
{
public:
	BlockMap();
	~BlockMap();

	void Update() override;

	void RemoveValue(float aValue, float aRadius, const CU::Vector2<float>& aPosition);
};

