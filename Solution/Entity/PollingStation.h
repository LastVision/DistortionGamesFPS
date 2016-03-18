#pragma once

class Entity;
class PollingStation
{
public:
	static PollingStation* GetInstance();
	static void Destroy();

	void AddEntity(Entity* anEntity);
	Entity* FindClosestEntityToEntity(const Entity& anEntity);
	Entity* FindClosestEntity(const CU::Vector3<float>& aPosition, float aMaxRange);

private:
	PollingStation();
	~PollingStation();

	static PollingStation* myInstance;

	CU::GrowingArray<Entity*> myPlayers;
	CU::GrowingArray<Entity*> myEnemies;
};


