#pragma once
#include "Component.h"
class Entity;
struct RotationComponentData;

class RotationComponent : public Component
{
public:
	RotationComponent(Entity& anEntity, const RotationComponentData& someData);
	~RotationComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const RotationComponentData& myData;
	float myStartRotation;
};

inline eComponentType RotationComponent::GetTypeStatic()
{
	return eComponentType::ROTATION;
}

inline eComponentType RotationComponent::GetType()
{
	return GetTypeStatic();
}