#pragma once
#include "Component.h"
class Entity;
struct VisualExplosionComponentData;

class VisualExplosionComponent : public Component
{
public:
	VisualExplosionComponent(Entity& anEntity, const VisualExplosionComponentData& someData, CU::Matrix44<float>& anOrientation);
	~VisualExplosionComponent();

	void Update(float aDeltaTime) override;

	void SetPosition(const CU::Vector3<float>& aPosition);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const VisualExplosionComponentData& myData;
	float myTotalTime;
	CU::Matrix44<float>& myOrientation;
};

inline eComponentType VisualExplosionComponent::GetTypeStatic()
{
	return eComponentType::VISUAL_EXPLOSION;
}

inline eComponentType VisualExplosionComponent::GetType()
{
	return GetTypeStatic();
}