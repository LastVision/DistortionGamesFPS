#pragma once
#include "Component.h"
#include <NetworkSubscriber.h>
class Entity;
struct HealthComponentData;

class HealthComponent : public Component, public NetworkSubscriber
{
public:
	HealthComponent(Entity& anEntity, const HealthComponentData& someData);
	~HealthComponent();

	void ReceiveNote(const DamageNote& aNote) override;
	void ReceiveNote(const HealthNote& aNote) override;
	void ReceiveNote(const CollisionNote& aNote) override;

	void TakeDamage(int aDamage);
	void HealthComponent::ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress);
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