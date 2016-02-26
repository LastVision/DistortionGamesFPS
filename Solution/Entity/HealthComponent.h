#pragma once
#include "Component.h"

class Entity;
struct HealthComponentData;

class HealthComponent : public Component
{
public:
	HealthComponent(Entity& anEntity, const HealthComponentData& someData);
	~HealthComponent();

	void TakeDamage(float aDamage);
	void Heal(float anAmount);

	void Reset() override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const HealthComponentData& myData;
	float myCurrentHealth;
};

inline static eComponentType GetTypeStatic()
{
	return eComponentType::HEALTH;
}

inline eComponentType GetType()
{
	return GetTypeStatic();
}