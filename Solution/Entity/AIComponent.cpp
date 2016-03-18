#include "stdafx.h"
#include "AIComponent.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>

AIComponent::AIComponent(Entity& anEntity, const AIComponentData& aData)
	: Component(anEntity)
	, myData(aData)
{
	int apa = 5;
}


AIComponent::~AIComponent()
{
}

void AIComponent::Update(float aDelta)
{
	CU::Vector3<float> movement(0.f, 0.f, 1.f);
	movement *= 0.13f;
	Prism::PhysicsInterface::GetInstance()->Move(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), movement, 0.05f, aDelta);
}
