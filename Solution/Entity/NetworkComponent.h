#pragma once
#include "Component.h"
#include "Subscriber.h"
class NetworkComponent : public Component, public Subscriber
{
public:
	NetworkComponent(Entity& anEntity, CU::Matrix44<float>& anOrientation, unsigned int& aNetworkID);
	~NetworkComponent();
	void Reset() override;

	void SetNetworkID(unsigned int anID);
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;


	void Update(float aDelta) override;

	void ReceiveMessage(const NetworkSetPositionMessage& aMessage) override;
	void ReceiveMessage(const NetworkOnHitMessage& aMessage) override;

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

	unsigned int& myNetworkID;

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