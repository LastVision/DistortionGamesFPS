#pragma once
#include <GrowingArray.h>
#include <Subscriber.h>
#include "../../PhysX/Include/PxSimulationEventCallback.h"

class Entity;

class SharedLevel : public Subscriber, public physx::PxSimulationEventCallback
{
public:
	SharedLevel();
	virtual ~SharedLevel();

	void AddEntity(Entity* aEntity);

	virtual void Update(const float aDeltaTime);

protected:
	CU::GrowingArray<Entity*> myEntities;
	CU::GrowingArray<Entity*> myPlayers;
};

