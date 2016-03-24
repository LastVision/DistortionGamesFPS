#pragma once
#include "Component.h"

class Entity;
struct SpawnpointComponentData;

class SpawnpointComponent : public Component
{
public:
	SpawnpointComponent(Entity& anEntity, const SpawnpointComponentData& aSpawnpointComponentData);
	~SpawnpointComponent();

	void Update(float aDelta) override;



private:
	const SpawnpointComponentData& myData;

	CU::GrowingArray<Entity*> myUnits;

};

