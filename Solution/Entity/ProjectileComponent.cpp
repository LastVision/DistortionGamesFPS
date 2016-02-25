#include "stdafx.h"
#include "ProjectileComponent.h"


ProjectileComponent::ProjectileComponent(Entity& aEntity, const ProjectileComponentData& aComponentData)
	: Component(aEntity)
	, myData(aComponentData)
{
}


ProjectileComponent::~ProjectileComponent()
{
}
