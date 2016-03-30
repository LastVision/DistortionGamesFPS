#include "stdafx.h"
#include "CollisionNote.h"
#include "DamageNote.h"
#include <EnemyKilledMessage.h>
#include "HealthComponent.h"
#include "HealthComponentData.h"
#include "HealthNote.h"
#include <NetMessageHealth.h>
#include <NetMessageOnHit.h>
#include <NetMessageEntityState.h>
#include <NetMessageSetActive.h>
#include "PhysicsComponent.h"
#include "PollingStation.h"
#include <PostMaster.h>
#include <SharedNetworkManager.h>
#include <RespawnTriggerMessage.h>
#include "TriggerComponent.h"

HealthComponent::HealthComponent(Entity& anEntity, const HealthComponentData& someData)
	: Component(anEntity)
	, myData(someData)
	, myCurrentHealth(someData.myMaxHealth)
{
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_HIT, this);
	if (myEntity.GetSubType() == "playerserver")
	{
		SharedNetworkManager::GetInstance()->AddMessage<NetMessageHealth>(NetMessageHealth(someData.myMaxHealth, myCurrentHealth, myEntity.GetGID()), myEntity.GetGID());
	}
}

HealthComponent::~HealthComponent()
{
	SharedNetworkManager::GetInstance()->UnSubscribe(eNetMessageType::ON_HIT, this);
}

void HealthComponent::ReceiveNote(const DamageNote& aNote)
{
	TakeDamage(aNote.myDamage);
}

void HealthComponent::ReceiveNote(const HealthNote& aNote)
{
	Heal(aNote.myAmount);
}

void HealthComponent::ReceiveNote(const CollisionNote& aNote)
{
	if (aNote.myOther->GetComponent<TriggerComponent>()->GetTriggerType() == eTriggerType::HEALTH_PACK)
	{
		Heal(aNote.myOther->GetComponent<TriggerComponent>()->GetValue());

	}
}

void HealthComponent::TakeDamage(int aDamage)
{
	myCurrentHealth -= aDamage;
	if (myEntity.GetSubType() == "playerserver")
	{
		SharedNetworkManager::GetInstance()->AddMessage<NetMessageHealth>(NetMessageHealth(myData.myMaxHealth, myCurrentHealth, myEntity.GetGID()), myEntity.GetGID());
	}
	if (myEntity.GetIsClient() == false)
	{
		SharedNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(aDamage, myEntity.GetGID()));
	}

	if (myCurrentHealth <= 0)
	{
		myCurrentHealth = 0;

		if (myEntity.GetIsClient() == false)
		{	
			if (myEntity.GetSubType() == "playerserver")
			{
				PostMaster::GetInstance()->SendMessage<RespawnTriggerMessage>(RespawnTriggerMessage(myEntity.GetGID()));
			}
			else
			{
				PostMaster::GetInstance()->SendMessage(EnemyKilledMessage());
			}
			myEntity.SetState(eEntityState::DIE);
			SharedNetworkManager::GetInstance()->AddMessage(NetMessageEntityState(myEntity.GetState(), myEntity.GetGID()));
			SharedNetworkManager::GetInstance()->AddMessage(NetMessageSetActive(false, false, myEntity.GetGID()));
			myEntity.GetComponent<PhysicsComponent>()->Sleep();
			if (myEntity.GetSubType() != "playerserver")
			{
				myEntity.Kill(false);
			}
			PollingStation::GetInstance()->HasDied(&myEntity);
		}
	}
}

void HealthComponent::ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in&)
{
	if (myEntity.GetIsClient() == false && myEntity.GetGID() == aMessage.myGID)
	{
		TakeDamage(aMessage.myDamage);
	}
}

void HealthComponent::Heal(int anAmount)
{
	myCurrentHealth += anAmount;
	if (myCurrentHealth > myData.myMaxHealth)
	{
		myCurrentHealth = myData.myMaxHealth;
	}
	if (myEntity.GetSubType() == "playerserver")
	{
		SharedNetworkManager::GetInstance()->AddMessage<NetMessageHealth>(NetMessageHealth(myData.myMaxHealth, myCurrentHealth, myEntity.GetGID()), myEntity.GetGID());
	}
}

void HealthComponent::Reset()
{
	myCurrentHealth = myData.myMaxHealth;
}

int HealthComponent::GetMaxHealth()
{
	return myData.myMaxHealth;
}