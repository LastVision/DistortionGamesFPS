#pragma once
#include "Component.h"

class Behavior;

class AIComponent : public Component
{
public:
	AIComponent(Entity& anEntity, const AIComponentData& aData, CU::Matrix44<float>& anOrientation);
	~AIComponent();

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	void operator=(AIComponent&) = delete;

	const AIComponentData& myData;

	Behavior* myBehavior;

	CU::Matrix44<float>& myOrientation;

};

inline eComponentType AIComponent::GetTypeStatic()
{
	return eComponentType::AI;
}

inline eComponentType AIComponent::GetType()
{
	return GetTypeStatic();
}