#pragma once
#include "Component.h"
#include <NetworkSubscriber.h>
class NetworkComponent : public Component, public NetworkSubscriber
{
public:
	NetworkComponent(Entity& anEntity, CU::Matrix44<float>& anOrientation);
	~NetworkComponent();
	void Reset() override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void Update(float aDelta) override;

	void ReceiveNetworkMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress) override;

	void SetPlayer(bool aBool);

private:
	void ClientUpdate(float aDelta);
	void ServerUpdate(float aDelta);

	CU::Matrix44<float>& myOrientation;
	CU::Vector3<float> myServerPosition;
	CU::Vector3<float> myPrevPosition;

	float myCurrentRotationY;
	float myServerRotationY;
	float myPrevRotationY;

	CU::Vector3<float> myFirstPosition;
	CU::Vector3<float> mySecondPosition;
	CU::Vector3<float> mySecondPosition2;

	bool myIsPlayer;
	bool myShouldReturn;
	bool myShouldUpdate;

	float mySendTime;
	float myAlpha;
};

inline eComponentType NetworkComponent::GetTypeStatic()
{
	return eComponentType::NETWORK;
}

inline eComponentType NetworkComponent::GetType()
{
	return GetTypeStatic();
}