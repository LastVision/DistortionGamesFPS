#include "stdafx.h"
#include "ProjectileComponent.h"


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
			myShouldBeUpdated = false;
		}
	}
}

void ProjectileComponent::Activate()
{
	myTimeUntilExplode = 2.f;
	myShouldBeUpdated = true;
}