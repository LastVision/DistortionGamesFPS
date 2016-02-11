#pragma once
#include "BuildingComponent.h"

struct BuildingComponentData
{
	bool myExistsInEntity = false;
	bool myUpgradesInQueue;
	float myMineFieldRadius;
	CU::StaticArray<eUnitType, 3> myBuildUnitTypes;
	CU::StaticArray<int, 3> myUnitCosts;
	CU::StaticArray<float, 3> myUnitBuildTimes;
	CU::StaticArray<CU::StaticArray<Upgrade, 3>, 3> myUnitUpgrades;
	CU::StaticArray<int, 3> myUnitSupplyCosts;
	int myMaxQueue;
};
