#include "stdafx.h"
#include "HealthComponent.h"
#include "HealthComponentData.h"

HealthComponent::HealthComponent(Entity& anEntity, const HealthComponentData& someData)
	: Component(anEntity)
	, myMaxHealth(someData.myMaxHealth)
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
	if (myCurrentHealth > myMaxHealth)
	{
		myCurrentHealth = myMaxHealth;
	}
}

void HealthComponent::Reset()
{
	myCurrentHealth = myMaxHealth;
}