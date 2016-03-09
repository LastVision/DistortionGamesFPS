#pragma once
#include "Component.h"

class Entity;
struct HealthComponentData;

class HealthComponent : public Component
{
public:
	HealthComponent(Entity& anEntity, const HealthComponentData& someData);
	~HealthComponent();

	void ReceiveNote(const DamageNote& aNote) override;

	void TakeDamage(int aDamage);
	void Heal(int anAmount);

	void Reset() override;

	int GetCurrentHealth();
	int GetMaxHealth();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const HealthComponentData& myData;
	int myCurrentHealth;
};

inline eComponentType HealthComponent::GetTypeStatic()
{
	return eComponentType::HEALTH;
}

inline eComponentType HealthComponent::GetType()
{
	return GetTypeStatic();
}

inline int HealthComponent::GetCurrentHealth()
{
	return myCurrentHealth;
}