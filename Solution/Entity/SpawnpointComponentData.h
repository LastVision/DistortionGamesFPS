#pragma once
#include <GrowingArray.h>

struct SpawnpointComponentData
{
	bool myExistsInEntity = false;
	CU::GrowingArray<std::string> myUnitTypes;
	float mySpawnpointLifetime = 0.f;
	float mySpawnInterval = 0.f;
	int mySpawnPerInterval = 0;
	int myUnitCount = 0;
};