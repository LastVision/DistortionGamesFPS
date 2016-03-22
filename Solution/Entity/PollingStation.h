#pragma once

class Entity;
class PollingStation
{
public:
	static PollingStation* GetInstance();
	static void Destroy();

	void AddEntity(Entity* anEntity);
	void RemovePlayer(Entity* aPlayer);

	Entity* FindClosestEntityToEntity(const Entity& anEntity);
	Entity* FindClosestEntity(const CU::Vector3<float>& aPosition, float aMaxRange);
	Entity* FindClosestPlayer(const CU::Vector3<float>& aPosition, float aMaxRange);

	const CU::GrowingArray<Entity*>& GetPlayers() const;

private:
	PollingStation();
	~PollingStation();

	static PollingStation* myInstance;

	CU::GrowingArray<Entity*> myPlayers;
	CU::GrowingArray<Entity*> myEnemies;
};


inline const CU::GrowingArray<Entity*>& PollingStation::GetPlayers() const
{
	return myPlayers;
}