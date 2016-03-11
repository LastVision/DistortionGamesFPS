#pragma once
#include "Component.h"

struct ProjectileComponentData;
class ProjectileComponent : public Component
{
public:
	ProjectileComponent(Entity& aEntity, const ProjectileComponentData& aComponentData);
	~ProjectileComponent();

	void Update(float aDelta) override;
	void Activate();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	bool GetShouldBeUpdated() const;

private:
	const ProjectileComponentData& myData;
	float myTimeUntilExplode;
	bool myShouldBeUpdated;
};

inline eComponentType ProjectileComponent::GetTypeStatic()
{
	return eComponentType::PROJECTILE;
}

inline eComponentType ProjectileComponent::GetType()
{
	return GetTypeStatic();
}

inline bool ProjectileComponent::GetShouldBeUpdated() const
{
	return myShouldBeUpdated;
}