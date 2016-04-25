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

	virtual void Init(const std::string& aMissionXMLPath) = 0;

	void AddEntity(Entity* aEntity);
	void AddEnemy(Entity* anEntity);
	void AddPlayerStartPosition(unsigned int aGID, const CU::Vector3<float>& aPosition);

	virtual void Update(const float aDeltaTime, bool aLoadingScreen);

	virtual void CollisionCallback(PhysicsComponent* aFirst, PhysicsComponent* aSecond, bool aHasEntered) = 0;
	virtual void ContactCallback(PhysicsComponent*, PhysicsComponent*){};

	void CleanUp();

protected:
	virtual void HandleTrigger(Entity& aFirstEntity, Entity& aSecondEntity, bool aHasEntered) = 0;
	void HandleExplosion(Entity& aFirstEntity, Entity& aSecondEntity);

	CU::GrowingArray<Entity*> myActiveEntities;
	CU::GrowingArray<Entity*> myInactiveEntities;
	CU::GrowingArray<Entity*> myActiveEnemies;
	CU::GrowingArray<Entity*> myInactiveEnemies;
	CU::GrowingArray<Entity*> myPlayers;

	std::unordered_map<unsigned int, Entity*> myActiveEntitiesMap;

	std::unordered_map<unsigned int, Entity*> myActiveUnitsMap;

	std::unordered_map<unsigned int, CU::Vector3<float>> myPlayerStartPositions;
};

