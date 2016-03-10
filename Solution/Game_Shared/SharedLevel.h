#pragma once
#include <GrowingArray.h>
#include <Subscriber.h>

class Entity;
class PhysicsComponent;

class SharedLevel : public Subscriber
{
public:
	SharedLevel();
	virtual ~SharedLevel();

	void AddEntity(Entity* aEntity);
	void AddEnemy(Entity* anEntity);

	virtual void Update(const float aDeltaTime);

	virtual void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond) = 0;

protected:
	CU::GrowingArray<Entity*> myEntities;
	CU::GrowingArray<Entity*> myEnemies;
	CU::GrowingArray<Entity*> myPlayers;
};

