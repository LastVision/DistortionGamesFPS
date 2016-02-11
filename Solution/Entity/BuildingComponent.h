#pragma once
#include "Component.h"
#include <StaticArray.h>
#include <queue>
#include <GameStructs.h>

struct BuildingComponentData;

struct BuildInfo
{
	BuildInfo() : myUnit(eUnitType::NOT_A_UNIT), myIsUpgrade(false){}
	BuildInfo(eUnitType aUnit, bool aIsUpgrade) : myUnit(aUnit), myIsUpgrade(aIsUpgrade){}
	eUnitType myUnit;
	bool myIsUpgrade;
};

class BuildingComponent : public Component
{
public:
	BuildingComponent(Entity& aEntity, BuildingComponentData& aData);
	~BuildingComponent();

	void Reset() override;

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	void BuildUnit(eUnitType aUnitType);
	void UpgradeUnit(eUnitType aUnitType);

	eUnitType GetEntityToSpawn() const;
	const float& GetCurrentBuildTime() const;
	const float& GetMaxBuildTime() const;
	int GetSpawnQueueSize() const;
	const int& GetUnitCost(eUnitType aUnitType) const;
	const int& GetUnitSupplyCost(eUnitType aUnitType) const;
	int GetUpgradeCost(eUnitType aUnitType);
	const int& GetUpgradeCost(eUnitType aUnitType, int aUpgradeLevel) const;
	bool CanUpgrade(eUnitType aUnitType) const;

	void SetIgnoreBuildTime(bool anIgnoreBuildTime); // enables instabuilding

	bool IsQueueFull() const;

	const int& GetUpgradeLevel(int aUnitID) const;
	const float& GetUpgradeCooldown(int aUnitID) const;
	const float& GetUpgradeMaxCooldown(int aUnitID) const;

	const CU::Vector2<float>& GetRallyPoint() const;
	void SetRallyPoint(const CU::Vector2<float>& aPoint);

	void Abort(int aIndex);

	const bool GetHasSpawnedAtLeastOnce() const;

	int GetMaxQueue() const;

	const CU::GrowingArray<BuildInfo>& GetQueue() const;

	int GetTotalQueueSupplyCost() const;

private:
	
	void UpdateUpgradeCooldown(float aDelta, eUnitType aUnit);

	CU::Vector2<float> mySpawnPoint;
	CU::Vector2<float> myRallyPoint;

	CU::StaticArray<int, 3> myUnitCosts;
	CU::StaticArray<float, 3> myUnitBuildTimes;
	CU::StaticArray<CU::StaticArray<Upgrade, 3>, 3> myUnitUpgrades; //3 upgrade-levels for all 3 units
	CU::StaticArray<int, 3> myUnitUpgradeProgress;
	CU::StaticArray<int, 3> myUnitSupplyCosts;
	CU::StaticArray<float, 3> myUpgradeCooldowns;
	CU::StaticArray<float, 3> myUpgradeMaxCooldowns;

	CU::GrowingArray<BuildInfo> myBuildQueue;

	float myCurrentBuildTime;
	float myMaxBuildTime;

	bool myIgnoreBuildTime;
	bool myUpgradesInQueue;
	bool myHasSpawnedAtLeastOnce;

	int myMaxQueue;

	void HandleMineField();
	void CheckUnitsForRemove(CU::GrowingArray<Entity*>& someUnits) const;
	void CheckUnitsForAdd(const CU::GrowingArray<Entity*>& someUnits, CU::GrowingArray<Entity*>& someUnitsOut) const;
	const float myMineFieldRadius;
	CU::GrowingArray<Entity*> myUnitsInMineField;
};

inline eComponentType BuildingComponent::GetTypeStatic()
{
	return eComponentType::BUILDING;
}

inline eComponentType BuildingComponent::GetType()
{
	return GetTypeStatic();
}

inline eUnitType BuildingComponent::GetEntityToSpawn() const
{
	if (myBuildQueue.Size() <= 0)
	{
		return eUnitType::NOT_A_UNIT;
	}

	return myBuildQueue[0].myUnit;
}

inline const float& BuildingComponent::GetCurrentBuildTime() const
{
	return myCurrentBuildTime;
}

inline const float& BuildingComponent::GetMaxBuildTime() const
{
	return myMaxBuildTime;
}

inline int BuildingComponent::GetSpawnQueueSize() const
{
	return myBuildQueue.Size();
}

inline bool BuildingComponent::IsQueueFull() const
{
	return myBuildQueue.Size() >= myMaxQueue;
}

inline void BuildingComponent::SetIgnoreBuildTime(bool anIgnoreBuildTime)
{
	myIgnoreBuildTime = anIgnoreBuildTime;
}

inline const int& BuildingComponent::GetUpgradeLevel(int aUnitID) const
{
	return myUnitUpgradeProgress[aUnitID];
}

inline const float& BuildingComponent::GetUpgradeCooldown(int aUnitID) const
{
	return myUpgradeCooldowns[aUnitID];
}

inline const float& BuildingComponent::GetUpgradeMaxCooldown(int aUnitID) const
{
	return myUpgradeMaxCooldowns[aUnitID];
}

inline const CU::Vector2<float>& BuildingComponent::GetRallyPoint() const
{
	return myRallyPoint;
}

inline void BuildingComponent::SetRallyPoint(const CU::Vector2<float>& aPoint)
{
	myRallyPoint = aPoint;
}

inline const bool BuildingComponent::GetHasSpawnedAtLeastOnce() const
{
	return myHasSpawnedAtLeastOnce;
}

inline int BuildingComponent::GetMaxQueue() const
{
	return myMaxQueue;
}

inline const CU::GrowingArray<BuildInfo>& BuildingComponent::GetQueue() const
{
	return myBuildQueue;
}