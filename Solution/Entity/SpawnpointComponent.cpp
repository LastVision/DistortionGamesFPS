#include "stdafx.h"
#include "SpawnpointComponent.h"
#include "SpawnpointComponentData.h"

#include "Entity.h"
#include <EnemyKilledMessage.h>

#include <MathHelper.h>
#include "SharedUnitManager.h"

#include <SharedNetworkManager.h>
#include <NetMessageActivateUnit.h>

#include <PostMaster.h>
#include <ActivateSpawnpointMessage.h>
#include <EmitterMessage.h>
SpawnpointComponent::SpawnpointComponent(Entity& anEntity, const SpawnpointComponentData& aSpawnpointComponentData)
	: Component(anEntity)
	, myData(aSpawnpointComponentData)
	, myIsActive(false)
	//, myUnits(512)
	, myTriggerConnections(4)
	, myBoundToTiggers(false)
	, myActivateOnce(false)
{
	myUnitManager = SharedUnitManager::GetInstance();
	PostMaster::GetInstance()->Subscribe(eMessageType::ACTIVATE_SPAWNPOINT, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::ENEMY_KILLED, this);

	/*myUnitCount = int((myData.mySpawnPerInterval * myData.mySpawnpointLifetime) / myData.mySpawnInterval);*/
	mySpawnTimer = myData.mySpawnInterval;
	myLifetime = myData.mySpawnpointLifetime;

	//for (int i = 0; i < myData.myUnitCount; ++i)
	//{
	//	int randomType = CU::Math::RandomRange(0, aSpawnpointComponentData.myUnitTypes.Size());
	//	std::string server = "";
	//	if (myEntity.GetIsClient() == false)
	//	{
	//		server = "Server";
	//	}
	//	Entity* toAdd = SharedUnitManager::GetInstance()->RequestUnit(myData.myUnitTypes[randomType] + server);
	//	if (toAdd != nullptr)
	//	{
	//		//myUnits.Add(toAdd);
	//	}
	//	else
	//	{
	//		DL_ASSERT("Could not find enemy to add in spawncomponent!");
	//	}
	//}
}


SpawnpointComponent::~SpawnpointComponent()
{
	//myUnits.RemoveAll();
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ACTIVATE_SPAWNPOINT, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ENEMY_KILLED, this);
}

void SpawnpointComponent::Update(float aDelta)
{
	if (myIsActive == false && myBoundToTiggers == false && myActivateOnce == false)
	{
		Activate();
		myActivateOnce = true;
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
	myIsActive = false;
}

void SpawnpointComponent::ReceiveMessage(const ActivateSpawnpointMessage& aMessage)
{
	if (myEntity.GetGID() == aMessage.myGID)
	{
		if (myIsActive == false)
		{
			Activate();
			return;
		}
	}
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

void SpawnpointComponent::ReceiveMessage(const EnemyKilledMessage& aMessage)
{
	for each (Entity* unit in myUnitManager->GetUnits())
	{
		if (unit->GetGID() == aMessage.myEnemy->GetGID())
		{
			myActiveCount--;
			return;
		}
	}
}

void SpawnpointComponent::BindToTrigger(unsigned int aGID)
{
	myBoundToTiggers = true;
	myTriggerConnections.Add(aGID);
}

void SpawnpointComponent::SpawnUnit(float aDelta)
{
	mySpawnTimer -= aDelta;
	if (mySpawnTimer < 0.f)
	{
		printf("Active units from spawnpoint %i", myActiveCount);

		for (int i = 0; i < myData.mySpawnPerInterval; ++i)
		{
			if (myActiveCount < myData.myUnitCount)
			{
				CU::Vector3<float> spawnPosition = myEntity.GetOrientation().GetPos();
				int randomType = CU::Math::RandomRange(0, myData.myUnitTypes.Size());

				Entity* toActivate = myUnitManager->RequestUnit(myData.myUnitTypes[randomType] + "Server");
				if (toActivate != nullptr)
				{
					SharedUnitManager::GetInstance()->ActivateUnit(toActivate, spawnPosition);
					SharedNetworkManager::GetInstance()->AddMessage(NetMessageActivateUnit(toActivate->GetGID(), spawnPosition));
					
					myActiveCount++;
				}
			}
		}
		mySpawnTimer = myData.mySpawnInterval;
	}
}
