#pragma once
#include "Component.h"

class AIComponent : public Component
{
public:
	AIComponent(Entity& anEntity, const AIComponentData& aData, CU::Matrix44<float>& anOrientation);
	~AIComponent();

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:

	void Shoot();

	void operator=(AIComponent&) = delete;
	const AIComponentData& myData;

	CU::Matrix44<float>& myOrientation;

	float myShootTime;



};

inline eComponentType AIComponent::GetTypeStatic()
{
	return eComponentType::AI;
}

inline eComponentType AIComponent::GetType()
{
	return GetTypeStatic();
}