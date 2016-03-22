#pragma once
#include "Component.h"

struct ProjectileComponentData;

class ProjectileComponent : public Component
{
public:
	ProjectileComponent(Entity& anEntity, const ProjectileComponentData& aData, CU::Matrix44<float>& anOrientation);
	~ProjectileComponent();

	void Activate(const CU::Matrix44<float>& anOrientation);

	void Update(float aDelta) override;

	int GetDamage() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const ProjectileComponentData& myData;
	CU::Matrix44<float>& myOrientation;

	float myLifetimeLeft;
};


inline eComponentType ProjectileComponent::GetTypeStatic()
{
	return eComponentType::BULLET;
}

inline eComponentType ProjectileComponent::GetType()
{
	return GetTypeStatic();
}