#include "stdafx.h"
#include "ProjectileComponent.h"
#include "PhysicsComponent.h"

ProjectileComponent::ProjectileComponent(Entity& aEntity, const ProjectileComponentData& aComponentData)
	: Component(aEntity)
	, myData(aComponentData)
	, myTimeUntilExplode(2.f)
	, myShouldBeUpdated(false)
{
}

ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Update(float aDelta)
{
	if (myShouldBeUpdated == true)
	{
		myTimeUntilExplode -= aDelta;
		if (myTimeUntilExplode <= 0.f)
		{
			myEntity.RemoveFromScene();
			myEntity.GetComponent<PhysicsComponent>()->Sleep();
			myShouldBeUpdated = false;
		}
	}
}

void ProjectileComponent::Activate()
{
	myTimeUntilExplode = 2.f;
	myShouldBeUpdated = true;
	if (myEntity.IsInScene() == false)
	{
		myEntity.AddToScene();
		myEntity.GetComponent<PhysicsComponent>()->Wake();
	}
}