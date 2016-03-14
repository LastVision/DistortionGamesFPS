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

	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond);

	void CleanUp();

protected:
	CU::GrowingArray<Entity*> myActiveEntities;
	CU::GrowingArray<Entity*> myInactiveEntities;
	CU::GrowingArray<Entity*> myActiveEnemies;
	CU::GrowingArray<Entity*> myInactiveEnemies;
	CU::GrowingArray<Entity*> myPlayers;
};

