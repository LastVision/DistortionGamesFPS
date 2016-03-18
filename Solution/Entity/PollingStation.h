#pragma once

class Entity;
class PollingStation
{
public:
	static PollingStation* GetInstance();
	static void Destroy();

	void AddEntity(const Entity& anEntity);
	Entity* FindClosestEntityToEntity(const Entity& anEntity);
	Entity* FindClosestEntity(const CU::Vector3<float>& aPosition, float aMaxRange);


private:
	PollingStation();
	~PollingStation();

	static PollingStation* myInstance;

	//CU::GrowingArray<const Entity&> myPlayers;
	//CU::GrowingArray<const Entity&> myEnemies;

};


