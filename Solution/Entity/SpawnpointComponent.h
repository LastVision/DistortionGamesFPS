#pragma once
#include "Component.h"
#include <Subscriber.h>
class Entity;
struct SpawnpointComponentData;

class SpawnpointComponent : public Component, public Subscriber
{
public:
	SpawnpointComponent(Entity& anEntity, const SpawnpointComponentData& aSpawnpointComponentData);
	~SpawnpointComponent();

	void Update(float aDelta) override;
	void Activate();
	void DeActivate();

	void ReceiveMessage(const ActivateSpawnpointMessage& aMessage) override;

	void BindToTrigger(unsigned int aGID);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
private:

	void SpawnUnit(float aDelta);

	const SpawnpointComponentData& myData;
	CU::GrowingArray<Entity*> myUnits;

	CU::GrowingArray<unsigned int> myTriggerConnections;

	int myUnitCount;
	int myActiveCount;
	int myUnitIndex;
	float mySpawnTimer;
	float myLifetime;
	bool myIsActive;
};

inline eComponentType SpawnpointComponent::GetTypeStatic()
{
	return eComponentType::SPAWNPOINT;
}

inline eComponentType SpawnpointComponent::GetType()
{
	return GetTypeStatic();
}

