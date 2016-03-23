#include "stdafx.h"
#include "SpawnpointComponent.h"
#include "SpawnpointComponentData.h"

#include "Entity.h"

#include <MathHelper.h>
#include "SharedUnitManager.h"

#include <NetMessageSetActive.h>

SpawnpointComponent::SpawnpointComponent(Entity& anEntity, const SpawnpointComponentData& aSpawnpointComponentData)
	: Component(anEntity)
	, myData(aSpawnpointComponentData)
	, myIsActive(false)
	, myUnits(512)
{


	myUnitCount = (myData.mySpawnPerInterval * myData.mySpawnpointLifetime) / myData.mySpawnInterval;
	mySpawnTimer = myData.mySpawnInterval;
	myLifetime = myData.mySpawnpointLifetime;

	for (unsigned int i = 0; i < myUnitCount; ++i)
	{
		int randomType = CU::Math::RandomRange(0, aSpawnpointComponentData.myUnitTypes.Size());
		myUnits.Add(SharedUnitManager::GetInstance()->RequestUnit(myData.myUnitTypes[randomType]));
	}


}


SpawnpointComponent::~SpawnpointComponent()
{
	myUnits.RemoveAll();
}

void SpawnpointComponent::Update(float aDelta)
{
	if (myIsActive == true)
	{
		myLifetime -= aDelta;
		if (myLifetime > 0.f)
		{
			SpawnUnit(aDelta);
		}
		else
		{
			DeActivate();
		}
	}
}

void SpawnpointComponent::Activate()
{
	myLifetime = myData.mySpawnpointLifetime;
	mySpawnTimer = myData.mySpawnInterval;
	myIsActive = true;
}

void SpawnpointComponent::DeActivate()
{
	myIsActive = false;
}

void SpawnpointComponent::ReceiveNetworkMessage(const NetMessageSetActive& aMessage, const sockaddr_in&)
{
	if (myEntity.GetGID() == aMessage.myGID)
	{
		if (myIsActive == false)
		{
			Activate();
		}
	}
}

void SpawnpointComponent::SpawnUnit(float aDelta)
{
	if (myUnitIndex >= myUnits.Size())
	{
		myUnitIndex = 0;
	}

	mySpawnTimer -= aDelta;
	if (mySpawnTimer < 0.f)
	{
		for (int i = 0; i < myData.mySpawnPerInterval; ++i)
		{
			if (myActiveCount < myUnitCount)
			{
				SharedUnitManager::GetInstance()->ActivateUnit(myUnits[myUnitIndex]);
				myUnitIndex++;
			}
		}
		mySpawnTimer = myData.mySpawnInterval;
	}
}
