#pragma once
#include "Component.h"

struct ProjectileComponentData;

class ProjectileComponent : public Component
{
public:
	ProjectileComponent(Entity& anEntity, const ProjectileComponentData& aData, CU::Matrix44<float>& anOrientation);
	~ProjectileComponent();


	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	void SetForward(const CU::Vector3<float>& aForwardVector);
	void SetPosition(const CU::Vector3<float>& aForwardVector);
	void Activate();
private:
	const ProjectileComponentData& myData;
	CU::Matrix44<float>& myOrientation;
	bool myShouldUpdate;
};


inline eComponentType ProjectileComponent::GetTypeStatic()
{
	return eComponentType::BULLET;
}

inline eComponentType ProjectileComponent::GetType()
{
	return GetTypeStatic();
}