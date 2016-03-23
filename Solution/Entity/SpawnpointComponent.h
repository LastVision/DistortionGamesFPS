#pragma once
#include "Component.h"
#include <NetworkSubscriber.h>

class Entity;
struct SpawnpointComponentData;

class SpawnpointComponent : public Component, public NetworkSubscriber
{
public:
	SpawnpointComponent(Entity& anEntity, const SpawnpointComponentData& aSpawnpointComponentData);
	~SpawnpointComponent();

	void Update(float aDelta) override;
	void Activate();
	void DeActivate();

	void ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in& aSenderAddress) override;

private:

	void SpawnUnit(float aDelta);

	const SpawnpointComponentData& myData;
	CU::GrowingArray<Entity*> myUnits;
	int myUnitCount;
	int myActiveCount;
	int myUnitIndex;
	float mySpawnTimer;
	float myLifetime;
	bool myIsActive;
};

