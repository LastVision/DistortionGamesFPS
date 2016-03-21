#include "stdafx.h"
#include "ProjectileComponent.h"
#include "PhysicsComponent.h"
#include "Entity.h"

ProjectileComponent::ProjectileComponent(Entity& anEntity, const ProjectileComponentData& aData, CU::Matrix44<float>& anOrientation)
	: Component(anEntity)
	, myOrientation(anOrientation)
	, myData(aData)
{
	myEntity.Kill(false);
}


ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Activate(const CU::Matrix44<float>& anOrientation)
{
	myEntity.Reset();
	myLifetimeLeft = myData.myLifetime;
	myOrientation = anOrientation;
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(0, 0, 1.f) * myOrientation + CU::Vector3<float>(0, 1.f, 0));
	//myEntity.GetComponent<PhysicsComponent>()->Wake();

}

void ProjectileComponent::Update(float aDelta)
{
	myLifetimeLeft -= aDelta;
	if (myLifetimeLeft < 0)
	{
		myEntity.Kill();
	}
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(0, 0, myData.mySpeed * aDelta) * myOrientation);
	myEntity.GetComponent<PhysicsComponent>()->TeleportToPosition(myEntity.GetOrientation().GetPos());
}

int ProjectileComponent::GetDamage() const
{
	return myData.myDamage;
}