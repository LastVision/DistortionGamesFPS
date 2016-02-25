#pragma once
#include "Component.h"

struct ProjectileComponentData;
class ProjectileComponent : public Component
{
public:
	ProjectileComponent(Entity& aEntity, const ProjectileComponentData& aComponentData);
	~ProjectileComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const ProjectileComponentData& myData;
};

inline eComponentType ProjectileComponent::GetTypeStatic()
{
	return eComponentType::PROJECTILE;
}

inline eComponentType ProjectileComponent::GetType()
{
	return GetTypeStatic();
}