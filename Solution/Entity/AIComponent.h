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

	void Shoot();

	void operator=(AIComponent&) = delete;
	void Move(float aDelta);
	void SetOrientation(const CU::Vector3<float>& aLookInDirection);

	const AIComponentData& myData;

	Behavior* myBehavior;

	CU::Matrix44<float>& myOrientation;

	float myShootTime;

	CU::GrowingArray<Entity*> myBullets;
	int myBulletIndex;



};

inline eComponentType AIComponent::GetTypeStatic()
{
	return eComponentType::AI;
}

inline eComponentType AIComponent::GetType()
{
	return GetTypeStatic();
}