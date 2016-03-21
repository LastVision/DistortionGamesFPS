#pragma once
#include "Component.h"

namespace Prism
{
	class Scene;
}
struct GrenadeComponentData;
class GrenadeComponent : public Component
{
public:
	GrenadeComponent(Entity& aEntity, const GrenadeComponentData& aComponentData, Prism::Scene* aScene);
	~GrenadeComponent();

	void Update(float aDelta) override;
	void Activate(unsigned int aShooterGID);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	bool GetShouldBeUpdated() const;
	bool HasExploded() const;

	void SetHasExploded(bool aHasExploded);

private:
	const GrenadeComponentData& myData;
	float myTimeUntilExplode;
	bool myShouldBeUpdated;
	bool myHasExploded;
	bool myShouldDeleteExplosion;
	bool myShouldReallyDeleteExplosion;

	Entity* myExplosion;

	unsigned int myShooterGID;
};

inline eComponentType GrenadeComponent::GetTypeStatic()
{
	return eComponentType::GRENADE;
}

inline eComponentType GrenadeComponent::GetType()
{
	return GetTypeStatic();
}

inline bool GrenadeComponent::GetShouldBeUpdated() const
{
	return myShouldBeUpdated;
}

inline bool GrenadeComponent::HasExploded() const
{
	return myHasExploded;
}

inline void GrenadeComponent::SetHasExploded(bool aHasExploded)
{
	myHasExploded = aHasExploded;
}