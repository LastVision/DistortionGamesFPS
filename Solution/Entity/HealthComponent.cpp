#include "stdafx.h"
#include "DamageNote.h"
#include "HealthComponent.h"
#include "HealthComponentData.h"

HealthComponent::HealthComponent(Entity& anEntity, const HealthComponentData& someData)
	: Component(anEntity)
	, myData(someData)
	, myCurrentHealth(someData.myMaxHealth)
{
}

HealthComponent::~HealthComponent()
{
}

void HealthComponent::ReceiveNote(const DamageNote& aNote)
{
	TakeDamage(aNote.myDamage);
}

void HealthComponent::TakeDamage(int aDamage)
{
	myCurrentHealth -= aDamage;
	if (myCurrentHealth <= 0)
	{
		myCurrentHealth = 0;
		myEntity.Kill();
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