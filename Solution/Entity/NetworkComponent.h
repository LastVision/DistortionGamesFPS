#pragma once
#include "Component.h"
class NetworkComponent : public Component
{
public:
	NetworkComponent(Entity& anEntity);
	~NetworkComponent();



	const unsigned int GetNetworkID() const;
	void SetNetworkID(unsigned int anID);
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	unsigned int myNetworkID;

};

inline eComponentType NetworkComponent::GetTypeStatic()
{
	return eComponentType::NETWORK;
}

inline eComponentType NetworkComponent::GetType()
{
	return GetTypeStatic();
}