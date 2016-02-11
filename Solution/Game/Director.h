#pragma once

#include <GrowingArray.h>
#include <Subscriber.h>
#include <GameStructs.h>

class Entity;
class BuildingComponent;

namespace Prism
{
	class Terrain;
}

class Director : public Subscriber
{
	friend class LevelFactory;
public:
	Director(eOwnerType aOwnerType, const Prism::Terrain& aTerrain);
	virtual ~Director();

	virtual void Update(float aDeltaTime);
	void RenderHealthBars(const Prism::Camera& aCamera);

	void CleanUp();

	bool SpawnUnit(eUnitType aUnitType);
	bool UpgradeUnit(eUnitType aUnitType);

	const int& GetVictoryPoints() const;
	const int& GetUnitCap() const;
	const int& GetUnitCount() const;

	const int GetActiveUnitsSize() const;
	
	virtual void ReceiveMessage(const SpawnUnitMessage& aMessage) override;
	void ReceiveMessage(const ResourceMessage& aMessage) override;
	void ReceiveMessage(const VictoryMessage& aMessage) override;
	void ReceiveMessage(const UpgradeUnitMessage& aMessage) override;
	void ReceiveMessage(const KillUnitMessage& aMessage) override;
	void ReceiveMessage(const ArtifactMessage& aMessage) override;
	void ReceiveMessage(const KilledPromotedMessage& aMessage) override;

	void SetGunPowder(int anAmount);
	void SetBaseIncome(int anAmount);

	Entity& GetBuilding() const;
	const BuildingComponent& GetBuildingComponent() const;

protected:
	bool IsAlreadyActive(Entity* aUnit);

	void ResourceGainUnlockCheck();

	CU::GrowingArray<Entity*> myUnits;
	CU::GrowingArray<Entity*> myActiveUnits;
	CU::GrowingArray<Entity*> myDeadUnits;
	Entity* myBuilding;
	const eOwnerType myOwner;
	const Prism::Terrain& myTerrain;

	float myTimeMultiplier;
	int myGunpowder;
	int myArtifacts;
	int myVictoryPoints;
	int myUnitCap;
	int myUnitCount;
	int myBaseIncome;

	bool myHasUnlockedRanger;
	bool myHasUnlockedTank;
	bool myHasUnlockedResource;

	float myIncomeTimer;
	float myCurrentIncomeTimer;

private:
	void operator=(Director&) = delete;
};

inline const int& Director::GetVictoryPoints() const
{
	return myVictoryPoints;
}

inline const int& Director::GetUnitCap() const
{
	return myUnitCap;
}

inline const int& Director::GetUnitCount() const
{
	return myUnitCount;
}

inline const int Director::GetActiveUnitsSize() const
{
	return myActiveUnits.Size();
}

inline void Director::SetGunPowder(int anAmount)
{
	myGunpowder = anAmount;
}

inline void Director::SetBaseIncome(int anAmount)
{
	myBaseIncome = anAmount;
}

inline Entity& Director::GetBuilding() const
{
	return *myBuilding;
}