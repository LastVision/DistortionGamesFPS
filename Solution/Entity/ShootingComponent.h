#pragma once
#include "Component.h"

class ShootingComponent : public Component
{
public:
	ShootingComponent(Entity& aEntity);
	~ShootingComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void ReceiveNote(const ShootNote& aNote) override;

private:
	
	// ShootingComponentData
};

inline eComponentType ShootingComponent::GetTypeStatic()
{
	return eComponentType::SHOOTING;
}

inline eComponentType ShootingComponent::GetType()
{
	return GetTypeStatic();
}