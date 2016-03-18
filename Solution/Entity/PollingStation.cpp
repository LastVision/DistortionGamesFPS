#include "stdafx.h"
#include "PollingStation.h"

PollingStation* PollingStation::myInstance = nullptr;
PollingStation* PollingStation::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new PollingStation();
	}

	return myInstance;
}

void PollingStation::Destroy()
{
	SAFE_DELETE(myInstance);
}

void PollingStation::AddEntity(const Entity& anEntity)
{
	//myEnemies.Add(anEntity);
	//if (anEntity.GetSubType() == "player")
	//{
	//	myEnemies.RemoveNonCyclicAtIndex(myEnemies.Size() - 1);
	//	myPlayers.Add(anEntity);
	//}
}


Entity* PollingStation::FindClosestEntityToEntity(const Entity& anEntity)
{
	Entity* toReturn = nullptr;

	//CU::GrowingArray<const Entity&>& entities = myPlayers;
	//if (anEntity.GetSubType() == "player")
	//{
	//	entities = myEnemies;
	//}

	//for (int i = 0; i < entities.Size(); ++i)
	//{
	//	float distance = CU::Length2(entities[i].GetOrientation().GetPos() - anEntity.GetOrientation().GetPos());


	//}

	return  toReturn;
}

PollingStation::PollingStation()
	//: myPlayers(16)
	//, myEnemies(128)
{
}

PollingStation::~PollingStation()
{
}