#include "stdafx.h"
#include "PollingStation.h"
#include <iostream>
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

PollingStation::PollingStation()
	: myPlayers(16)
	, myEnemies(128)
{
}

PollingStation::~PollingStation()
{
}

void PollingStation::AddEntity(Entity* anEntity)
{
	if (anEntity->GetSubType() == "playerserver")
	{
		DL_ASSERT_EXP(myPlayers.Find(anEntity) == myPlayers.FoundNone, "Can't add player twice.");
		myPlayers.Add(anEntity);
	}
	else
	{
		DL_ASSERT_EXP(myEnemies.Find(anEntity) == myEnemies.FoundNone, "Can't add enemy twice.");
		myEnemies.Add(anEntity);
	}
}

void PollingStation::RemovePlayer(Entity* aPlayer)
{
	DL_ASSERT_EXP(myPlayers.Find(aPlayer) != myPlayers.FoundNone, "Player not found.");
	myPlayers.RemoveCyclic(aPlayer);
}


Entity* PollingStation::FindClosestEntityToEntity(const Entity& anEntity)
{
	Entity* toReturn = nullptr;

	CU::GrowingArray<Entity*>& entities = myPlayers;

	if (anEntity.GetSubType() == "playerserver")
	{
		entities = myEnemies;
	}
	float distance = 0.f;
	float prevDistance = 0.f;
	for (int i = 0; i < entities.Size(); ++i)
	{
		distance = CU::Length(entities[i]->GetOrientation().GetPos() - anEntity.GetOrientation().GetPos());

		/*if (entities[i]->IsAlive() == true)
		{*/
		toReturn = entities[i];

		prevDistance = distance;
		/*}*/
	}
	return  toReturn;
}

Entity* PollingStation::FindClosestPlayer(const CU::Vector3<float>& aPosition, float aMaxRange)
{
	Entity* toReturn = nullptr;

	float currentMinDistance2 = FLT_MAX;

	for each (Entity* player in myPlayers)
	{
		float distance2 = CU::Length2(player->GetOrientation().GetPos() - aPosition);
		if (distance2 < aMaxRange * aMaxRange && distance2 < currentMinDistance2)
		{
			currentMinDistance2 = distance2;
			toReturn = player;
		}
	}

	return toReturn;
}

