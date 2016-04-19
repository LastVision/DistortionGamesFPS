#include "stdafx.h"

#include "AIComponent.h"
#include "HealthComponent.h"
#include <NetMessageLevelComplete.h>
#include "PollingStation.h"
#include <SharedNetworkManager.h>

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
		bool alive = player->GetComponent<HealthComponent>()->GetCurrentHealth() > 0;

		float distance2 = CU::Length2(player->GetOrientation().GetPos() - aPosition);
		if (alive && distance2 < aMaxRange * aMaxRange && distance2 < currentMinDistance2)
		{
			currentMinDistance2 = distance2;
			toReturn = player;
		}
	}

	return toReturn;
}

void PollingStation::HasDied(Entity* anEntity)
{
	if (anEntity->GetSubType() == "playerserver")
	{
		bool allPlayersDead = true;
		for each(Entity* player in myPlayers)
		{
			if (player->GetState() != eEntityState::DIE)
			{
				allPlayersDead = false;
				break;
			}
		}

		if (allPlayersDead == true)
		{
			SharedNetworkManager::GetInstance()->AddMessage(NetMessageLevelComplete(true));
		}
	}
	else
	{
		//DL_ASSERT("No handle for enemy death yet.");
		//handle dead enemies here
	}
}

void PollingStation::SetEnemyTargetPosition(Entity* aEntityToTarget)
{
	/*for each (Entity* enemy in myEnemies)
	{
		enemy->GetComponent<AIComponent>()->SetDefendTarget(aEntityToTarget);
	}*/
	myCurrentDefendTarget = aEntityToTarget;
}

void PollingStation::ResetEnemyTargetPosition()
{
	/*for each (Entity* enemy in myEnemies)
	{
		enemy->GetComponent<AIComponent>()->ResetDefendTarget();
	}*/
	myCurrentDefendTarget = nullptr;
}

Entity* PollingStation::GetCurrentDefendTarget(const CU::Vector3<float>& aEnemyPosition) const
{
	Entity* toReturn = nullptr;

	float currentMinDistance2 = FLT_MAX;
	float defendAggroRange = 30.f;

	if (myCurrentDefendTarget != nullptr)
	{
		float distance2 = CU::Length2(myCurrentDefendTarget->GetOrientation().GetPos() - aEnemyPosition);
		if (distance2 < defendAggroRange * defendAggroRange && distance2 < currentMinDistance2)
		{
			currentMinDistance2 = distance2;
			toReturn = myCurrentDefendTarget;
		}
	}

	return toReturn;
}