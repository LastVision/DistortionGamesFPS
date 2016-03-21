#include "stdafx.h"
#include "AIComponent.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include <NetMessagePosition.h>
#include <SharedNetworkManager.h>
#include "PollingStation.h"
AIComponent::AIComponent(Entity& anEntity, const AIComponentData& aData, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myData(aData)
	, myOrientation(anOrientation)
{
	myShootTime = 1.f;
}

AIComponent::~AIComponent()
{
}

void AIComponent::Update(float aDelta)
{
	CU::Vector3<float> movement(1.f, 0.f, 0.f);
	movement *= 0.13f;
	Prism::PhysicsInterface::GetInstance()->Move(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), movement, 0.05f, aDelta);
	CU::Vector3<float> pos;
	Prism::PhysicsInterface::GetInstance()->GetPosition(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), pos);
	myOrientation.SetPos(pos);

	myShootTime -= aDelta;
	if (myShootTime < 0.f)
	{
		Shoot();
		myShootTime = 1.f;
	}
}

void AIComponent::Shoot()
{

}