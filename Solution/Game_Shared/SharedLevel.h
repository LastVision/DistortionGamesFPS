#pragma once
#include <GrowingArray.h>
#include <unordered_map>
#include <Subscriber.h>
#include <NetworkSubscriber.h>

class Entity;
class PhysicsComponent;

class SharedLevel : public Subscriber, public NetworkSubscriber
{
public:
	SharedLevel();
	virtual ~SharedLevel();

	virtual void Init() = 0;

	void AddEntity(Entity* aEntity);
	void AddEnemy(Entity* anEntity);

	virtual void Update(const float aDeltaTime);

	void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond);

	void CleanUp();

protected:
	void HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity);
	void HandleExplosion(Entity& aFirstEntity, Entity& aSecondEntity);

	CU::GrowingArray<Entity*> myActiveEntities;
	CU::GrowingArray<Entity*> myInactiveEntities;
	CU::GrowingArray<Entity*> myActiveEnemies;
	CU::GrowingArray<Entity*> myInactiveEnemies;
	CU::GrowingArray<Entity*> myPlayers;

	std::unordered_map<unsigned int, Entity*> myActiveEntitiesMap;
};

