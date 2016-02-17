#include "stdafx.h"
#include "ShootingComponent.h"

ShootingComponent::ShootingComponent(Entity& aEntity)
	: Component(aEntity)
{
}

ShootingComponent::~ShootingComponent()
{
}

void ShootingComponent::ReceiveNote(const ShootNote& aNote)
{

}