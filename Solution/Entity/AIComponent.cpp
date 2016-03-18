#include "stdafx.h"
#include "AIComponent.h"


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

}
