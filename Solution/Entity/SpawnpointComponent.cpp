#include "stdafx.h"
#include "SpawnpointComponent.h"
#include "SpawnpointComponentData.h"

#include "Entity.h"

#include <MathHelper.h>
#include "SharedUnitManager.h"

#include <SharedNetworkManager.h>
#include <NetMessageActivateUnit.h>

#include <PostMaster.h>
#include <ActivateSpawnpointMessage.h>

SpawnpointComponent::SpawnpointComponent(Entity& anEntity, const SpawnpointComponentData& aSpawnpointComponentData)
	: Component(anEntity)
	, myData(aSpawnpointComponentData)
	, myIsActive(false)
	, myUnits(512)
	, myTriggerConnections(4)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ACTIVATE_SPAWNPOINT, this);

	myUnitCount = int((myData.mySpawnPerInterval * myData.mySpawnpointLifetime) / myData.mySpawnInterval);
	mySpawnTimer = myData.mySpawnInterval;
	myLifetime = myData.mySpawnpointLifetime;

	for (int i = 0; i < myUnitCount; ++i)
	{
		int randomType = CU::Math::RandomRange(0, aSpawnpointComponentData.myUnitTypes.Size());
		std::string server = "";
		if (myEntity.GetIsClient() == false)
		{
			server = "Server";
		}
		Entity* toAdd = SharedUnitManager::GetInstance()->RequestUnit(myData.myUnitTypes[randomType] + server);
		myUnits.Add(toAdd);
	}
}


SpawnpointComponent::~SpawnpointComponent()
{
	myUnits.RemoveAll();
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ACTIVATE_SPAWNPOINT, this);
}

void SpawnpointComponent::Update(float aDelta)
{
	if (myIsActive == false && myTriggerConnections.Size() <= 0)
	{
		Activate();
	}
	else if (myIsActive == true)
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
	myUnitIndex = 0;
	myActiveCount = 0;
	myIsActive = false;
}

void SpawnpointComponent::ReceiveMessage(const ActivateSpawnpointMessage& aMessage)
{
	for each(unsigned int gid in myTriggerConnections)
	{
		if (gid == aMessage.myGID)
		{
			if (myIsActive == false)
			{
				Activate();
				return;
			}
		}
	}
}

void SpawnpointComponent::BindToTrigger(unsigned int aGID)
{
	myTriggerConnections.Add(aGID);
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
			if (myUnits[myUnitIndex]->IsAlive() == false)
			{
				if (myActiveCount < myUnitCount)
				{
					CU::Vector3<float> spawnPosition = myEntity.GetOrientation().GetPos();
					//spawnPosition.x + 5 * i;
					SharedUnitManager::GetInstance()->ActivateUnit(myUnits[myUnitIndex], spawnPosition);
					SharedNetworkManager::GetInstance()->AddMessage(NetMessageActivateUnit(myUnits[myUnitIndex]->GetGID(), spawnPosition));
					myUnitIndex++;
					myActiveCount++;
				}
			}
		}
		mySpawnTimer = myData.mySpawnInterval;
	}
}
