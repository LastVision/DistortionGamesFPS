#pragma once
#include "Component.h"

namespace Prism
{
	class Scene;
}
struct ProjectileComponentData;
class ProjectileComponent : public Component
{
public:
	ProjectileComponent(Entity& aEntity, const ProjectileComponentData& aComponentData, Prism::Scene* aScene);
	~ProjectileComponent();

	void Update(float aDelta) override;
	void Activate(unsigned int aShooterGID);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	bool GetShouldBeUpdated() const;
	bool HasExploded() const;

	void SetHasExploded(bool aHasExploded);

private:
	const ProjectileComponentData& myData;
	float myTimeUntilExplode;
	bool myShouldBeUpdated;
	bool myHasExploded;
	bool myShouldDeleteExplosion;
	bool myShouldReallyDeleteExplosion;

	Entity* myExplosion;

	unsigned int myShooterGID;
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

inline bool ProjectileComponent::HasExploded() const
{
	return myHasExploded;
}

inline void ProjectileComponent::SetHasExploded(bool aHasExploded)
{
	myHasExploded = aHasExploded;
}