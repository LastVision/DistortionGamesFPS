#include "stdafx.h"
#include "NetworkComponent.h"
#include "Entity.h"


NetworkComponent::NetworkComponent(Entity& anEntity)
	: Component(anEntity)
{
}


NetworkComponent::~NetworkComponent()
{
}

const unsigned int NetworkComponent::GetNetworkID() const
{
	return myNetworkID;
}
