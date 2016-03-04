#pragma once
#include <GrowingArray.h>
#include <Subscriber.h>

class Entity;

class SharedLevel : public Subscriber
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

