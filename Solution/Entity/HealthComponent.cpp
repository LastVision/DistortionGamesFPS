#include "stdafx.h"
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

void HealthComponent::TakeDamage(float aDamage)
{
	myCurrentHealth -= aDamage;
	if (myCurrentHealth <= 0.f)
	{
		myCurrentHealth = 0.f;
		myEntity.Kill();
	}
}

void HealthComponent::Heal(float anAmount)
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