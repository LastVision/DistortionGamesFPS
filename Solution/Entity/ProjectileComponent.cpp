#include "stdafx.h"
#include "ProjectileComponent.h"
#include "PhysicsComponent.h"
	#include "Entity.h"

ProjectileComponent::ProjectileComponent(Entity& anEntity, const ProjectileComponentData& aData, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myOrientation(anOrientation)
	, myData(aData)
	, myShouldUpdate(false)
{
}


ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Update(float aDelta)
{
	if (myShouldUpdate == true)
	{
		//myEntity.GetComponent<PhysicsComponent>()->AddForce(myOrientation.GetForward(), myData.mySpeed);
	}
}

void ProjectileComponent::SetForward(const CU::Vector3<float>& aForwardVector)
{
	myOrientation.SetForward(aForwardVector);
	myEntity.GetComponent<PhysicsComponent>()->MoveToPosition(myOrientation.GetPos());
}

void ProjectileComponent::SetPosition(const CU::Vector3<float>& aForwardVector)
{
	myOrientation.SetPos(aForwardVector);
}

void ProjectileComponent::Activate()
{
	myShouldUpdate = true;
	myEntity.GetComponent<PhysicsComponent>()->Wake();
	//myEntity.AddToScene();
}
