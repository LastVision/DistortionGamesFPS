#pragma once
#include "Component.h"
class NetworkComponent : public Component
{
public:
	NetworkComponent(Entity& anEntity);
	~NetworkComponent();



	const unsigned int GetNetworkID() const;
	void SetNetworkID(unsigned int anID);
private:

	unsigned int myNetworkID;



};

