#include "stdafx.h"
#include "Entity.h"
#include "RotationComponent.h"
#include "RotationComponentData.h"
#include <MathHelper.h>

RotationComponent::RotationComponent(Entity& anEntity, const RotationComponentData& someData)
	: Component(anEntity)
	, myData(someData)
{
	DL_ASSERT_EXP(myEntity.GetIsClient() == true, "Can't rotate on server.");
	myStartRotation = CU::Math::RandomRange<float>(0.f, 3.14f * 2.f);
	myEntity.SetRotation(CU::Vector3<float>(0.f, myStartRotation, 0.f));
}

RotationComponent::~RotationComponent()
{
}

void RotationComponent::Update(float aDeltaTime)
{
	myEntity.SetRotation(CU::Vector3<float>(0.f, myData.myRotationSpeed * aDeltaTime, 0.f));
}