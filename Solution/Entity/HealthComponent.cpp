#include "stdafx.h"
#include "CollisionNote.h"
#include "DamageNote.h"
#include <EnemyKilledMessage.h>
#include "HealthComponent.h"
#include "HealthComponentData.h"
#include "HealthNote.h"
#include <NetMessageOnHit.h>
#include <NetMessageEntityState.h>
#include <NetMessageSetActive.h>
#include "TriggerComponent.h"
#include <PostMaster.h>
#include <SharedNetworkManager.h>

HealthComponent::HealthComponent(Entity& anEntity, const HealthComponentData& someData)
	: Component(anEntity)
	, myData(someData)
	, myCurrentHealth(someData.myMaxHealth)
{
	SharedNetworkManager::GetInstance()->Subscribe(eNetMessageType::ON_HIT, this);
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

	if (myEntity.GetIsClient() == false)
	{
		SharedNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(aDamage, myEntity.GetGID()));
	}

	if (myCurrentHealth <= 0)
	{
		myCurrentHealth = 0;
		//myEntity.Kill();

		if (myEntity.GetIsClient() == false)
		{
			myEntity.SetState(eEntityState::DIE);
			SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(myEntity.GetState(), myEntity.GetGID()));
			//SharedNetworkManager::GetInstance()->AddMessage<NetMessageSetActive>(NetMessageSetActive(false, myEntity.GetGID()));
		}
	}
}

void HealthComponent::ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress)
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
}

void HealthComponent::Reset()
{
	myCurrentHealth = myData.myMaxHealth;
}

int HealthComponent::GetMaxHealth()
{
	return myData.myMaxHealth;
}